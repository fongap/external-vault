# Fong's AdFilter

一款高效、全面的广告过滤规则列表，专为提升网络浏览体验而设计。本项目自动聚合多个高质量过滤源，提供最新、精简的广告拦截规则。

> 本项目为自动化聚合工具，不包含人工编写的规则。聚合逻辑与构建脚本为原创代码，采用 MIT 许可；规则数据版权归各上游项目所有。

## 主要特点

- **全面覆盖**：整合多个主流过滤器的精华规则
- **自动更新**：每日 UTC 时间 16:00 自动构建最新规则
- **智能去重**：自动合并多源规则，消除重复项
- **标准格式**：兼容 Adblock Plus 2.0 规范

## 数据来源

本项目聚合以下上游过滤列表（按字母序）：

| 上游项目 | 说明 | 许可 |
|----------|------|------|
| [AdGuard Base Filter](https://github.com/AdguardTeam/AdguardFilters) | 基础广告拦截规则 | MIT |
| [AdGuard Chinese Filter](https://github.com/AdguardTeam/AdguardFilters/tree/master/ChineseFilter) | 中文区特化规则 | MIT |
| [cjxlist](https://github.com/cjx82630/cjxlist) | 中文广告/隐私/烦人元素规则 | MIT |
| [EasyList](https://github.com/easylist/easylist) | 国际通用广告拦截规则 | CC BY 3.0 |
| [EasyList Cookie](https://github.com/easylist/easylistcookie) | Cookie 相关规则 | CC BY 3.0 |
| [Fanboy Annoyance](https://github.com/ryanbr/fanboy-adblock) | 烦人元素/社交按钮规则 | CC BY 3.0 |
| [uBlock Origin Annoyances](https://github.com/uBlockOrigin/uAssets/tree/master/filters/annoyances) | 烦人元素规则 | Various |
| [uBlock Origin Cookie](https://github.com/uBlockOrigin/uAssets/tree/master/filters/cookie) | Cookie 相关规则 | Various |

完整来源列表见构建脚本配置。

## 使用方法

在 uBlock Origin、AdGuard、Adblock Plus 等兼容 Adblock Plus 2.0 规范的工具中添加以下自定义过滤列表：

```text
https://ex.135468.xyz/adblock/adblocklist.txt
```

也可直接下载 `adfilter.txt` 导入本地使用。

## 构建与更新

- **构建工具**：Python 3.10+，内部维护的 `build.py`（不随发布分发）
- **更新频率**：每日 UTC 16:00 自动运行 GitHub Actions 工作流
- **输出产物**：`adfilter.txt`（去重、排序、校验后的合并规则）

本地构建（仅供维护者参考）：

```bash
python build.py --output adfilter.txt
```

## 规则统计

当前版本（随每次构建更新）：

- 规则总数：144,667 条
- 网络过滤规则：88,063
- 元素隐藏规则：46,867
- 例外规则：9,737

## 许可说明

**本仓库原创代码与构建脚本**：[MIT License](../LICENSE)

**聚合的过滤规则数据**：各上游项目保留各自的版权与许可。**本项目的 MIT 许可不覆盖、不重新许可任何第三方规则数据。**

详细第三方声明见仓库根目录 [THIRD_PARTY_NOTICES.md](../THIRD_PARTY_NOTICES.md)。

## 贡献指南

本项目为自动化聚合项目，不接受直接规则提交。如发现误拦截或漏拦截，请向对应上游项目反馈。

- 发现构建失败：提交 Issue
- 上游源变更：提交 Issue 或 PR 更新来源配置
- 文档改进：欢迎 PR

## 相关链接

- 在线订阅：`https://ex.135468.xyz/adblock/adblocklist.txt`
- 问题反馈：[GitHub Issues](https://github.com/Fongap/external-vault/issues)
- 上游项目汇总：见上文数据来源表格