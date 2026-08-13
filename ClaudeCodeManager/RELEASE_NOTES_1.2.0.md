# ClaudeCodeManager 1.2.0

## 新增：多智能体（Agent Teams）全局管理

- 主窗口新增 `多智能体` 开关。
- 为当前 Windows 用户管理 `~/.claude/settings.json` 中的实验功能变量。
- 将多智能体工作规则写入 `~/.claude/CLAUDE.md` 的唯一托管区块。
- 支持幂等启停、启用前值恢复、外部改写保护、原文件备份和跨文件补偿回滚。
- 非法 JSON、重复键、损坏标记、无效编码和过大文件均拒绝覆盖。

## 作用范围

适用于当前 Windows 用户的 Claude Code CLI 与 Claude Desktop 本地 Code 会话；
不包含云端会话、WSL、SSH 或其他用户。多智能体需要 Claude Code 2.1.32+，
属于实验功能并会增加 Token 用量。建议启用后新建会话。

## 下载

- `ClaudeCodeManager-1.2.0.exe`：独立可执行文件。
- `ClaudeCodeManager-1.2.0-windows-x64.zip`：包含 EXE、README、CHANGELOG、LICENSE。
