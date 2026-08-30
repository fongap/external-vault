# SecurePigeon（安全信鸽）项目审核报告：Windows 7+ 兼容性 & 安全

> 审核日期：2026-08-30。方法：仓库中只有 4 个 PyInstaller 打包产物（无源码），
> 全部源码通过 PyInstaller 归档提取 + CPython 3.12 字节码反汇编逆向重建
> （重建源码位于 `src/`，反汇编底稿位于 `_extracted/_dis/`）。

## 一、项目结构与功能

| 产物 | 入口脚本 | 功能 |
|---|---|---|
| `一键部署到U盘.exe` | `initialize_usb.py` | 作者端：清盘 → 生成作者/临时 RSA 密钥对 → 初始化 `.pigeon.store` 加密文件仓 → 写激活凭据/主密钥文件/TOTP 二维码 → icacls 防删除锁定 |
| `收件箱.exe` | `inbox.py` | 买家端：选择文件 → RSA-OAEP+AES-256-GCM 混合加密后追加写入 U 盘文件仓；含 7 天试用/激活码门控 |
| `取件箱.exe` | `retrieve.py` | 取件端：作者私钥或 6 位 TOTP 临时取件码，从文件仓解密导出全部文件 |
| `赋码工具.exe` | `activation_tool.py` | 作者端：HMAC-SHA256(master_key, device_id) 生成激活码，可记录 CSV |

核心库：`pigeon_store.py`（PIGEON02 仓格式：4KiB 头 + MSG3 追加记录；AESGCM 分块加密 + RSA-3072-OAEP-SHA256 双接收方封装；NTFS 稀疏文件）、`activation.py`（卷序列号设备指纹 + 试用/激活）、`temporary_access.py`（离线 TOTP 临时取件）。

UI 方案：四个程序均为 Python 命令行内核 + 临时 `.ps1`（UTF-8 BOM）调用 `powershell.exe -NoProfile -STA -ExecutionPolicy RemoteSigned -File` 弹出 WinForms 窗口，中文经 base64(UTF-16LE) 传输；进度条通过 `%TEMP%` 下 `.status` 文件 + Timer 轮询。

## 二、Windows 7 兼容性结论

### 2.1 致命阻断（打包层，必须换工具链重建）

| 组件 | 现状 | 问题 | Win7 兼容选择 |
|---|---|---|---|
| CPython | 3.12（`python312.dll`） | 3.9 起放弃 Win7，3.12 在 Win7 上无法启动 | **Python 3.8.10**（最后支持 Win7 的版本；建议 32 位以兼容 32 位系统） |
| PyInstaller | 6.x（onedir `_internal` 布局） | 6.x 引导程序要求 Win8+，头文件已移除 Win7 feature level | **PyInstaller 4.10** |
| cryptography | 50.0.0 | 新轮子不支持 Win7 | **42.0.7**（官方 changelog 明确恢复了 Win7 兼容，并声明下一版不再支持，故必须钉死） |
| Pillow | 12.3.0 | 不支持 py3.8 | **10.4.0** |
| qrcode | 8.2 | 要求 Python 3.9+ | **7.4.2** |

目标机要求：Win7 **SP1** + VC++ 2015-2022 运行库（`vcredist`）+ Universal CRT（KB2999226，通常已由 Windows Update 安装）。

### 2.2 源码级 Win7 不兼容点（唯一一处，三个程序各一份）

三个进度窗 PowerShell 脚本使用 `Get-Content -Raw`（**PowerShell 3.0+** 参数；Win7 出厂自带 PS 2.0）：

- `inbox.py` → `DeliveryProgress`（进度 + **取消按钮也会失效**，因为 CANCEL 判断在 `Get-Content` 之后）
- `retrieve.py` → `RetrievalProgress`
- `initialize_usb.py` → `DeploymentProgress`

**修复（已应用到 `src/retrieve.py`、`src/inbox.py`；`initialize_usb.py` 需对原源码做同样修改）：**

