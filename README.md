# external-vault

`external-vault` is a small monorepo for independently maintained utilities and
generated resources from Fongap. Each project keeps its own documentation and
build instructions while sharing one repository-level license policy.

## Projects

| Project | Description |
| --- | --- |
| [ClaudeCodeManager](ClaudeCodeManager/) | A lightweight Windows launcher and configuration manager that delegates sessions and model execution to the native Claude Code CLI. |
| [Fong's AdFilter](#fongs-adfilter) | An automatically generated, deduplicated advertising and annoyance filter list. |

## Claude Code Manager

Claude Code Manager handles project selection, supported `settings.json` values,
Windows credentials, proxy environment variables, a consistent launcher icon and
native install/update entry points. Conversation state, model calls, context
compression, authentication, MCP and runtime recovery remain the responsibility
of the native `claude.exe`.

See [ClaudeCodeManager/README.md](ClaudeCodeManager/README.md) for architecture,
security boundaries and build instructions.

## Fong's AdFilter

一款高效、全面的广告过滤规则列表，专为提升网络浏览体验而设计。本项目自动聚合多个高质量过滤源，提供最新、精简的广告拦截规则。

### 主要特点

- **全面覆盖**：整合多个主流过滤器的精华规则
- **自动更新**：每日 UTC 时间 16:00 自动构建最新规则
- **智能去重**：自动合并多源规则，消除重复项
- **标准格式**：兼容 Adblock Plus 2.0 规范

### 数据来源

- AdGuard 基础过滤器与中文特别版
- cjxlist、Fanboy 与 uBlock Origin 烦人元素规则
- EasyList Cookie 与 uBlock Origin Cookie 规则

### 使用方法

在 uBlock Origin、AdGuard 等工具中添加以下自定义过滤列表：

```text
https://ex.135468.xyz/adblock/adblocklist.txt
```

聚合规则的著作权与许可仍归各上游项目；详情见
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)。

## License

本仓库原创代码与文档采用 [MIT License](LICENSE)。第三方聚合数据不因存放于
本仓库而被重新许可。
