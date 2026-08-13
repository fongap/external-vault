# ClaudeCodeManager 1.1.2

本版本修复 V1.1 在第三方或自建网关上的模型探活回归。

## 主要修复

- 恢复 V1.0 的探测顺序：先读取 `/v1/models`，再用真实模型 ID 验证 `/v1/messages`。
- 恢复模型列表请求对 Bearer、`x-api-key`、`x-gateway-access-key` 三种认证头的兼容。
- Messages 探活在组合认证头返回 401/403 后，自动回退到逐个认证头重试。
- 使用包含真实 `model`、`max_tokens=1` 和最小消息的合法请求体。
- 将 2xx、400、422 识别为 Messages 路由存在，并区分“模型列表可用”和“Messages 已确认兼容”。
- 修正 Windows 文件版本资源，使 EXE 属性正确显示 1.1.2。

## 验证

- Windows x64 Release 构建通过。
- 模型解析、URL 和探活 JSON 单元测试通过。
- 本地伪网关端到端测试通过，覆盖真实模型发现、三认证头 GET 和单认证头 POST 回退。

## 下载

- `ClaudeCodeManager-1.1.2.exe`：独立可执行文件。
- `ClaudeCodeManager-1.1.2-windows-x64.zip`：包含 EXE、README、CHANGELOG、LICENSE。
- `SHA256SUMS.txt`：下载文件的 SHA-256 校验值。
