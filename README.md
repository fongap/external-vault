# external-vault

`external-vault` is a small monorepo for independently maintained utilities and generated resources from Fongap. Each project keeps its own documentation and build instructions while sharing one repository-level license policy.

## Projects

| Project | Description | Status |
| --- | --- | --- |
| [ClaudeCodeManager](ClaudeCodeManager/) | A lightweight Windows launcher and configuration manager that delegates sessions and model execution to the native Claude Code CLI. | [![Build](https://img.shields.io/github/actions/workflow/status/Fongap/external-vault/claude-code-manager.yml?label=build&logo=githubactions)](https://github.com/Fongap/external-vault/actions/workflows/claude-code-manager.yml) [![Release](https://img.shields.io/github/v/release/Fongap/external-vault?filter=claude-code-manager*&label=release&logo=github)](https://github.com/Fongap/external-vault/releases) |
| [ADFilter](ADFilter/) | An automatically generated, deduplicated advertising and annoyance filter list aggregated from multiple upstream sources. | [![Build](https://img.shields.io/github/actions/workflow/status/Fongap/external-vault/adfilter.yml?label=build&logo=githubactions)](https://github.com/Fongap/external-vault/actions/workflows/adfilter.yml) [![Rules](https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/Fongap/external-vault/main/ADFilter/stats.json&label=rules&logo=adguard)](https://github.com/Fongap/external-vault/tree/main/ADFilter) |

## License

This repository's original code and documentation are licensed under the [MIT License](LICENSE).

Third-party aggregated data (e.g., filter rules in `ADFilter/`) remains subject to the terms, notices, attribution requirements, and copyright of their respective upstream projects. The MIT License in this repository does not replace or relicense third-party data.

See [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) for details.