```powershell
# 原写法（PS 3.0+）：
(Get-Content -LiteralPath $p -Raw -Encoding UTF8).Split([char]9)
# PS 2.0 兼容写法（.NET，全版本可用；状态文件为无 BOM UTF-8，ReadAllText 默认即按 UTF-8 读）：
([IO.File]::ReadAllText($p)).Split([char]9)
```

另建议把 `$ratio=if(...){...}else{...}` 包成 `$(if ...)`（PS 2.0 下更稳妥），已同样应用。

### 2.3 逐项核查为 Win7 兼容的用法（无需修改）

`GetLogicalDrives`、`GetVolumeInformationW`、`DeviceIoControl(FSCTL_SET_SPARSE)`（NTFS 稀疏文件，XP+ 支持；FAT32/exFAT 上静默失败仅退化为占盘写零）、`msvcrt.locking`、`attrib +h`、`icacls`（Vista+，脚本用 SID 通配 `*S-1-1-0/*S-1-5-32-544/*S-1-5-18`）、`WScript.Shell` 建 `.lnk`、WinForms 全部控件与 `Microsoft YaHei UI` 字体（Vista+ 自带）、`powershell.exe -STA`（PS 2.0 支持，且 PS 2.0 默认 MTA，显式 -STA 是必要的）、`from __future__ import annotations` 使全部 PEP 604 注解在 3.8 下安全、海象运算符 3.8 支持、`shutil.copytree(dirs_exist_ok=)` 3.8 支持。**Python 源码除上述 `-Raw` 外在 3.8 下无需改动即可编译。**

## 三、安全审核发现（按严重程度）

### 高危

1. **激活机制可一分钟绕过**（`activation.py`）：`.pigeon.license` 只存激活码的 SHA-256 哈希，验证不涉及主密钥/卷序列号。任何人写入 `32 字节零 + SHA256(自选码) + \n + 自选码` 即永久激活。32 字节"混淆主密钥"在验证路径中未被使用；其混淆掩码 = SHA256(卷序列号小端)，卷序列号任何人都可读，XOR 即还原明文主密钥。
2. **授权文件可整盘克隆**：激活状态不重新绑定当前卷序列号，复制 `.pigeon.license` 即克隆激活。
3. **试用期可无限重置**：删除/改写明文 `.pigeon.trial`（ASCII 日期）即重置 7 天；回拨系统时钟亦可延长（`start_trial` 用 UTC 写、`days_remaining` 用本地时区解析，还有时区错位 bug）。
4. **临时取件码可离线穷举**（`temporary_access.py`）：seed 明文在文件头，第二因子仅 10^6 空间 6 位码、无 KDF 拉伸，拿到 U 盘者可用 AES-GCM 当校验 oracle 每秒数百万次离线穷举；`fail_count` 节流仅对走 `verify_and_consume` 的在线路径有效。文件头（`last_used`/`fail_count`）无完整性保护，可清零重放/解除节流；取件前备份文件、用后回滚即可无限重放同一枚码。
5. **U 盘上的公钥可被替换**（`inbox.py`）：`.pigeon.temp.public.pem`/`.pigeon.public.pem` 存于可写 U 盘，投递前替换公钥即可静默重定向后续投递内容。公钥应内嵌进 exe 或经签名校验。
6. **激活主密钥明文落盘**（部署端）：「激活主密钥-设备ID.txt」为 64 位 HEX 明文，拿到即能为任意设备签发激活码；赋码工具 CLI 的 `--master-key` 还会出现在进程命令行中。

### 中危

