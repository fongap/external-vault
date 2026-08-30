"""Provision a write-only encrypted USB mailbox (Windows / NTFS)."""

from __future__ import annotations

import argparse
import base64
import ctypes
import errno  # noqa: F401  (imported by the original, never referenced)
import os
import shutil
import subprocess
import sys
import tempfile
import time
import uuid
from pathlib import Path

from activation import (
    LICENSE_NAME,
    TRIAL_NAME,
    code_hash,
    deploy_license,
    generate_activation_code,
    generate_master_key,
    get_device_id,
)
from pigeon_store import (
    MailboxError,
    initialize_store,
    load_public_key,
    new_rsa_keypair,
    write_author_keypair,
)
from temporary_access import (
    ACCESS_NAME,
    PUBLIC_NAME,
    provision as provision_temporary_access,
    save_qr,
)

PACKAGED_INBOX_NAME = '收件箱'
INBOX_NAME = '.pigeon.inbox'
INBOX_EXE_NAME = '收件箱.exe'
INBOX_SHORTCUT_NAME = '收件箱.lnk'

PACKAGED_RETRIEVE_NAME = '取件箱'
RETRIEVE_NAME = '.pigeon.retrieve'
RETRIEVE_EXE_NAME = '取件箱.exe'
RETRIEVE_SHORTCUT_NAME = '取件箱.lnk'

PRIVATE_NAME = '密钥文件-请勿放入U盘.pem'
DEFAULT_KIT_NAME = '安全信鸽-密钥文件'
QR_NAME = 'temporary-access-totp-qr.png'

EVERYONE = '*S-1-1-0'
ADMINISTRATORS = '*S-1-5-32-544'
SYSTEM = '*S-1-5-18'

PS_UI_SCRIPT_NAME = 'pigeon_ui.ps1'


def _ps_product_script(script: str) -> str:
    """Write *script* to the deployer's own folder as a product ``.ps1``.

    This replaces the previous %TEMP% + random-name + unlink pattern.  Using
    ``-File`` instead of ``-EncodedCommand`` keeps the base64+utf-16le command
    line off the process argument list.  UTF-8 **with BOM** is required [...]
    """
    directory = Path(sys.argv[0]).resolve().parent
    directory.mkdir(parents=True, exist_ok=True)
    path = directory / PS_UI_SCRIPT_NAME
    with open(path, 'wb') as handle:
        handle.write(b'\xef\xbb\xbf')
        handle.write(script.encode('utf-8'))
    return str(path)


def _ps_value(script: str) -> str:
    path = _ps_product_script(script)
    command = [
        'powershell.exe',
        '-NoProfile',
        '-STA',
        '-ExecutionPolicy',
        'RemoteSigned',
        '-File',
        path,
    ]
    try:
        result = subprocess.run(
            command,
            capture_output=True,
            text=True,
            encoding='utf-8',
            errors='replace',
            creationflags=134217728,
            stdin=subprocess.DEVNULL,
        )
    except FileNotFoundError:
        return ''
    if result.returncode != 0:
        return ''
    line = result.stdout.strip()
    if not line:
        return ''
    try:
        return base64.b64decode(line).decode('utf-16le')
    except (ValueError, UnicodeDecodeError):
        return ''


def gui_input(title: str, prompt: str, default: str = '', secret: bool = False) -> str:
    t = base64.b64encode(title.encode('utf-16le')).decode('ascii')
    p = base64.b64encode(prompt.encode('utf-16le')).decode('ascii')
    d = base64.b64encode(default.encode('utf-16le')).decode('ascii')

    script = (
        "Add-Type -AssemblyName System.Windows.Forms;Add-Type -AssemblyName System.Drawing;$t=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
        f"{t}"
        "'));$p=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
        f"{p}"
        "'));$d=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
        f"{d}"
        "'));$wa=[System.Windows.Forms.Screen]::PrimaryScreen.WorkingArea;$DW=440;$DH=245;$FW=[Math]::Min($DW,[Math]::Max(320,$wa.Width-24));$FH=[Math]::Min($DH,[Math]::Max(240,$wa.Height-24));$f=New-Object System.Windows.Forms.Form;$f.Text=$t;$f.Size=New-Object Drawing.Size($FW,$FH);$f.StartPosition='CenterScreen';$f.BackColor=[Drawing.Color]::White;$f.FormBorderStyle='FixedDialog';$f.MaximizeBox=$false;$l=New-Object System.Windows.Forms.Label;$l.Text=$p;$l.Location=New-Object Drawing.Point(28,22);$l.Size=New-Object Drawing.Size(384,40);$l.Font=New-Object Drawing.Font('Microsoft YaHei UI',10);$l.ForeColor=[Drawing.ColorTranslator]::FromHtml('#6E6E73');$f.Controls.Add($l);$x=New-Object System.Windows.Forms.TextBox;$x.Text=$d;$x.Location=New-Object Drawing.Point(28,72);$x.Size=New-Object Drawing.Size(384,32);$x.Font=New-Object Drawing.Font('Microsoft YaHei UI',10);"
        + ('$x.UseSystemPasswordChar=$true;' if secret else '')
        + "$f.Controls.Add($x);$b=New-Object System.Windows.Forms.Button;$b.Text='继续';$b.Location=New-Object Drawing.Point(332,125);$b.Size=New-Object Drawing.Size(84,32);$b.FlatStyle='Flat';$b.BackColor=[Drawing.ColorTranslator]::FromHtml('#1677FF');$b.ForeColor=[Drawing.Color]::White;$b.Font=New-Object Drawing.Font('Microsoft YaHei UI',10);$b.FlatAppearance.BorderSize=0;$f.Controls.Add($b);$b.Add_Click({$f.Tag=[Convert]::ToBase64String([Text.Encoding]::Unicode.GetBytes($x.Text));$f.Close()});[void]$f.ShowDialog();if($f.Tag){Write-Output $f.Tag}"
    )
    return _ps_value(script)


