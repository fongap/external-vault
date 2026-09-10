# OCI Capacity Watch

通过 Cloudflare Cron 定时触发 GitHub `repository_dispatch`，由 GitHub Actions 查询 Oracle Cloud Compute Capacity Report；当指定 `VM.Standard.A1.Flex` 配置出现容量时创建 GitHub Issue 提醒。

Cloudflare 只负责触发。OCI 检测、凭据、判断和通知全部留在 GitHub。

只做容量查询，不创建、修改或删除 OCI 实例。

## 架构

```text
Cloudflare Cron
      ↓
trigger-worker
      ↓ repository_dispatch: oci-capacity-watch
GitHub Actions
      ↓
OCI Capacity Report
      ↓
AVAILABLE → GitHub Issue
```

## 目录

```text
projects/oci-capacity-watch/
├─ README.md
├─ check-capacity.sh
└─ trigger-worker/
   ├─ wrangler.toml
   └─ src/index.js
```

GitHub Workflow：`.github/workflows/oci-capacity-watch.yml`

## Cloudflare Trigger

`trigger-worker` 只向 GitHub 发送 `repository_dispatch`，不接触 OCI。

Cloudflare Secret：

| 名称 | 用途 |
| --- | --- |
| `GITHUB_TOKEN` | 仅用于向 `fongap/external-vault` 创建 `repository_dispatch` |

Cloudflare Variables：

| 名称 | 用途 |
| --- | --- |
| `GITHUB_REPOSITORY` | 目标仓库，默认 `fongap/external-vault` |
| `GITHUB_EVENT_TYPE` | 事件类型，默认 `oci-capacity-watch` |

细粒度 GitHub Token 仅授权 `fongap/external-vault`，Repository permissions 设置 `Contents: Read and write`。

Cron 当前为：

```text
17 * * * *
```

即 UTC 每小时第 17 分钟触发一次。

部署：

```bash
cd projects/oci-capacity-watch/trigger-worker
npx wrangler secret put GITHUB_TOKEN
npx wrangler deploy
```

## GitHub Actions

Workflow 仅接受：

- `repository_dispatch`：`oci-capacity-watch`
- `workflow_dispatch`：手动测试

GitHub 本身不再配置 `schedule`。

检测到 `AVAILABLE` 时创建提醒 Issue；同一提醒 Issue 仍为 open 时不重复创建。未检测到容量时正常结束，不创建 Issue。

## OCI 环境变量

认证直接使用 OCI CLI 原生环境变量，不生成 `~/.oci/config`。

GitHub Secrets：

| 名称 | 用途 |
| --- | --- |
| `OCI_CLI_USER` | API 用户 OCID |
| `OCI_CLI_TENANCY` | Tenancy OCID，同时作为 Capacity Report 的 root compartment |
| `OCI_CLI_FINGERPRINT` | API signing key fingerprint |
| `OCI_CLI_KEY_CONTENT` | PEM 私钥完整内容 |

GitHub Variables：

| 名称 | 必填 | 用途 |
| --- | --- | --- |
| `OCI_CLI_REGION` | 是 | Region，例如 `us-phoenix-1` |
| `OCI_SHAPE` | 是 | 目标 Shape，例如 `VM.Standard.A1.Flex` |
| `OCI_OCPUS` | 是 | 目标实例 OCPU 数 |
| `OCI_MEMORY_GB` | 是 | 目标实例内存 GB |
| `OCI_AVAILABILITY_DOMAINS` | 否 | 逗号分隔的 AD；留空则自动枚举全部 AD |
| `OCI_CLI_VERSION` | 否 | OCI CLI 版本；未设置时使用 Workflow 中的固定默认版本 |

示例：

```text
OCI_CLI_REGION=us-phoenix-1
OCI_SHAPE=VM.Standard.A1.Flex
OCI_OCPUS=4
OCI_MEMORY_GB=24
OCI_AVAILABILITY_DOMAINS=
```

## OCI 权限

检测身份至少需要：

```text
COMPARTMENT_INSPECT
COMPUTE_CAPACITY_REPORT_CREATE
```

可按 OCI IAM policy 将权限限制到容量报告与必要的 compartment inspect 权限，不需要授予实例创建、修改或删除权限。

## 本地执行

安装 `oci` 与 `jq` 后，注入同名环境变量：

```bash
bash projects/oci-capacity-watch/check-capacity.sh
```

如果设置 `OCI_AVAILABILITY_DOMAINS`，脚本仅检测指定 AD；否则通过 OCI IAM 自动取得当前租户的 AD 列表。

## 结果语义

`AVAILABLE` 仅表示 Capacity Report 查询时该配置存在可用 host capacity，不构成容量预留。真正创建实例时仍可能因容量变化而失败。