7. 文件仓**记录头未认证**（`pigeon_store.py`）：`total/长度/plain_size/nonce_prefix` 明文且无 MAC，第二遍导出直接信任这些长度字段 → 恶意/损坏仓可触发超大内存分配（DoS）；且无记录签名，任何人可向仓追加"合法格式"记录（垃圾投递 DoS，作者无法验证来源）。
8. `append_file` 无文件锁（TOCTOU）：多进程同时投递会互相覆盖记录头损坏仓；崩溃一致性依赖"密文首 4 字节恰好不是 MSG3"的概率性事实。
9. PowerShell 临时脚本 TOCTOU：`%TEMP%\pigeon-ps-*.ps1` 写入到 `-File` 执行之间可被同用户恶意进程替换；`-ExecutionPolicy RemoteSigned` + 本地文件实质等同 Bypass。进度 `.status` 文件是无鉴权控制通道（任何本地进程可写 CANCEL 打断投递）。
10. 取件端"是否 U 盘"仅凭根目录存在 `.pigeon.store` 标记文件判断（`GetLogicalDrives` 枚举后不看驱动器类型）——映射网络盘/C 盘上的标记文件也会被接受，建议加 `GetDriveTypeW == DRIVE_REMOVABLE` 校验。收件端则相反：`USB_ROOT = exe目录.parent.parent`，exe 放错层级会解析到错误的"根"。
11. PowerShell 缺失时静默失败：`retrieve.py` 中 `ps()` 失败返回 stderr 提示，但调用方只扫 stdout，最终静默退出 0，用户无任何提示。

### 低危

12. `pigeon_store.private_key_pem()` 提供 NoEncryption 明文私钥导出（`temporary_access.provision` 依赖它，属必要，但任何误用都会落盘裸私钥）；临时取件私钥 PEM 解密后驻留内存直到 GC（`del` 不是擦除）。
13. 取件时 `glob(".pigeon-tmp-*") + unlink()` 会删除目标目录中同模式的无辜文件；docstring 声称的"临时名冲突回退到带序号安全名"实际未实现。
14. CSV 记录未做 Excel 公式注入中和（订单号/备注列以 `=`/`+`/`-`/`@` 开头时）。
15. 临时取件窗口含未来片 (+1)，下一片码可提前使用；`msvcrt.locking` 解锁区间与加锁区间错位、必然静默失败（互斥靠 close 兜底，行为正确但属巧合）。
16. UI 文档宣称"防删除保护"，但 icacls deny 仅防删除，不防内容篡改/复制——与上述 1/4/5 组合后，U 盘的整体安全模型是"混淆"而非"访问控制"，明文密钥文件的安全完全依赖作者的保管。

## 四、Win7 兼容构建方案（已就绪的交付物）

- `src/` — 逆向重建的 7 个源文件（`pigeon_store.py`、`activation.py`、`temporary_access.py`、`inbox.py`、`retrieve.py`、`activation_tool.py` 已重建并编译验证；`initialize_usb.py` 主体流程已梳理（见本报告第一节），完整源码建议使用你的原版并仅应用 2.2 节的 `-Raw` 修复）。
- `src/requirements-win7.txt` — 依赖钉版（Python 3.8.10 + PyInstaller 4.10 + cryptography 42.0.7 + Pillow 10.4.0 + qrcode 7.4.2）。
- `src/build_win7.bat` — 构建脚本（注意：PyInstaller 4.10 的 onedir 布局没有 `_internal` 子目录；用 `--collect-all cryptography` 保证 42.x 的 `_rust` 扩展与 DLL 被正确收集）。

构建步骤：安装 Python 3.8.10（32 位）→ `py -3.8 -m pip install -r requirements-win7.txt` → 在 `src/` 运行 `build_win7.bat` → 将 dist 产物按原部署结构放回 U 盘。

**注意**：`cryptography 42.0.7` 的 Win7 兼容是官方声明但官方不测试的；`Pillow 10.4.0` 在 Win7 上的轮子也建议在真实 Win7 SP1 机器上各跑一次自检（部署端有 `--self-test` 参数可验证 qrcode/PIL）。

## 五、版本事实来源