def gui_folder(title: str, prompt: str, default: str) -> str:
    t = base64.b64encode(title.encode('utf-16le')).decode('ascii')
    p = base64.b64encode(prompt.encode('utf-16le')).decode('ascii')
    d = base64.b64encode(default.encode('utf-16le')).decode('ascii')

    script = (
        "Add-Type -AssemblyName System.Windows.Forms;Add-Type -AssemblyName System.Drawing;$t=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
        f"{t}"
        "'));$p=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
        f"{p}"
        "'));$d=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
        f"{d}"
        "'));$wa=[System.Windows.Forms.Screen]::PrimaryScreen.WorkingArea;$DW=460;$DH=265;$FW=[Math]::Min($DW,[Math]::Max(320,$wa.Width-24));$FH=[Math]::Min($DH,[Math]::Max(240,$wa.Height-24));$f=New-Object System.Windows.Forms.Form;$f.Text=$t;$f.Size=New-Object Drawing.Size($FW,$FH);$f.StartPosition='CenterScreen';$f.BackColor=[Drawing.Color]::White;$f.FormBorderStyle='FixedDialog';$f.MaximizeBox=$false;$l=New-Object System.Windows.Forms.Label;$l.Text=$p;$l.Location=New-Object Drawing.Point(28,22);$l.Size=New-Object Drawing.Size(404,40);$l.Font=New-Object Drawing.Font('Microsoft YaHei UI',10);$l.ForeColor=[Drawing.ColorTranslator]::FromHtml('#6E6E73');$f.Controls.Add($l);$x=New-Object System.Windows.Forms.TextBox;$x.Text=$d;$x.Location=New-Object Drawing.Point(28,72);$x.Size=New-Object Drawing.Size(320,32);$x.Font=New-Object Drawing.Font('Microsoft YaHei UI',10);$f.Controls.Add($x);$pick=New-Object System.Windows.Forms.Button;$pick.Text='浏览';$pick.Location=New-Object Drawing.Point(358,73);$pick.Size=New-Object Drawing.Size(74,30);$pick.FlatStyle='Flat';$pick.BackColor=[Drawing.Color]::White;$pick.ForeColor=[Drawing.ColorTranslator]::FromHtml('#1677FF');$pick.Font=New-Object Drawing.Font('Microsoft YaHei UI',9);$pick.FlatAppearance.BorderColor=[Drawing.ColorTranslator]::FromHtml('#1677FF');$pick.FlatAppearance.BorderSize=1;$f.Controls.Add($pick);$pick.Add_Click({$z=New-Object System.Windows.Forms.FolderBrowserDialog;$z.Description='选择密钥文件保存文件夹';$z.SelectedPath=$x.Text;if($z.ShowDialog() -eq [Windows.Forms.DialogResult]::OK){$x.Text=$z.SelectedPath}});$b=New-Object System.Windows.Forms.Button;$b.Text='继续';$b.Location=New-Object Drawing.Point(352,140);$b.Size=New-Object Drawing.Size(84,32);$b.FlatStyle='Flat';$b.BackColor=[Drawing.ColorTranslator]::FromHtml('#1677FF');$b.ForeColor=[Drawing.Color]::White;$b.Font=New-Object Drawing.Font('Microsoft YaHei UI',10);$b.FlatAppearance.BorderSize=0;$f.Controls.Add($b);$b.Add_Click({$f.Tag=[Convert]::ToBase64String([Text.Encoding]::Unicode.GetBytes($x.Text));$f.Close()});[void]$f.ShowDialog();if($f.Tag){Write-Output $f.Tag}"
    )
    return _ps_value(script)


