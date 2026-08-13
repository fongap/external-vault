# Changelog

## 1.2.1 - 2026-08-13

### UI 与术语

- 中文界面、提示和说明统一使用“多智能体”；首次说明写作“多智能体（Agent Teams）”，
  底层 `CLAUDE_CODE_EXPERIMENTAL_AGENT_TEAMS` 环境变量保持不变。
- 右侧操作区固定为三行两列：检查更新/桌面快捷、导入配置/打开配置、模型配置/多智能体。
- 用户级 `CLAUDE.md` 托管规则同步采用“多智能体”“主智能体”“单智能体”等统一称谓。

## 1.2.0 - 2026-08-13

### 多智能体

- 主窗口新增当前 Windows 用户级 `多智能体` 开关，同时管理
  `~/.claude/settings.json` 实验环境变量和 `~/.claude/CLAUDE.md` 多智能体规则。
- settings 更新只定向修改 `CLAUDE_CODE_EXPERIMENTAL_AGENT_TEAMS`，保留其他顶层及
  `env` 字段；CLAUDE.md 只增删唯一标记托管区块。
- 首次启用会记录目标变量的原始 JSON 值；停用时仅在当前仍为 Manager 写入的 `"1"`
  时恢复。检测到外部改写会保留用户值并警告。
- 两个配置文件写前保留原始备份，使用 PID/时钟唯一临时文件与原子替换；第二阶段
  失败时自动补偿回滚第一阶段。
- 非法 JSON、重复目标键、重复/缺失/倒序托管标记、无效 UTF-8 以及超大配置全部
  fail closed，不覆盖用户文件。
- 成功写入后配置统一规范化为无 BOM UTF-8；JSON 格式可能重排，但未知字段和
  CLAUDE.md 托管标记外的文本内容会保留。

## 1.1.2 - 2026-08-13

### Gateway compatibility

- 修复 V1.1 探活顺序回归：恢复 V1.0 先读取 `/v1/models` 的行为，再使用返回的
  真实模型 ID 探测 `/v1/messages`，避免缺失 `model` 时的 401/403 阻断模型发现。
- 恢复模型列表 GET 的 V1.0 三认证头兼容性；Messages POST 先发送组合头，若收到
  401/403，再依次回退 Bearer、`x-api-key` 和 `x-gateway-access-key` 单头认证。
- 探活 `/v1/messages` 使用合法最小请求体（真实 `model` + `max_tokens=1` + 单字符
  prompt）；没有手填或发现模型时才回退 `"default"`。
- 2xx、400 和 422 均确认 Messages 路由存在；404/405 标记为协议不兼容，其他响应
  明确显示为“模型列表可用但 Messages 兼容性未确认”，不再把预检失败误报成密钥失效。
- 模型保存前拒绝 URL 含 `@`（userinfo），避免凭据随 settings.json 写入或被网关
  访问日志记录。

### Build

- 收敛版本号、超时、provider 索引与代理提示等到 `CCM_*` 常量块，单点修改即可
  影响窗口标题、快捷方式描述、关于对话框与启动横幅；版本横幅不再携带空的
  `-r0` 后缀。

### Credential isolation

- “仅配置模型”模式现在强制清空 `ActiveCredentialNames`，阻止先前网关的 bearer
  凭据泄漏到使用官方 Claude 的会话。
- 启动器加载凭据时若注册名在 Windows 凭据管理器中已不存在，输出 `[WARN]`
  提示用户重新保存。

### Input handling

- 探活输入触发去抖：URL/密钥/下拉框焦点变化统一合并到一个 400 ms Win32 计时器，
  避免每次按键或焦点切换就发起一次 HTTP 请求。
- 关闭模型配置向导时取消未触发的探活计时器，避免关闭后仍发起网络请求。

### Startup

- 移除首次启动时静默创建桌面快捷方式（现改为显式点击主窗口按钮），不再因
  未签名 exe 触发 SmartScreen 黄色提示。
- `run_unified_console_launcher` 优先 `AttachConsole(ATTACH_PARENT_PROCESS)`，
  仅在没有父终端时才分配新控制台；`FreeConsole` 只释放自己分配的终端。
- 启动 claude.exe 时新增 `CREATE_NEW_PROCESS_GROUP`，Ctrl-C 在终端内的信号
  现在路由到子进程而非 launcher。
- `update_thread` 在未检测到 claude.exe 切换到安装流程前清理代理环境变量，
  避免跨流程的代理状态泄漏。
- 显式抑制 `WM_ERASEBKGND` 并响应 `WM_PRINTCLIENT`，减少 owner-draw 控件
  重绘时的窗口闪烁。

## 1.0.1 - 2026-08-09

### Gateway compatibility

- 修复模型列表读取成功但 Claude Code 运行时报“模型不存在或无访问权限”的假成功。
- “测试并获取模型”现在先验证 Anthropic `/v1/messages` 路由，再读取 `/v1/models`。
- 404/405 会被识别为仅支持 OpenAI Chat Completions 的不兼容接口，并阻止误保存。
- 明确 NVIDIA Integrate 免费托管地址不能直接作为 Claude Code 的网关；支持
  Anthropic Messages 的自托管 NIM 或兼容网关仍可使用。

### Credential isolation

- 启动器只注入当前配置激活的凭据，不再加载 Windows 凭据库中的全部历史密钥。
- 官方账号模式无 API Key 时停用旧网关凭据，让原生 Claude Code 使用 `/login`。
- 停止重复保存无 Claude Code 作用的 `GATEWAY_ACCESS_KEY` 环境变量。
- 清理遗留的 `CLAUDE_CODE_MAX_CONTEXT_TOKENS` 和 `contextWindow` 非公开字段。

## 1.0.0 - 2026-08-09

### Architecture

- 收敛为“原生 Claude Code 执行、Manager 薄封装”。
- 保留同一 EXE 的 `--claude-launcher` 入口，直接启动原生 `claude.exe`。
- 安装与升级分别调用 Claude 官方安装入口和 `claude update`。
- 不修改 `claude.exe` 资源，不新增会话引擎或请求代理层。

### Model planning

- 为主力、Opus、Sonnet、Haiku、Subagent 增加一致的本地上下文规划字段。
- 自动识别网关返回的常见上下文容量字段，并允许手动标注未知容量。
- 容量、预算、压缩预警与输出预留只作为本地元数据，不写入 Claude Code
  不支持的配置项。

### Reliability and security

- 保留用户 PATH 的 `REG_SZ`/`REG_EXPAND_SZ` 类型，避免异常覆盖。
- 收紧 JSON、HTTP/HTTPS URL、凭据写入和可执行文件路径校验。
- 修复网络测试、配置导入重复触发以及 UTF-8 分块输出边界问题。
- 线程启动失败时恢复 UI 状态并清理敏感临时缓冲区。

### Interface

- 统一主窗口与模型配置窗口的字体、间距、控件高度和视觉层级。
- 支持窗口缩放、最大化、尺寸记忆与 Per-Monitor V2 DPI。
- 修正项目路径、浏览/刷新、网络检测、模型表格和按钮组的对齐问题。
- 模型表格改为无逐格描边的整行轻底色，并校正输入文字垂直基线。
- 保留包含 16 至 256 px 图层的自定义橙色图标。
