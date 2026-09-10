#!/usr/bin/env bash
set -Eeuo pipefail

required_commands=(oci jq)
for command_name in "${required_commands[@]}"; do
  if ! command -v "$command_name" >/dev/null 2>&1; then
    echo "::error::Required command not found: $command_name"
    exit 1
  fi
done

required_env=(
  OCI_CLI_USER
  OCI_CLI_TENANCY
  OCI_CLI_FINGERPRINT
  OCI_CLI_KEY_CONTENT
  OCI_CLI_REGION
  OCI_SHAPE
  OCI_OCPUS
  OCI_MEMORY_GB
)

for name in "${required_env[@]}"; do
  if [[ -z "${!name:-}" ]]; then
    echo "::error::Required environment variable is not set: $name"
    exit 1
  fi
done

if ! jq -en --arg value "$OCI_OCPUS" '$value | tonumber | select(. > 0)' >/dev/null; then
  echo "::error::OCI_OCPUS must be a number greater than 0."
  exit 1
fi

if ! jq -en --arg value "$OCI_MEMORY_GB" '$value | tonumber | select(. > 0)' >/dev/null; then
  echo "::error::OCI_MEMORY_GB must be a number greater than 0."
  exit 1
fi

shape_payload="$({
  jq -cn \
    --arg shape "$OCI_SHAPE" \
    --arg ocpus "$OCI_OCPUS" \
    --arg memory "$OCI_MEMORY_GB" \
    '[{
      instanceShape: $shape,
      instanceShapeConfig: {
        ocpus: ($ocpus | tonumber),
        memoryInGBs: ($memory | tonumber)
      }
    }]'
})"

ads=()
if [[ -n "${OCI_AVAILABILITY_DOMAINS:-}" ]]; then
  IFS=',' read -r -a raw_ads <<< "$OCI_AVAILABILITY_DOMAINS"
  for ad in "${raw_ads[@]}"; do
    ad="${ad#"${ad%%[![:space:]]*}"}"
    ad="${ad%"${ad##*[![:space:]]}"}"
    [[ -n "$ad" ]] && ads+=("$ad")
  done
else
  mapfile -t ads < <(
    oci iam availability-domain list \
      --compartment-id "$OCI_CLI_TENANCY" \
      --all \
      --output json \
      | jq -r '.data[].name'
  )
fi

if (( ${#ads[@]} == 0 )); then
  echo "::error::No Availability Domain was resolved."
  exit 1
fi

available_ads=()
query_errors=0

printf 'Checking %s (%s OCPU / %s GB) in region %s\n' \
  "$OCI_SHAPE" "$OCI_OCPUS" "$OCI_MEMORY_GB" "$OCI_CLI_REGION"

for ad in "${ads[@]}"; do
  echo "Checking $ad ..."

  if ! response="$(
    oci compute compute-capacity-report create \
      --availability-domain "$ad" \
      --compartment-id "$OCI_CLI_TENANCY" \
      --shape-availabilities "$shape_payload" \
      --max-retries 2 \
      --output json
  )"; then
    echo "::warning::Capacity query failed for $ad."
    query_errors=$((query_errors + 1))
    continue
  fi

  status="$(jq -r '.data["shape-availabilities"][0]["availability-status"] // "UNKNOWN"' <<< "$response")"
  count="$(jq -r '.data["shape-availabilities"][0]["available-count"] // "unknown"' <<< "$response")"

  printf '%s: %s (available-count=%s)\n' "$ad" "$status" "$count"

  if [[ "$status" == "AVAILABLE" ]]; then
    available_ads+=("$ad")
  fi
done

available=false
if (( ${#available_ads[@]} > 0 )); then
  available=true
fi

if [[ -n "${GITHUB_OUTPUT:-}" ]]; then
  {
    echo "available=$available"
    echo "available_ads<<EOF"
    printf '%s\n' "${available_ads[@]}"
    echo "EOF"
  } >> "$GITHUB_OUTPUT"
fi

if [[ -n "${GITHUB_STEP_SUMMARY:-}" ]]; then
  {
    echo "### OCI Capacity Watch"
    echo
    echo "- Region: \`$OCI_CLI_REGION\`"
    echo "- Shape: \`$OCI_SHAPE\`"
    echo "- Configuration: \`$OCI_OCPUS OCPU / $OCI_MEMORY_GB GB\`"
    echo "- Capacity available: \`$available\`"
    if (( ${#available_ads[@]} > 0 )); then
      echo "- Available ADs:"
      printf '  - `%s`\n' "${available_ads[@]}"
    fi
  } >> "$GITHUB_STEP_SUMMARY"
fi

if [[ "$available" == "true" ]]; then
  exit 0
fi

if (( query_errors > 0 )); then
  echo "::error::No capacity was confirmed and $query_errors Availability Domain query/queries failed."
  exit 1
fi

exit 0