def gui_drive(title: str) -> str:
    drives = []
    mask = ctypes.windll.kernel32.GetLogicalDrives()
    for index in range(26):
        if not mask & (1 << index):
            continue
        root = f'{chr(65 + index)}:\\'
        if root[0] not in frozenset({'C'}):
            drives.append(root)
    values = base64.b64encode('|'.join(drives).encode('utf-16le')).decode('ascii')
    t = base64.b64encode(title.encode('utf-16le')).decode('ascii')

    script = (
        "Add-Type -AssemblyName System.Windows.Forms;Add-Type -AssemblyName System.Drawing;$t=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
        f"{t}"
        "'));$items=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
        f"{values}"
        "')).Split('|');$wa=[System.Windows.Forms.Screen]::PrimaryScreen.WorkingArea;$DW=440;$DH=245;$FW=[Math]::Min($DW,[Math]::Max(320,$wa.Width-24));$FH=[Math]::Min($DH,[Math]::Max(240,$wa.Height-24));$f=New-Object System.Windows.Forms.Form;$f.Text=$t;$f.Size=New-Object Drawing.Size($FW,$FH);$f.StartPosition='CenterScreen';$f.BackColor=[Drawing.Color]::White;$f.FormBorderStyle='FixedDialog';$f.MaximizeBox=$false;$l=New-Object System.Windows.Forms.Label;$l.Text='选择要制作成安全信鸽的 U 盘。部署会清空其中的全部文件。';$l.Location=New-Object Drawing.Point(28,22);$l.Size=New-Object Drawing.Size(384,40);$l.Font=New-Object Drawing.Font('Microsoft YaHei UI',10);$l.ForeColor=[Drawing.ColorTranslator]::FromHtml('#6E6E73');$f.Controls.Add($l);$x=New-Object System.Windows.Forms.ComboBox;$x.Location=New-Object Drawing.Point(28,72);$x.Size=New-Object Drawing.Size(384,32);$x.DropDownStyle='DropDownList';$x.Font=New-Object Drawing.Font('Microsoft YaHei UI',10);[void]$x.Items.AddRange($items);if($x.Items.Count -gt 0){$x.SelectedIndex=0};$f.Controls.Add($x);$b=New-Object System.Windows.Forms.Button;$b.Text='继续';$b.Location=New-Object Drawing.Point(332,125);$b.Size=New-Object Drawing.Size(84,32);$b.FlatStyle='Flat';$b.BackColor=[Drawing.ColorTranslator]::FromHtml('#1677FF');$b.ForeColor=[Drawing.Color]::White;$b.Font=New-Object Drawing.Font('Microsoft YaHei UI',10);$b.FlatAppearance.BorderSize=0;$f.Controls.Add($b);$b.Add_Click({if($x.SelectedItem){$f.Tag=[Convert]::ToBase64String([Text.Encoding]::Unicode.GetBytes([string]$x.SelectedItem));$f.Close()}});[void]$f.ShowDialog();if($f.Tag){Write-Output $f.Tag}"
    )
    return _ps_value(script)


def gui_notice(title: str, body: str, error: bool = False) -> None:
    try:
        t = base64.b64encode(title.encode('utf-16le')).decode('ascii')
        b = base64.b64encode(body.encode('utf-16le')).decode('ascii')
        ok = base64.b64encode('确定'.encode('utf-16le')).decode('ascii')
        accent = '#34C759' if not error else '#FF3B30'
        _ps_value(
            "Add-Type -AssemblyName System.Windows.Forms;Add-Type -AssemblyName System.Drawing;$accent=[Drawing.ColorTranslator]::FromHtml('"
            + accent
            + "');$t=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
            + t
            + "'));$b=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
            + b
            + "'));$ok=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
            + ok
            + "'));$wa=[System.Windows.Forms.Screen]::PrimaryScreen.WorkingArea;$DW=420;$DH=375;$FW=[Math]::Min($DW,[Math]::Max(320,$wa.Width-24));$FH=[Math]::Min($DH,[Math]::Max(240,$wa.Height-24));$f=New-Object Windows.Forms.Form;$f.Text='';$f.Size=New-Object Drawing.Size($FW,$FH);$f.StartPosition='CenterScreen';$f.FormBorderStyle='FixedDialog';$f.MaximizeBox=$false;$f.MinimizeBox=$false;$f.ShowInTaskbar=$false;$f.BackColor=[Drawing.Color]::White;$s=New-Object Windows.Forms.Panel;$s.Size=New-Object Drawing.Size(420,4);$s.Location=New-Object Drawing.Point(0,0);$s.BackColor=$accent;$f.Controls.Add($s);$l=New-Object Windows.Forms.Label;$l.Text=$t;$l.Location=New-Object Drawing.Point(32,26);$l.Size=New-Object Drawing.Size(356,24);$l.Font=New-Object Drawing.Font('Microsoft YaHei UI',13,[Drawing.FontStyle]::Bold);$l.ForeColor=[Drawing.ColorTranslator]::FromHtml('#1C1C1E');$f.Controls.Add($l);$m=New-Object Windows.Forms.Label;$m.Text=$b;$m.Location=New-Object Drawing.Point(32,56);$m.Size=New-Object Drawing.Size(356,205);$m.Font=New-Object Drawing.Font('Microsoft YaHei UI',10);$m.ForeColor=[Drawing.ColorTranslator]::FromHtml('#6E6E73');$m.TextAlign='TopLeft';$f.Controls.Add($m);$k=New-Object Windows.Forms.Button;$k.Text=$ok;$k.Location=New-Object Drawing.Point(310,270);$k.Size=New-Object Drawing.Size(84,32);$k.Font=New-Object Drawing.Font('Microsoft YaHei UI',10);$k.FlatStyle='Flat';$k.BackColor=[Drawing.Color]::White;$k.ForeColor=[Drawing.ColorTranslator]::FromHtml('#1677FF');$k.FlatAppearance.BorderSize=0;$k.DialogResult=[Windows.Forms.DialogResult]::OK;$f.AcceptButton=$k;$f.Controls.Add($k);[void]$f.ShowDialog()"
        )
    except Exception:
        pass
    return None


