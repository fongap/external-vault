# external-vault

`external-vault` is a small monorepo for independently maintained utilities and generated resources from Fongap. Each project keeps its own documentation and build instructions while sharing one repository-level license policy.

## Projects

| Project | Description |
| --- | --- |
| [ClaudeCodeManager](ClaudeCodeManager/) | A lightweight Windows launcher and configuration manager that delegates sessions and model execution to the native Claude Code CLI. |
| [Fong's AdFilter](adfilter/) | An automatically generated, deduplicated advertising and annoyance filter list aggregated from multiple upstream sources. |

## License

This repository's original code and documentation are licensed under the [MIT License](LICENSE).

Third-party aggregated data (e.g., filter rules in `adfilter/`) remains subject to the terms, notices, attribution requirements, and copyright of their respective upstream projects. The MIT License in this repository does not replace or relicense third-party data.

See [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) for details.