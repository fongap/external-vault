# Architecture

```text
AgentDock.exe
  ├─ project and settings selection
  ├─ Windows Credential Manager
  ├─ proxy and gateway environment injection
  ├─ native install / update / doctor entry points
  ├─ --claude-launcher <project> ─→ native claude.exe
  └─ --codex-launcher  <project> ─→ native codex.exe / codex.cmd
            ├─ sessions and authentication
            ├─ model routing and context compression
            ├─ MCP and updates
            └─ runtime recovery
```

The launcher deliberately stays outside the model request path. This keeps the
configuration surface aligned with the public Claude Code CLI and avoids a second
session, authentication or context-management implementation.

For a third-party gateway, the Manager first fetches `/v1/models` using the broad
authentication-header compatibility of V1.0. It then probes Anthropic
`/v1/messages` with a minimal valid payload containing a real discovered or
user-supplied model ID. The combined authentication headers are tried first;
401/403 responses fall back to each header independently. A working model-list
endpoint is reported separately from confirmed Claude Code Messages compatibility.


## Codex target

The Codex target follows the same rules with protocol-specific differences:

- The live file is the user-level `~/.codex/config.toml`; Codex ignores
  project-level provider overrides, so no per-project scope exists.
- A line-level transform claims only the top-level `model`,
  `model_reasoning_effort` and `model_provider` keys plus the dedicated
  `[model_providers.ccm]` section. Everything else — sandbox, MCP, profiles,
  comments, formatting — is copied through verbatim. Invalid TOML, duplicated
  managed keys or a duplicated managed section abort the write (fail-closed).
- Gateway keys live in Windows Credential Manager under `CODEX_API_KEY` and
  are referenced from `config.toml` via `env_key`; the launcher injects the
  variable, so no secret reaches disk and `auth.json` (which may hold ChatGPT
  OAuth tokens) is never modified.
- Discovery probes `/v1/responses` with a minimal non-inferential payload
  (`model` + single-token `input`), because Codex `wire_api` only supports
  the Responses protocol; 404/405 block saving, mirroring the Messages probe.
- Switching back to ChatGPT login or model-only mode removes only the managed
  provider traces; foreign `model_provider` values and sections are preserved.

## Adding a new agent

The agent surface is driven by the `CCM_AGENTS` descriptor table (display
name, config dir/file, wire protocol, probe label). Supporting another CLI
(e.g. Gemini CLI) means: append one table entry; add a config writer beside
`codex_toml_transform`; add a takeover reader beside
`wizard_read_live_codex`; give it provider-item strings and save semantics in
`wizard_update_app` / `wizard_save`; add a launch mode in
`run_unified_console_launcher`. Profile storage, the wizard app selector,
config path resolution and the discovery probe pick the new agent up from
the table without further changes.

## Provider profiles

Each target app keeps up to 16 named profiles. Profile fields live in the
registry (`ProfilesClaude` / `ProfilesCodex`, packed with control-character
separators); the API key itself never enters the registry — it stays in
Windows Credential Manager under a stable per-profile slot (`P<id>`) and the
profile carries only the reference. Switching loads a profile into the wizard
and runs the normal save pipeline (probe, backup, atomic write, credential
activation), so profiles introduce no second write path. On first open with
an empty profile list the wizard reads the currently live config back into
the form so the user can adopt it as the first profile.

## Settings boundary

Only fields supported by Claude Code are written into its settings files, such as
the selected model, supported model mapping environment variables,
`ANTHROPIC_BASE_URL` and the primary model `effortLevel`. For Codex, only the
managed keys listed above are touched inside `config.toml`.

Context capacity labels and budgets belong to the Manager UI and are stored under
`HKCU\Software\Fongap\AgentDock` (legacy `...\ClaudeCodeManager` is read as a fallback after the rename). They are advisory unless the selected
gateway independently enforces equivalent limits.

Credential values remain in Windows Credential Manager. The registry stores a
separate list of credentials active for the current provider, so changing from a
gateway to account login does not inject stale bearer tokens into the native CLI.