def hide(path: Path) -> None:
    subprocess.run(
        ['attrib', '+h', str(path)],
        check=False,
        capture_output=True,
        creationflags=134217728,
    )


def create_inbox_shortcut(root: Path) -> Path:
    shortcut = root / INBOX_SHORTCUT_NAME
    try:
        shortcut.unlink()
    except OSError:
        pass
    tmp = root / 'inbox-tmp.lnk'
    try:
        tmp.unlink()
    except OSError:
        pass

    # NTFS volume roots cannot hold a Chinese-named .lnk directly, so build an
    # ASCII temp name first and rename it afterwards.
    quoted_tmp = str(tmp).replace("'", "''")
    quoted_target = str(root / INBOX_NAME / INBOX_EXE_NAME).replace("'", "''")
    quoted_root = str(root).replace("'", "''")

    script = (
        "$s=New-Object -ComObject WScript.Shell;\n"
        f"$l=$s.CreateShortcut('{quoted_tmp}');\n"
        f"$l.TargetPath='{quoted_target}';\n"
        f"$l.WorkingDirectory='{quoted_root}';\n"
        f"$l.IconLocation='{quoted_target},0';\n"
        "$l.Save()\n"
    )
    path = _ps_product_script(script)
    result = subprocess.run(
        [
            'powershell.exe',
            '-NoProfile',
            '-STA',
            '-ExecutionPolicy',
            'RemoteSigned',
            '-File',
            path,
        ],
        capture_output=True,
        text=True,
        encoding='utf-8',
        errors='replace',
        creationflags=134217728,
        stdin=subprocess.DEVNULL,
    )
    if result.returncode != 0 or not tmp.exists():
        raise MailboxError('Unable to create the visible delivery shortcut.')
    os.replace(tmp, shortcut)
    if not shortcut.exists():
        raise MailboxError('Unable to create the visible delivery shortcut.')
    return shortcut


def create_retrieve_shortcut(root: Path) -> Path:
    """在 U 盘根建「取件箱.lnk」，指向 .pigeon.retrieve/取件箱.exe（作者用）。

    与 create_inbox_shortcut 同款：NTFS 卷根不能直接保存中文 .lnk，故先建
    ASCII 临时名再重命名为中文；脚本经 _ps_product_script（UTF-8 BOM）+ -File 跑。
    """
    shortcut = root / RETRIEVE_SHORTCUT_NAME
    try:
        shortcut.unlink()
    except OSError:
        pass
    tmp = root / 'retrieve-tmp.lnk'
    try:
        tmp.unlink()
    except OSError:
        pass

    quoted_tmp = str(tmp).replace("'", "''")
    quoted_target = str(root / RETRIEVE_NAME / RETRIEVE_EXE_NAME).replace("'", "''")
    quoted_root = str(root).replace("'", "''")

    script = (
        "$s=New-Object -ComObject WScript.Shell;\n"
        f"$l=$s.CreateShortcut('{quoted_tmp}');\n"
        f"$l.TargetPath='{quoted_target}';\n"
        f"$l.WorkingDirectory='{quoted_root}';\n"
        f"$l.IconLocation='{quoted_target},0';\n"
        "$l.Save()\n"
    )
    path = _ps_product_script(script)
    result = subprocess.run(
        [
            'powershell.exe',
            '-NoProfile',
            '-STA',
            '-ExecutionPolicy',
            'RemoteSigned',
            '-File',
            path,
        ],
        capture_output=True,
        text=True,
        encoding='utf-8',
        errors='replace',
        creationflags=134217728,
        stdin=subprocess.DEVNULL,
    )
    if result.returncode != 0 or not tmp.exists():
        raise MailboxError('Unable to create the retrieve shortcut.')
    os.replace(tmp, shortcut)
    if not shortcut.exists():
        raise MailboxError('Unable to create the retrieve shortcut.')
    return shortcut


def filesystem_type(root: Path) -> str:
    volume = ctypes.create_unicode_buffer(261)
    filesystem = ctypes.create_unicode_buffer(261)
    serial = ctypes.c_uint32()
    maximum = ctypes.c_uint32()
    flags = ctypes.c_uint32()
    ok = ctypes.windll.kernel32.GetVolumeInformationW(
        str(root),
        volume,
        len(volume),
        ctypes.byref(serial),
        ctypes.byref(maximum),
        ctypes.byref(flags),
        filesystem,
        len(filesystem),
    )
    if not ok:
        raise OSError(ctypes.get_last_error(), 'Cannot read target filesystem type.')
    return filesystem.value.upper()


