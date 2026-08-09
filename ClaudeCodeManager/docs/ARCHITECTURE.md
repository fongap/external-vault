# Architecture

```text
ClaudeCodeManager.exe
  ├─ project and settings selection
  ├─ Windows Credential Manager
  ├─ proxy and gateway environment injection
  ├─ native install / update / doctor entry points
  └─ --claude-launcher <project>
       └─ native claude.exe
            ├─ sessions and authentication
            ├─ model routing and context compression
            ├─ MCP and updates
            └─ runtime recovery
```

The launcher deliberately stays outside the model request path. This keeps the
configuration surface aligned with the public Claude Code CLI and avoids a second
session, authentication or context-management implementation.

Before accepting a third-party gateway, the Manager probes the Anthropic
`/v1/messages` route with an empty invalid payload. This confirms protocol routing
without performing model inference. A working `/v1/models` endpoint alone is not
treated as Claude Code compatibility.

## Settings boundary

Only fields supported by Claude Code are written into its settings files, such as
the selected model, supported model mapping environment variables,
`ANTHROPIC_BASE_URL` and the primary model `effortLevel`.

Context capacity labels and budgets belong to the Manager UI and are stored under
`HKCU\Software\Fongap\ClaudeCodeManager`. They are advisory unless the selected
gateway independently enforces equivalent limits.

Credential values remain in Windows Credential Manager. The registry stores a
separate list of credentials active for the current provider, so changing from a
gateway to account login does not inject stale bearer tokens into the native CLI.
