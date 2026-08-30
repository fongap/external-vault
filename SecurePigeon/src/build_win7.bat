@echo off
REM ============================================================
REM  Secure Pigeon — Windows 7 兼容构建脚本
REM
REM  前置条件：
REM    1. 安装 Python 3.8.10（建议 32 位版本，可在 32/64 位 Win7 上都运行；
REM       下载: https://www.python.org/downloads/release/python-3810/ ）
REM    2. 在本目录执行:
REM          py -3.8 -m pip install -r requirements-win7.txt
REM    3. 将原始源码（含 initialize_usb.py）放入本目录，或使用本目录中
REM       从 exe 逆向重建的源码（initialize_usb.py 需自行补齐/使用原源码，
REM       并按下述说明修改 Get-Content -Raw 一处）。
REM
REM  源码级修复（已应用到本目录 retrieve.py / inbox.py / activation.py /
REM  pigeon_store.py / temporary_access.py；initialize_usb.py 需在你的原源码上做）：
REM    1. Win7（PS 2.0）：进度窗 PowerShell 中的
REM          (Get-Content -LiteralPath $p -Raw -Encoding UTF8).Split([char]9)
REM       改为  ([IO.File]::ReadAllText($p)).Split([char]9)
REM       （Get-Content -Raw 需要 PowerShell 3.0+）
REM    2. 安全修复 #1（激活）：deploy_license 调用点改为传激活码与公钥指纹：
REM          deploy_license(root, master_key, activation_code,
REM                         fingerprint(author_public_key),
REM                         fingerprint(temporary_public_key))
REM       并在 pigeon_store 的 import 中加入 fingerprint。
REM       （原: deploy_license(root, master_key, code_hash(activation_code))）
REM    3. temporary_access.provision / save_qr 签名不变，无需改动。
REM ============================================================

setlocal
set PY=py -3.8

REM 常量：共享参数。--collect-all cryptography 用于让旧版 PyInstaller
REM 正确收集 cryptography 42.x 的 _rust 扩展与 DLL。
set COMMON=--noconfirm --clean --collect-all cryptography

REM 1) 收件箱（inbox）
%PY% -m PyInstaller %COMMON% --name inbox --hidden-import inbox inbox.py
REM 2) 取件箱（retrieve）
%PY% -m PyInstaller %COMMON% --name retrieve --hidden-import retrieve retrieve.py
REM 3) 赋码工具（activation_tool）
%PY% -m PyInstaller %COMMON% --name activation_tool --hidden-import activation_tool activation_tool.py
REM 4) 一键部署到U盘（initialize_usb，需要原始源码 + qrcode/PIL）
%PY% -m PyInstaller %COMMON% --name initialize_usb --hidden-import initialize_usb initialize_usb.py

echo.
echo 构建完成。产物在 dist\ 下，各为 onedir 目录。
echo PyInstaller 4.10 的 onedir 布局是 exe 与依赖同目录（无 _internal 子目录），
echo 与当前 PyInstaller 6 生成的 _internal 布局不同，属正常现象。
echo.
echo 发布到 U 盘时请保持 exe 与其同目录依赖整体拷贝。
endlocal