def run_icacls(path: Path, args: str) -> None:
    result = subprocess.run(
        ['icacls', str(path), *args],
        capture_output=True,
        text=True,
        creationflags=134217728,
    )
    if result.returncode != 0:
        detail = result.stderr.strip() or result.stdout.strip()
        raise MailboxError('Cannot enable NTFS deletion protection: ' + detail)


def lock_file(path: Path, mailbox: bool = False, executable: bool = False) -> None:
    """Set the minimum required access without any deny ACE.

    实测在文件上添加 deny(D)/(W) 会让 Windows 打开该文件句柄时被拒——
    即使管理员也无法读取，导致收件箱读 .pigeon.public.pem / .pigeon.license
    失败，激活与投递全部失效。删除保护完全由根目录 lock_root 的
    DELETE_CHILD deny 提供，文件级 deny 既多余又有害，因此不再添加。
    """
    rights = '(R,W)' if mailbox else ('(R,X)' if executable else '(R)')
    run_icacls(path, '/inheritance:r')
    run_icacls(path, '/grant:r', f'{EVERYONE}:{rights}')
    run_icacls(path, '/grant', f'{ADMINISTRATORS}:(F)')
    run_icacls(path, '/grant', f'{SYSTEM}:(F)')


def lock_folder(path: Path) -> None:
    """递归锁定 onedir 目录树为只读+可执行（收件箱不解压直接运行）。

    注意：/inheritance:r 与 /t 同时使用会导致子项失败（RC=5），因此
    /inheritance:r 只对目录本身执行，递归由 /grant /t 完成。
    """
    run_icacls(path, '/inheritance:r')
    run_icacls(path, '/grant:r', f'{EVERYONE}:(R,X)', '/t')
    run_icacls(path, '/grant', f'{ADMINISTRATORS}:(F)', '/t')
    run_icacls(path, '/grant', f'{SYSTEM}:(F)', '/t')


def lock_shortcut(path: Path) -> None:
    """Lock a delivery shortcut without denying write/delete.

    实测对 .lnk 加 deny(W)/(D) 会让 Windows 报「没有应用程序与此操作的
    指定文件有关联」，无法解析或启动快捷方式。删除保护由根目录的
    lock_root（deny DC,WD,AD）提供。
    """
    run_icacls(path, '/inheritance:r')
    run_icacls(path, '/grant:r', f'{EVERYONE}:(R)')
    run_icacls(path, '/grant', f'{ADMINISTRATORS}:(F)')
    run_icacls(path, '/grant', f'{SYSTEM}:(F)')


def lock_root(root: Path) -> None:
    run_icacls(root, '/deny', f'{EVERYONE}:(DC,WD,AD)')


def reset_acls(root: Path) -> None:
    """Remove a previous deployment's delete-protection ACLs so the drive can
    be cleared and rewritten.  Best-effort: a fresh drive has none, and the
    deployer owns the prior files so the DACL can be reset.  The root's
    ``(DENY)(WD,AD,DC)`` ACE would otherwise both block clearing the drive [...]"""
    subprocess.run(
        ['icacls', str(root), '/reset', '/t', '/c'],
        capture_output=True,
        text=True,
        check=False,
        creationflags=134217728,
    )