- [cryptography changelog](https://cryptography.io/en/latest/changelog/)：42.0.7 "Restored Windows 7 compatibility for our pre-built wheels"，并声明下一版本不再支持 Win7。
- [PyInstaller 4.2 requirements](https://pyinstaller.org/en/v4.2/requirements.html)（"Windows 8 or newer (Windows 7 should work too, but is not supported)"）与 [PyInstaller 6.x changelog](https://pyinstaller.org/en/stable/CHANGES.html)（移除 Win7 feature level）。
- [Python 3.11 for Win7 讨论](https://discuss.python.org/t/why-is-there-no-python3-11-for-windows7/24443)：3.9 起放弃 Win7。

## 六、修复记录

### 修复 #1：激活机制可绕过 + 主密钥混淆形同虚设（已完成，2026-08-30）

**改动文件**：`src/activation.py`（重写核心验证）、`src/test_activation_fix.py`（新增，9 项测试全部通过）。

新 license 格式 **PLF3**（`.pigeon.license`，116 字节 + 可选缓存码）：

```
magic "PLF3"(4) | scrypt salt(16) | master_key XOR scrypt(serial,salt)(32) | tag(64 hex ascii)
tag = HMAC-SHA256(master_key, b"pigeon-activation-v3" || serial_le32 || normalized_code)
```

修复点：
1. **标签密钥化**：原格式存激活码的无密钥 SHA-256（攻击者写"任意码+其哈希"即激活）；现改为以 master_key 为密钥、绑定卷序列号和激活码的 HMAC——伪造必须先还原 master_key。
2. **混淆拉伸**：原掩码 = 裸 `SHA256(卷序列号)`（任何人都可算）；现改为 `scrypt(serial, salt)`（N=16384, r=8，本机 78ms/次，Win7 老机器预计 100–500ms），且 salt 每次部署随机，无法跨盘预计算。
3. **绑定卷序列号**：验证时用*当前*卷序列号还原 master_key 并重算 tag——克隆 license 到另一块盘 / 重格 U 盘后激活立即失效。
4. **时区 bug**：`start_trial` 写 UTC 日期、`days_remaining` 用本地时区解析（`time.mktime`），现统一 UTC（`calendar.timegm`）；`start_trial` 改用 `w+b` 避免 `.pigeon.trial` 缺失时抛未捕获 `FileNotFoundError`。
5. **旧格式兼容已关闭**：测试证明"旧格式兼容读取"会重新打开旁路（旧格式文件可原样复制到新盘），因此旧格式验证默认**拒绝**，仅有显式常量 `ACCEPT_LEGACY_LICENSES = False` 可开启（开启即接受旁路风险，不建议）。**已部署的旧 U 盘需要重新部署才能激活。**

**需要同步修改的调用点（仅 1 行，部署端 `initialize_usb.py` 原源码第 549 行附近）**：

```python
# 原：
deploy_license(root, master_key, code_hash(activation_code))
# 改为（传激活码本身，不再是其哈希）：
deploy_license(root, master_key, activation_code)
```

> **2026-08-30 更新：原始 `initialize_usb.py`（745 行）已找到并放入 `src/`，
> 上述补丁已应用完毕**（连同修复 #3 的公钥指纹、修复 #4 的 Win7 `-Raw` 修复），
> 并通过端到端冒烟测试：部署流程产出的 PLF3 license（180 字节）指纹与盘上
> 公钥一致、激活闭环通过。`src/` 现已包含全部 7 个源文件，`build_win7.bat`
> 的 4 个构建目标均可直接执行。`code_hash` 导入保留（赋码工具 GUI 导入
> 密钥文件的正则仍引用说明文档格式，无影响）。

赋码工具（`activation_tool.py`）与 `generate_activation_code` 算法**不变**，激活码本身格式不变；收件箱（`inbox.py`）调用的所有函数签名不变，无需改动。

**测试覆盖**（`src/test_activation_fix.py`）：部署→验证→激活闭环、错误码拒绝、旧式伪造拒绝、跨盘克隆拒绝（原盘仍有效）、重格后失效、裸 SHA256 掩码不再能还原主密钥、旧格式默认拒绝/显式开启后可用、试用日期单一时间基准、tag 篡改检测。

### 修复 #2：临时取件码（temporary_access v3，已完成，2026-08-30）

**改动文件**：`src/temporary_access.py`（重新设计）、`src/test_temporary_access_fix.py`（新增，10 项测试通过）。函数签名不变，`retrieve.py`/部署端无需改动。

核对原始反汇编时确认了原程序的**两个隐藏缺陷**，本次一并修复：

1. **输入的码从未参与验证**——原 `verify_and_consume` 只用窗口候选时间片的码试解密，用户输入的 6 位数除格式检查外完全未被使用（任何 6 位数字都能过）。v3 强制输入码与窗口内候选码常数时间匹配。
2. **凭据绑定部署时刻的时间片**——原实现用 `current_counter()+1` 的码加密私钥，窗口只有 ±3 片：部署约 90 秒后临时取件即永久失效。v3 解密密钥只由 seed 派生（与码无关），授权可发生在任意时刻。

其余加固：
3. **seed 掩码存储**：seed 不再明文，改为 `XOR scrypt(卷序列号, salt)`（salt 每次部署随机）——还原 seed 需理解格式 + scrypt，与 PLF3 同哲学。
4. **AAD 绑定**：magic/version/还原后 seed/nonce 进入 GCM AAD，篡改头部即解密失败；fail_count/last_used 保持可原地更新。
5. scrypt（N=32768）拉伸密钥派生；防重放单调闸门与失败节流保留；修正 `msvcrt` 解锁区间错位；版本号 3，v1/v2 凭据明确拒绝并提示"请重新部署 U 盘"。

物理边界（诚实声明，已写入模块文档）：验证秘密与被保护数据在同一块可写 U 盘上时，离线方案对理解格式的持盘者只能提高成本不能根除。残余风险：备份旧凭据文件 + 录得已用码可回滚重放；计数器字段可被清零。

### 修复 #3：公钥替换检测（已完成，2026-08-30）

**改动文件**：`src/activation.py`、`src/inbox.py`。

PLF3 license 扩展为 180 字节：追加两把公钥的指纹（SHA-256 of SPKI DER，各 32 字节）。收件箱启动加载公钥后调用 `verify_public_keys()` 比对——盘上 `.pigeon.public.pem` / `.pigeon.temp.public.pem` 被替换（投递内容静默重定向到攻击者）时立即拒绝投递并告警。替换公钥现在还需伪造 license（即还原 master_key）。无 license / 旧格式 license 时校验跳过（no-op）。

### 修复 #4：文件仓健壮性 + 文件锁 + U 盘类型校验 + PS 缺失提示（已完成，2026-08-30）

**改动文件**：`src/pigeon_store.py`、`src/retrieve.py`、`src/inbox.py`、`src/test_store_and_pinning_fix.py`（新增，9 项测试通过）。

1. **记录头认证**：元数据 GCM 的 AAD 从固定常量改为绑定整个记录头（含各长度字段）——篡改记录头任何字段都会使元数据解密失败，导出时计为损坏记录。读取端先试新 AAD、失败回落旧 AAD，**v2 原始程序写入的旧仓完全兼容可读**。
2. **长度自洽校验**：新增 `_valid_record_header`（marker、长度上限、`total == 各字段严格之和`），两遍扫描都启用——伪造 `plain_size=4GB` 的恶意仓在 23ms 内被拒，不再可能触发超大内存分配。
3. **append_file 跨进程锁**：以文件最后一字节为互斥锁（Windows 强制区锁；刻意不用字节 0 以免阻塞读头部的并发操作），并发投递被明确拒绝（"文件仓正被其他程序使用"）而非互相覆盖写坏仓库。
4. **U 盘类型校验**（`retrieve.py`）：`GetDriveTypeW == DRIVE_REMOVABLE` 才进入盘符列表/被 `resolve_store` 接受——固定硬盘、网络映射盘、光驱上的 `.pigeon.store` 标记文件不再被当作 U 盘。
5. **PowerShell 缺失提示**（两个 exe 的 `main`）：`shutil.which` 预检 + 原生 `MessageBoxW` 报错，替代原实现"stdout 无输出 → 静默退出 0"的行为。

**全部回归**：三个测试套件 28/28 通过，六个源文件编译通过。
