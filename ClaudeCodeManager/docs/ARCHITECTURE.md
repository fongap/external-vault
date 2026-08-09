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

## Settings boundary

Only fields supported by Claude Code are written into its settings files, such as
the selected model, supported model mapping environment variables,
`ANTHROPIC_BASE_URL` and the primary model `effortLevel`.

Context capacity labels and budgets belong to the Manager UI and are stored under
`HKCU\Software\Fongap\ClaudeCodeManager`. They are advisory unless the selected
gateway independently enforces equivalent limits.