class DeploymentProgress:
    def __init__(self) -> None:
        self._ps_script = None
        self.path = (
            Path(tempfile.gettempdir())
            / f'secure-pigeon-deploy-{uuid.uuid4().hex}.status'
        )
        self.update(1, '正在准备部署')
        script = (
            "$p=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
            f"{base64.b64encode(str(self.path).encode('utf-16le')).decode('ascii')}"
            "'));Add-Type -AssemblyName System.Windows.Forms;Add-Type -AssemblyName System.Drawing;$wa=[System.Windows.Forms.Screen]::PrimaryScreen.WorkingArea;$DW=440;$DH=195;$FW=[Math]::Min($DW,[Math]::Max(320,$wa.Width-24));$FH=[Math]::Min($DH,[Math]::Max(240,$wa.Height-24));$f=New-Object System.Windows.Forms.Form;$f.Text='安全信鸽 · 正在部署';$f.Size=New-Object Drawing.Size($FW,$FH);$f.StartPosition='CenterScreen';$f.BackColor=[Drawing.Color]::White;$f.FormBorderStyle='FixedDialog';$f.ControlBox=$false;$title=New-Object System.Windows.Forms.Label;$title.Text='正在创建安全信鸽 U 盘';$title.Location=New-Object Drawing.Point(28,24);$title.Size=New-Object Drawing.Size(384,24);$title.Font=New-Object Drawing.Font('Microsoft YaHei UI',13,[Drawing.FontStyle]::Bold);$title.ForeColor=[Drawing.ColorTranslator]::FromHtml('#1C1C1E');$f.Controls.Add($title);$l=New-Object System.Windows.Forms.Label;$l.Location=New-Object Drawing.Point(28,56);$l.Size=New-Object Drawing.Size(384,40);$l.Font=New-Object Drawing.Font('Microsoft YaHei UI',9);$l.ForeColor=[Drawing.ColorTranslator]::FromHtml('#6E6E73');$f.Controls.Add($l);$track=New-Object System.Windows.Forms.Panel;$track.Size=New-Object Drawing.Size(384,4);$track.Location=New-Object Drawing.Point(28,110);$track.BackColor=[Drawing.ColorTranslator]::FromHtml('#F0F0F0');$f.Controls.Add($track);$fill=New-Object System.Windows.Forms.Label;$fill.Size=New-Object Drawing.Size(0,4);$fill.Location=New-Object Drawing.Point(0,0);$fill.BackColor=[Drawing.ColorTranslator]::FromHtml('#1677FF');$track.Controls.Add($fill);$n=New-Object System.Windows.Forms.Label;$n.Text='请勿拔出 U 盘或关闭电脑';$n.Location=New-Object Drawing.Point(28,126);$n.Size=New-Object Drawing.Size(384,20);$n.ForeColor=[Drawing.ColorTranslator]::FromHtml('#AEAEB2');$n.Font=New-Object Drawing.Font('Microsoft YaHei UI',8);$f.Controls.Add($n);$t=New-Object System.Windows.Forms.Timer;$t.Interval=250;$t.Add_Tick({if(Test-Path -LiteralPath $p){$v=(Get-Content -LiteralPath $p -Raw -Encoding UTF8).Split([char]9);if($v.Length -ge 3){$pct=[Math]::Max(0,[Math]::Min(100,[int]$v[1]));$fill.Size=New-Object Drawing.Size([Math]::Floor(384*$pct/100),4);$l.Text=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String($v[2]));if($v[0] -ne 'RUN'){$fill.BackColor=[Drawing.ColorTranslator]::FromHtml('#34C759');$t.Stop();$f.Close()}}}});$f.Add_Shown({$t.Start()});[void]$f.ShowDialog()"
        )
        self._ps_script = _ps_product_script(script)
        self.process = subprocess.Popen(
            [
                'powershell.exe',
                '-NoProfile',
                '-STA',
                '-ExecutionPolicy',
                'RemoteSigned',
                '-File',
                self._ps_script,
            ],
            creationflags=134217728,
        )

    def update(self, percent: int, message: str, state: str = 'RUN') -> None:
        encoded_message = base64.b64encode(message.encode('utf-16le')).decode('ascii')
        self.path.write_text(
            f'{state}\t{percent}\t{encoded_message}', encoding='utf-8'
        )

    def close(self, failed: bool = False) -> None:
        self.update(
            100 if not failed else 0,
            '部署未完成' if failed else '部署完成',
            'ERROR' if failed else 'DONE',
        )
        try:
            self.process.wait(timeout=3)
        except subprocess.TimeoutExpired:
            self.process.terminate()
        try:
            self.path.unlink()
        except OSError:
            return None


