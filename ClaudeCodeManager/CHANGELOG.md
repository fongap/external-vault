# Changelog

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
