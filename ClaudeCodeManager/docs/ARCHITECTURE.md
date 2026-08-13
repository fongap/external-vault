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

For a third-party gateway, the Manager first fetches `/v1/models` using the broad
authentication-header compatibility of V1.0. It then probes Anthropic
`/v1/messages` with a minimal valid payload containing a real discovered or
user-supplied model ID. The combined authentication headers are tried first;
401/403 responses fall back to each header independently. A working model-list
endpoint is reported separately from confirmed Claude Code Messages compatibility.

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