def main() -> int:
    parser = argparse.ArgumentParser(description='Secure Pigeon USB deployment')
    parser.add_argument('--drive', help='Target USB root, e.g. X:\\')
    parser.add_argument('--inbox', help='Path to the packaged inbox application')
    parser.add_argument('--author-kit', help='Folder that stores the author private key')
    parser.add_argument('--self-test', action='store_true', help=argparse.SUPPRESS)
    args = parser.parse_args()

    if args.self_test:
        try:
            import qrcode  # noqa: F401
            from PIL import Image  # noqa: F401

            with tempfile.TemporaryDirectory() as folder:
                seed = os.urandom(20)
                qr = Path(folder) / 'self-test.png'
                save_qr(seed, qr)
                if qr.stat().st_size < 100:
                    return 91
                new_rsa_keypair()
            return 0
        except Exception:
            return 92

    program_dir = Path(sys.argv[0]).resolve().parent
    drive_text = args.drive or gui_drive('安全信鸽 · 选择 U 盘').strip()
    root = Path(drive_text + '\\' if len(drive_text) == 2 and drive_text[1] == ':' else drive_text).resolve()

    inbox = Path(args.inbox).resolve() if args.inbox else None
    if inbox is None:
        same_level = program_dir / PACKAGED_INBOX_NAME
        parent_level = program_dir.parent / PACKAGED_INBOX_NAME
        inbox = same_level if same_level.is_dir() else (
            parent_level if parent_level.is_dir() else same_level
        )

        r_same = program_dir / PACKAGED_RETRIEVE_NAME
        r_parent = program_dir.parent / PACKAGED_RETRIEVE_NAME
        retrieve_dir = r_same if r_same.is_dir() else (
            r_parent if r_parent.is_dir() else r_same
        )

    default_kit = Path.home() / 'Documents' / DEFAULT_KIT_NAME
    kit_text = args.author_kit or gui_folder(
        '安全信鸽 · 保存密钥',
        '选择密钥文件保存位置。密钥文件不会存入 U 盘，请妥善备份。',
        str(default_kit),
    ).strip()
    author_kit = Path(kit_text).resolve() if kit_text else default_kit.resolve()
    private_path = author_kit / PRIVATE_NAME

    try:
        import qrcode  # noqa: F401
        from PIL import Image  # noqa: F401
    except ImportError as exc:
        gui_notice('部署文件不完整', f'部署程序缺少二维码组件，尚未修改 U 盘。\n\n{exc}', True)
        return 1

    if not (root.drive and root.exists() and root.is_dir()):
        gui_notice('无法开始部署', '请选择有效的 U 盘根目录，例如 E:\\。', True)
        return 1

    if root.drive.upper() == 'C:':
        gui_notice('无法开始部署', '不能选择 Windows 系统盘 C:。请选择 U 盘。', True)
        return 1

    if filesystem_type(root) != 'NTFS':
        gui_notice('无法开始部署', '目标 U 盘需要 NTFS 格式，才能启用防删除保护。', True)
        return 1

    if shutil.disk_usage(root).total < 536870912:
        gui_notice('U 盘容量不足', '目标 U 盘容量至少需要 512 MB，尚未修改 U 盘。', True)
        return 1

    if not inbox.is_dir():
        gui_notice(
            '部署文件不完整',
            '请将”收件箱”文件夹和”一键部署到U盘.exe”放在同一文件夹后重试。',
            True,
        )
        return 1

    if not retrieve_dir.is_dir():
        gui_notice(
            '部署文件不完整',
            '请将“取件箱”文件夹也和“一键部署到U盘.exe”放在同一文件夹后重试。',
            True,
        )
        return 1

    if private_path.exists():
        gui_notice(
            '无法创建密钥',
            f'所选文件夹已有密钥文件。为防止覆盖，尚未修改 U 盘。\n\n请选择一个新的空文件夹：\n{private_path}',
            True,
        )
        return 1

    try:
        author_kit.mkdir(parents=True, exist_ok=True)
        probe = author_kit / '.pigeon-write-test'
        probe.write_bytes(b'')
        probe.unlink()
    except OSError as exc:
        gui_notice('无法保存密钥', f'所选文件夹不可写，尚未修改 U 盘。\n\n{exc}', True)
        return 1

    password = gui_input('安全信鸽 · 设置密钥密码', '设置密钥文件密码（至少 10 位）。', secret=True)
    if len(password) < 10:
        gui_notice('密码太短', '密钥文件密码至少需要 10 位，尚未修改 U 盘。', True)
        return 1
    if password != gui_input('安全信鸽 · 确认密钥密码', '请再次输入相同的密码。', secret=True):
        gui_notice('密码不一致', '两次输入的密码不同，尚未修改 U 盘。', True)
        return 1

    existing = [
        p for p in root.iterdir()
        if p.name not in frozenset({'$RECYCLE.BIN', 'System Volume Information'})
    ]
    if existing:
        print('Existing files on target:')
        for item in existing:
            print(' -', item.name)

    answer = gui_input(
        '安全信鸽 · 最后确认',
        f'即将清空 {root} 中的所有文件，并创建仅能投递的文件信箱。\n输入 DEPLOY 后开始。',
    )
    if answer != 'DEPLOY':
        return 2

    public_path = root / '.pigeon.public.pem'
    progress = DeploymentProgress()
    try:
        progress.update(3, '正在清理目标 U 盘')
        reset_acls(root)
        total_items = len(existing)
        for idx, item in enumerate(existing, 1):
            pct = 3 + int(4 * idx / max(1, total_items))
            progress.update(pct, f'正在清理目标 U 盘（{idx}/{total_items}）')
            if item.is_dir():
                shutil.rmtree(item)
            else:
                item.unlink()

        progress.update(8, '正在生成密钥文件')
        write_author_keypair(private_path, public_path, password)

        progress.update(14, '正在创建临时取件凭据')
        temporary_private_key = new_rsa_keypair()
        temporary_seed = provision_temporary_access(root, temporary_private_key)
        qr_path = author_kit / QR_NAME
        save_qr(temporary_seed, qr_path)
        progress.update(20, '动态验证码二维码已生成，请扫码保存')
        try:
            os.startfile(str(qr_path))
        except OSError:
            pass

        progress.update(22, '正在生成 U 盘激活凭据')
        master_key = generate_master_key()
        device_id = get_device_id(root)

        activation_code = generate_activation_code(device_id, master_key)
        deploy_license(root, master_key, code_hash(activation_code))

        master_key_path = author_kit / f'激活主密钥-{device_id}.txt'
        master_key_path.write_text(
            f'安全信鸽 — 授权密钥（请妥善保存）\n\n设备 ID：{device_id}\n授权密钥 (HEX)：{master_key.hex()}\nU 盘：{root}\n生成时间：{time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())}\n\n此文件用于为买家生成激活码，请妥善保存，切勿交给买家。\n需要时用赋码工具生成激活码：\n  python activation_tool.py --device-id {device_id} --master-key {master_key.hex()} --order 订单号\n',
            encoding='utf-8',
        )

        progress.update(23, '正在写入使用说明')
        readme = root / '使用说明.txt'
        readme.write_text(
            '安全信鸽 U 盘 · 使用说明\n========================\n\n把文件放进 U 盘的方法：\n\n1. 双击 U 盘里的「收件箱」。\n2. 在弹出窗口里选择要放入的一个或多个文件。\n3. 等进度条走完，看到「投递完成」就成功了。\n\n【注意事项】\n· 不要删除、改名或移动 U 盘里的任何文件，尤其是「收件箱」。\n· 不要格式化 U 盘，否则所有已放入的文件将永久丢失。\n\n如有问题，请直接联系收件人。\n',
            encoding='utf-8',
        )

        (root / TRIAL_NAME).write_text('PENDING\n', encoding='ascii')

        progress.update(24, '正在写入投递程序')
        inbox_folder = root / INBOX_NAME

        total_inbox_files = sum(1 for _ in Path(inbox).rglob('*') if _.is_file())
        copied = [0]

        def _counting_copy(src, dst, *, follow_symlinks=True):
            shutil.copy2(src, dst, follow_symlinks=follow_symlinks)
            copied[0] += 1
            pct = 24 + int(1 * copied[0] / max(1, total_inbox_files))
            progress.update(pct, f'正在写入投递程序（{copied[0]}/{total_inbox_files}）')

        shutil.copytree(inbox, inbox_folder, dirs_exist_ok=True, copy_function=_counting_copy)

        built_exe = inbox_folder / 'Inbox.exe'
        final_exe = inbox_folder / INBOX_EXE_NAME
        if built_exe.exists() and built_exe != final_exe:
            built_exe.rename(final_exe)
        inbox_shortcut = create_inbox_shortcut(root)

        retrieve_folder = root / RETRIEVE_NAME
        total_retrieve_files = sum(1 for _ in Path(retrieve_dir).rglob('*') if _.is_file())
        rcopied = [0]

        def _counting_copy_r(src, dst, *, follow_symlinks=True):
            shutil.copy2(src, dst, follow_symlinks=follow_symlinks)
            rcopied[0] += 1
            progress.update(24, f'正在写入取件箱（{rcopied[0]}/{total_retrieve_files}）')

        shutil.copytree(retrieve_dir, retrieve_folder, dirs_exist_ok=True, copy_function=_counting_copy_r)
        retrieve_shortcut = create_retrieve_shortcut(root)

        free = shutil.disk_usage(root).free
        visible_reserve = 4096
        if free <= visible_reserve + 2097152:
            raise MailboxError(
                'Too little space remains after placing the inbox and retrieve applications.'
            )
        store = root / '.pigeon.store'
        store_capacity = free - visible_reserve

        def report_store(done: int, total: int) -> None:
            percent = 25 + int(65 * done / max(1, total))
            progress.update(
                percent,
                f'正在创建文件仓：{done / 1024 / 1024:.0f} / {total / 1024 / 1024:.0f} MB',
            )

        initialize_store(
            store,
            load_public_key(public_path),
            load_public_key(root / PUBLIC_NAME),
            store_capacity,
            report_store,
        )

        blank = root / '.blank.bin'
        blank.write_bytes(b'')

        for path in (
            root / INBOX_NAME,
            root / RETRIEVE_NAME,
            public_path,
            root / PUBLIC_NAME,
            root / ACCESS_NAME,
            root / LICENSE_NAME,
            root / TRIAL_NAME,
            store,
            blank,
        ):
            hide(path)

        progress.update(92, '正在隐藏内部文件并启用防删除保护')
        lock_folder(inbox_folder)
        lock_shortcut(inbox_shortcut)
        lock_folder(retrieve_folder)
        lock_shortcut(retrieve_shortcut)
        lock_file(public_path)
        lock_file(root / PUBLIC_NAME)
        lock_file(root / ACCESS_NAME, mailbox=True)
        lock_file(root / LICENSE_NAME, mailbox=True)
        lock_file(blank)
        lock_file(store, mailbox=True)
        lock_file(readme)
        lock_root(root)

        progress.update(99, '正在完成最后设置')
        progress.close()
        gui_notice(
            '部署完成',
            f'安全信鸽 U 盘已创建。\n\n设备 ID：{device_id}\n\n激活主密钥文件：\n{master_key_path}\n\n首次使用收件箱时，可选择 7 天免费试用或立即激活。\n\n请妥善保存密钥文件：\n{private_path}',
        )
        return 0
    except Exception as exc:
        progress.close(failed=True)
        gui_notice(
            '部署未完成',
            f'部署未完成。请不要拔出 U 盘，并保留已生成的密钥文件。\n\n{exc}',
            True,
        )
        raise


if __name__ == '__main__':
    try:
        raise SystemExit(main())
    except SystemExit:
        raise
    except Exception as exc:
        try:
            gui_notice('部署未完成', f'发生意外错误，尚未完成部署。\n\n{exc}', True)
        except Exception:
            try:
                ctypes.windll.user32.MessageBoxW(0, str(exc), '安全信鸽 · 部署未完成', 16)
            except Exception:
                pass
        raise SystemExit(1)
