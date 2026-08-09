# Claude Code Manager

Claude Code Manager 是一个面向 Windows 的 Claude Code 轻量启动器与配置工具。
它坚持“原生 Claude Code 执行、Manager 薄封装”：Manager 只负责桌面入口和
Windows 集成，不复制 Claude Code 的会话或模型运行时。

> 本项目是独立工具，不隶属于 Anthropic，也不包含或修改 `claude.exe`。

## 产品边界

Manager 负责：

- 项目目录选择与启动入口
- Claude Code 公开支持的 `settings.json` 配置
- Windows 凭据管理器中的敏感凭据
- `ANTHROPIC_BASE_URL`、模型名和代理环境变量注入
- 安装、`claude update`、必要日志与故障诊断入口
- AppUserModelID、控制台标题、窗口/任务栏/快捷方式图标

原生 Claude Code 负责：

- 会话状态、模型调用与路由
- 上下文压缩、认证、更新机制、MCP
- 运行时错误恢复以及 CLI 交互

第三方网关配置只注入 Claude Code 支持的环境变量、模型名和凭据。Manager
不增加请求代理层，也不接管网关的路由与预算。

## 模型与上下文

每个角色可配置模型 ID、上下文容量标注、建议预算、压缩预警、输出预留和
思考强度。这里需要区分两层含义：

- 模型的真实上下文上限由模型或网关决定。
- 容量、预算、预警和输出预留是 Manager 本地保存的展示与软校验元数据。

Claude Code 没有公开的逐模型 `contextWindow` CLI 参数，因此 Manager 不会把
这些本地字段伪装成 `.claude/settings.json` 配置。网关模型列表若返回
`context_window`、`context_length`、`max_context_length` 或
`input_token_limit`，容量会自动填充；否则可手动标注。

## 安全与凭据

- Token、API Key、`GATEWAY_ACCESS_KEY` 等敏感值保存在 Windows 凭据管理器。
- 敏感值不会以明文保留在 `settings.json`。
- 启动原生 CLI 时仅向子进程注入必要的凭据与代理环境变量。
- Manager 不修改 Claude Code 可执行文件及其资源或签名。

本地规划元数据保存在：

```text
HKCU\Software\Fongap\ClaudeCodeManager
```

## 原生启动器与图标

轻量启动器入口为：

```text
ClaudeCodeManager.exe --claude-launcher <项目目录>
```

启动器设置 AppUserModelID、控制台标题和窗口图标，然后启动原生
`claude.exe`。内置 ICO 含 16、20、24、32、40、48、64、128、256 px 图层，
适配标题栏、任务栏、快捷方式与高 DPI。

## 构建

要求：Windows x64、PowerShell，以及 Zig 0.15.2 或兼容版本。

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\build.ps1 -ZigPath zig -Configuration release
```

输出文件：`dist\ClaudeCodeManager.exe`。

## 测试

主程序采用 C11 严格警告构建。上下文容量解析测试位于
`tests/context_capacity_parser_test.c`，覆盖数值与字符串形式的常见网关字段。

## UI 设计原则

- 主窗口与模型配置窗口可缩放、可最大化，并分别跟踪 Per-Monitor DPI。
- 使用统一的 Microsoft YaHei UI 字体层级、控件高度、卡片留白与按钮内边距。
- 页面只突出当前主要操作；次级操作保持克制。
- 模型表格使用整行轻底色，避免逐单元格边框造成视觉噪声。
- 保留原生控件的键盘、下拉和焦点行为，同时校正文案基线与高 DPI 裁切。

## License

MIT，参见仓库根目录 [LICENSE](../LICENSE)。

## 官方参考

- [Claude Code model configuration](https://code.claude.com/docs/en/model-config)
- [Claude Code settings](https://code.claude.com/docs/en/settings)
