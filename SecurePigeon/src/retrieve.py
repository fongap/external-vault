"""取件箱 — 从安全信鸽 U 盘导出文件，自动判断密钥或验证码方式。"""

from __future__ import annotations

import base64
import ctypes
import os
import subprocess
import sys  # noqa: F401  (imported in the original binary but never referenced)
import tempfile
import uuid
from pathlib import Path

from pigeon_store import MailboxError, extract_all, load_private_key
from temporary_access import ACCESS_NAME, verify_and_consume  # noqa: F401

NO_WINDOW = 0x08000000  # 134217728, CREATE_NO_WINDOW
STORE_NAME = ".pigeon.store"


def _ps_temp_file(script: str) -> Path:
    """Write *script* to a UTF-8-BOM temp ``.ps1`` and return its path.

    Using ``-File`` instead of ``-EncodedCommand`` drops the base64+utf-16le
    command-line signature that heuristic AV engines flag heavily.

    (docstring tail beyond this point was truncated in the disassembly dump)
    """
    fd, name = tempfile.mkstemp(suffix=".ps1", prefix="pigeon-ps-")
    os.write(fd, b"\xef\xbb\xbf")
    os.write(fd, script.encode("utf-8"))
    os.close(fd)
    return Path(name)


def _ps_args(script_path: Path) -> list[str]:
    return [
        "powershell.exe",
        "-NoProfile",
        "-STA",
        "-ExecutionPolicy",
        "RemoteSigned",
        "-File",
        str(script_path),
    ]


def ps(script: str) -> subprocess.CompletedProcess[str]:
    """Run a PowerShell script (UTF-8-BOM temp file, ``-File``) and wait.

    Single call site is ``request_retrieval`` (synchronous dialog); the temp
    ``.ps1`` is deleted once the child has finished running.

    (docstring tail beyond this point was truncated in the disassembly dump)
    """
    path = _ps_temp_file(script)
    command = _ps_args(path)
    try:
        try:
            return subprocess.run(
                command,
                capture_output=True,
                text=True,
                encoding="utf-8",
                errors="replace",
                creationflags=NO_WINDOW,
                stdin=subprocess.DEVNULL,
            )
        except FileNotFoundError:
            return subprocess.CompletedProcess(
                command,
                -1,
                stdout="",
                stderr="PowerShell not found",
            )
    finally:
        try:
            path.unlink()
        except OSError:
            pass


def field(value: str) -> str:
    return base64.b64encode(value.encode("utf-16le")).decode("ascii")


def _logical_roots() -> list[str]:
    """Return mounted drive roots ``['C:\\', 'D:\\', ...]`` (forward slashes norm. away)."""
    mask = ctypes.windll.kernel32.GetLogicalDrives()
    return [f"{chr(65 + i)}:\\" for i in range(26) if mask & (1 << i)]


DRIVE_REMOVABLE = 2


def _drive_type(root: str) -> int:
    """Win32 GetDriveTypeW: 2=removable, 3=fixed, 4=remote, 5=cdrom, 6=ramdisk."""
    return ctypes.windll.kernel32.GetDriveTypeW(ctypes.c_wchar_p(root))


def _is_removable_drive(path: Path) -> bool:
    """True 仅当 path 所在卷是可移动磁盘。

    原实现仅凭根目录存在 ``.pigeon.store`` 标记文件识别"U 盘"，固定硬盘/
    网络映射盘/光驱上的同名文件同样会被接受。加驱动器类型校验后，只有
    可移动磁盘上的文件仓才会出现在选择列表中。
    """
    try:
        return _drive_type(str(path.drive or path)) == DRIVE_REMOVABLE
    except OSError:
        return False


def discover_usb_stores() -> list[Path]:
    """Enumerate every mounted volume that carries a pigeon store, returning the
    full ``store`` path for each.  Drives without ``.pigeon.store`` are skipped.

    Single-drive deployments yield a one-element list.
    """
    stores: list[Path] = []
    for root in _logical_roots():
        store = Path(root) / STORE_NAME
        if not store.is_file():
            continue
        if not _is_removable_drive(store):
            continue
        stores.append(store)
    return stores


def resolve_store(choice: str) -> Path:
    """Resolve a user selection to a validated ``.pigeon.store`` path.

    Accepts either a drive root (``X:\\`` — the value the form's drive combo
    submits) or an explicit store path.  Raises if no store is present.
    """
    if not choice:
        raise MailboxError("未检测到安全信鸽 U 盘，请插入后重试。")
    resolved = Path(choice).resolve()
    store = resolved if resolved.name == STORE_NAME else resolved / STORE_NAME
    if not store.is_file():
        raise MailboxError("所选位置不是安全信鸽 U 盘。")
    if not _is_removable_drive(store):
        raise MailboxError("所选位置不在可移动 U 盘上，为安全起见已拒绝。")
    return store


def _ensure_powershell() -> bool:
    """PowerShell 缺失时弹原生对话框提示并返回 False。

    原实现中 ``ps()`` 失败只写入 stderr，而调用方只扫描 stdout，最终静默
    退出 0，用户看不到任何错误。"""
    import shutil

    if shutil.which("powershell.exe"):
        return True
    try:
        ctypes.windll.user32.MessageBoxW(
            0,
            "未找到 PowerShell（powershell.exe），无法显示界面。\n请修复系统组件后重试。",
            "安全信鸽 · 取件箱",
            0x10,
        )
    except Exception:
        pass
    return False


def notify(title: str, detail: str, error: bool = False) -> None:
    t = field(title)
    d = field(detail)
    ok = field("确定")
    accent = "#34C759" if not error else "#FF3B30"
    ps(
        "Add-Type -AssemblyName System.Windows.Forms;Add-Type -AssemblyName System.Drawing;$accent=[Drawing.ColorTranslator]::FromHtml('"
        + accent
        + "');$t=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
        + t
        + "'));$d=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
        + d
        + "'));$ok=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
        + ok
        + "'));$wa=[System.Windows.Forms.Screen]::PrimaryScreen.WorkingArea;$DW=420;$DH=285;$FW=[Math]::Min($DW,[Math]::Max(320,$wa.Width-24));$FH=[Math]::Min($DH,[Math]::Max(240,$wa.Height-24));$f=New-Object Windows.Forms.Form;$f.Text='';$f.Size=New-Object Drawing.Size($FW,$FH);$f.StartPosition='CenterScreen';$f.FormBorderStyle='FixedDialog';$f.MaximizeBox=$false;$f.MinimizeBox=$false;$f.ShowInTaskbar=$false;$f.BackColor=[Drawing.Color]::White;$s=New-Object Windows.Forms.Panel;$s.Size=New-Object Drawing.Size(420,4);$s.Location=New-Object Drawing.Point(0,0);$s.BackColor=$accent;$f.Controls.Add($s);$l=New-Object Windows.Forms.Label;$l.Text=$t;$l.Location=New-Object Drawing.Point(32,28);$l.Size=New-Object Drawing.Size(356,24);$l.Font=New-Object Drawing.Font('Microsoft YaHei UI',13,[Drawing.FontStyle]::Bold);$l.ForeColor=[Drawing.ColorTranslator]::FromHtml('#1C1C1E');$f.Controls.Add($l);$m=New-Object Windows.Forms.Label;$m.Text=$d;$m.Location=New-Object Drawing.Point(32,60);$m.Size=New-Object Drawing.Size(356,100);$m.Font=New-Object Drawing.Font('Microsoft YaHei UI',10);$m.ForeColor=[Drawing.ColorTranslator]::FromHtml('#6E6E73');$f.Controls.Add($m);$k=New-Object Windows.Forms.Button;$k.Text=$ok;$k.Location=New-Object Drawing.Point(310,180);$k.Size=New-Object Drawing.Size(84,32);$k.Font=New-Object Drawing.Font('Microsoft YaHei UI',10);$k.FlatStyle='Flat';$k.BackColor=[Drawing.Color]::White;$k.ForeColor=[Drawing.ColorTranslator]::FromHtml('#1677FF');$k.FlatAppearance.BorderSize=0;$k.DialogResult=[Windows.Forms.DialogResult]::OK;$f.AcceptButton=$k;$f.Controls.Add($k);[void]$f.ShowDialog()"
    )


class RetrievalProgress:
    """取件进度窗口（作者导出文件时显示，无取消）。"""

    def __init__(self, total: int = 0) -> None:
        self.path = (
            Path(tempfile.gettempdir())
            / f"secure-pigeon-retrieve-{uuid.uuid4().hex}.status"
        )
        self.total = total
        self._write("RUN", 0)
        self._ps_script = None
        path64 = field(str(self.path))
        title64 = field("安全信鸽 · 正在取件")
        script = (
            "$p=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
            + path64
            + "'));$title=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
            + title64
            + "'));Add-Type -AssemblyName System.Windows.Forms;Add-Type -AssemblyName System.Drawing;$wa=[System.Windows.Forms.Screen]::PrimaryScreen.WorkingArea;$DW=420;$DH=200;$FW=[Math]::Min($DW,[Math]::Max(320,$wa.Width-24));$FH=[Math]::Min($DH,[Math]::Max(180,$wa.Height-24));$f=New-Object Windows.Forms.Form;$f.Text=$title;$f.Size=New-Object Drawing.Size($FW,$FH);$f.StartPosition='CenterScreen';$f.FormBorderStyle='FixedDialog';$f.ControlBox=$false;$f.BackColor=[Drawing.Color]::White;$tl=New-Object Windows.Forms.Label;$tl.Text=$title;$tl.Location=New-Object Drawing.Point(28,20);$tl.Size=New-Object Drawing.Size(364,24);$tl.Font=New-Object Drawing.Font('Microsoft YaHei UI',13,[Drawing.FontStyle]::Bold);$tl.ForeColor=[Drawing.ColorTranslator]::FromHtml('#1C1C1E');$f.Controls.Add($tl);$pct=New-Object Windows.Forms.Label;$pct.Location=New-Object Drawing.Point(28,50);$pct.Size=New-Object Drawing.Size(100,36);$pct.Font=New-Object Drawing.Font('Microsoft YaHei UI',24,[Drawing.FontStyle]::Bold);$pct.ForeColor=[Drawing.ColorTranslator]::FromHtml('#1677FF');$pct.Text='0%';$f.Controls.Add($pct);$track=New-Object Windows.Forms.Panel;$track.Size=New-Object Drawing.Size(364,8);$track.Location=New-Object Drawing.Point(28,96);$track.BackColor=[Drawing.ColorTranslator]::FromHtml('#F0F0F0');$f.Controls.Add($track);$fill=New-Object Windows.Forms.Label;$fill.Size=New-Object Drawing.Size(0,8);$fill.Location=New-Object Drawing.Point(0,0);$fill.BackColor=[Drawing.ColorTranslator]::FromHtml('#1677FF');$track.Controls.Add($fill);$c=New-Object Windows.Forms.Label;$c.Location=New-Object Drawing.Point(28,112);$c.Size=New-Object Drawing.Size(364,18);$c.Font=New-Object Drawing.Font('Microsoft YaHei UI',9);$c.ForeColor=[Drawing.ColorTranslator]::FromHtml('#AEAEB2');$f.Controls.Add($c);$t=New-Object Windows.Forms.Timer;$t.Interval=180;$t.Add_Tick({if(Test-Path -LiteralPath $p){$v=([IO.File]::ReadAllText($p)).Split([char]9);if($v.Length -ge 3){$max=[int]$v[2];$cur=[Math]::Min([int]$v[1],$max);$ratio=$(if($max -gt 0){$cur/$max}else{0});$fillW=[Math]::Floor(364*$ratio);$fill.Size=New-Object Drawing.Size($fillW,8);$pctTxt=[Math]::Floor($ratio*100);$pct.Text=($pctTxt.ToString()+'%');$c.Text='正在导出第 '+$v[1]+' / '+$v[2]+' 个文件';if($v[0] -ne 'RUN'){$t.Stop();$f.Close()}}}});$f.Add_Shown({$t.Start()});[void]$f.ShowDialog()"
        )
        self._ps_script = _ps_temp_file(script)
        self.process = subprocess.Popen(
            _ps_args(self._ps_script),
            creationflags=NO_WINDOW,
            stdin=subprocess.DEVNULL,
        )

    def _write(self, state: str, current: int) -> None:
        self.path.write_text(f"{state}\t{current}\t{self.total}", encoding="utf-8")

    def update(self, done: int, total: int) -> None:
        self.total = total
        self._write("RUN", done)

    def close(self) -> None:
        self._write("DONE", self.total)
        try:
            self.process.wait(timeout=3)
        except subprocess.TimeoutExpired:
            self.process.kill()
            self.process.wait()
        try:
            self.path.unlink()
        except OSError:
            pass
        if self._ps_script is not None:
            try:
                self._ps_script.unlink()
            except OSError:
                pass


def request_retrieval() -> dict | None:
    """Show a single form that accepts either private-key credentials *or* a
    TOTP code.  The caller determines the mode from the returned keys.

    Returns a dict with keys ``mode`` (``'author'`` or ``'temporary'``),
    ``store``, ``key``, ``output`` and ``credential`` — or ``None`` when the
    user closes the form without submitting.

    (docstring tail beyond this point was truncated in the disassembly dump)
    """
    stores = discover_usb_stores()
    default_idx = 0
    if not stores:
        drive_items_b64 = field("")
    else:
        drive_labels = []
        for idx, st in enumerate(stores):
            drive = st.drive
            drive_labels.append(f"{drive}\\")
        drive_items_b64 = field("|".join(drive_labels))
    chosen_default_b64 = field(str(stores[default_idx]) if stores else "")
    output = field(str(Path.home() / "Documents" / "安全信鸽取件"))
    script = f"""Add-Type -AssemblyName System.Windows.Forms;Add-Type -AssemblyName System.Drawing
$wa=[System.Windows.Forms.Screen]::PrimaryScreen.WorkingArea
$DW=560;$DH=624
$FW=[Math]::Min($DW,[Math]::Max(400,$wa.Width-24));$FH=[Math]::Min($DH,[Math]::Max(360,$wa.Height-24))
$SX=$FW/$DW;$SY=$FH/$DH
function P([int]$x,[int]$y){{New-Object Drawing.Point([int]($x*$SX),[int]($y*$SY))}}
function S([int]$w,[int]$h){{New-Object Drawing.Size([int]($w*$SX),[int]($h*$SY))}}
function F([double]$s){{[float]($s*[Math]::Min($SX,$SY))}}
$bd=[Drawing.ColorTranslator]::FromHtml('#1677FF');$bg=[Drawing.Color]::White;$t1=[Drawing.ColorTranslator]::FromHtml('#1C1C1E');$t2=[Drawing.ColorTranslator]::FromHtml('#6E6E73');$card=[Drawing.Color]::FromArgb(247,248,250);$fn='Microsoft YaHei UI'
$driveItems=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('{drive_items_b64}'))
$chosenStore=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('{chosen_default_b64}'))
$outputDefault=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('{output}'))
$f=New-Object System.Windows.Forms.Form;$f.Text='安全信鸽 · 取件箱';$f.Size=New-Object Drawing.Size($FW,$FH);$f.StartPosition='CenterScreen';$f.BackColor=$bg;$f.FormBorderStyle='FixedDialog';$f.MaximizeBox=$false;$f.TopMost=$true
$tl=New-Object System.Windows.Forms.Label;$tl.Text='安全信鸽 · 取件箱';$tl.Location=(P 32 18);$tl.Size=(S 496 26);$tl.Font=New-Object Drawing.Font($fn,(F 15),[Drawing.FontStyle]::Bold);$tl.ForeColor=$t1;$f.Controls.Add($tl)
$st=New-Object System.Windows.Forms.Label;$st.Text='选择目标 U 盘，并填写密钥文件信息或临时取件码（二选一即可）。';$st.Location=(P 32 48);$st.Size=(S 496 20);$st.Font=New-Object Drawing.Font($fn,(F 9));$st.ForeColor=$t2;$f.Controls.Add($st)
$drvLbl=New-Object System.Windows.Forms.Label;$drvLbl.Text='U 盘';$drvLbl.Location=(P 32 72);$drvLbl.Size=(S 60 20);$drvLbl.Font=New-Object Drawing.Font($fn,(F 9));$drvLbl.ForeColor=$t2;$f.Controls.Add($drvLbl)
$drvBox=New-Object System.Windows.Forms.ComboBox;$drvBox.Location=(P 100 70);$drvBox.Size=(S 428 28);$drvBox.DropDownStyle='DropDownList';$drvBox.Font=New-Object Drawing.Font($fn,(F 10));$drvItems=@($driveItems.Split('|')|Where-Object{{$_ -ne ''}});[void]$drvBox.Items.AddRange($drvItems);if($drvBox.Items.Count -gt 0){{$drvBox.SelectedIndex=0}};$f.Controls.Add($drvBox)
function Card([int]$y,[string]$title,[int]$h){{$p=New-Object System.Windows.Forms.Panel;$p.Location=(P 32 $y);$p.Size=(S 496 $h);$p.BackColor=$card;$p.BorderStyle='FixedSingle';$f.Controls.Add($p);$a=New-Object System.Windows.Forms.Label;$a.Text=$title;$a.Location=(P 18 13);$a.Size=(S 460 20);$a.Font=New-Object Drawing.Font($fn,(F 9),[Drawing.FontStyle]::Bold);$a.ForeColor=$t1;$p.Controls.Add($a);return $p}}
$c1=Card 108 '方式一：密钥取件（部署时保存的 .pem 文件 + 密码）' 140
$keyBox=New-Object System.Windows.Forms.TextBox;$keyBox.Location=(P 18 44);$keyBox.Size=(S 358 30);$keyBox.AutoSize=$false;$keyBox.Font=New-Object Drawing.Font($fn,(F 10));$c1.Controls.Add($keyBox)
$browse=New-Object System.Windows.Forms.Button;$browse.Text='选择密钥';$browse.Location=(P 390 44);$browse.Size=(S 90 30);$browse.Font=New-Object Drawing.Font($fn,(F 9));$browse.FlatStyle='Flat';$browse.BackColor=$bg;$browse.ForeColor=$bd;$browse.FlatAppearance.BorderColor=$bd;$browse.FlatAppearance.BorderSize=1;$c1.Controls.Add($browse);$browse.Add_Click({{$d=New-Object System.Windows.Forms.OpenFileDialog;$d.Filter='PEM 密钥文件 (*.pem)|*.pem|所有文件|*.*';if($d.ShowDialog() -eq 'OK'){{$keyBox.Text=$d.FileName}}}})
$pwl=New-Object System.Windows.Forms.Label;$pwl.Text='密钥密码';$pwl.Location=(P 18 78);$pwl.Size=(S 80 16);$pwl.ForeColor=$t2;$pwl.Font=New-Object Drawing.Font($fn,(F 8));$c1.Controls.Add($pwl)
$password=New-Object System.Windows.Forms.TextBox;$password.Location=(P 18 96);$password.Size=(S 460 30);$password.AutoSize=$false;$password.UseSystemPasswordChar=$true;$password.Font=New-Object Drawing.Font($fn,(F 10));$c1.Controls.Add($password)
$div=New-Object System.Windows.Forms.Label;$div.Text='—— 或 ——';$div.Location=(P 32 256);$div.Size=(S 496 16);$div.TextAlign='MiddleCenter';$div.ForeColor=$t2;$div.Font=New-Object Drawing.Font($fn,(F 9));$f.Controls.Add($div)
$c2=Card 280 '方式二：临时取件（输入作者提供的 6 位动态验证码）' 88
$totpBox=New-Object System.Windows.Forms.TextBox;$totpBox.Location=(P 18 44);$totpBox.Size=(S 280 30);$totpBox.AutoSize=$false;$totpBox.Font=New-Object Drawing.Font($fn,(F 10));$totpBox.TextAlign='Center';$totpBox.MaxLength=6;$totpBox.Add_KeyPress({{if($_.KeyChar -ge [char]32 -and -not [char]::IsDigit($_.KeyChar)){{$_.Handled=$true}}}});$c2.Controls.Add($totpBox)
$c3=Card 376 '导出位置' 88
$outputBox=New-Object System.Windows.Forms.TextBox;$outputBox.Text=$outputDefault;$outputBox.Location=(P 18 44);$outputBox.Size=(S 358 30);$outputBox.AutoSize=$false;$outputBox.Font=New-Object Drawing.Font($fn,(F 10));$c3.Controls.Add($outputBox)
$outBrowse=New-Object System.Windows.Forms.Button;$outBrowse.Text='选择';$outBrowse.Location=(P 390 44);$outBrowse.Size=(S 90 30);$outBrowse.Font=New-Object Drawing.Font($fn,(F 9));$outBrowse.FlatStyle='Flat';$outBrowse.BackColor=$bg;$outBrowse.ForeColor=$bd;$outBrowse.FlatAppearance.BorderColor=$bd;$outBrowse.FlatAppearance.BorderSize=1;$c3.Controls.Add($outBrowse);$outBrowse.Add_Click({{$d=New-Object System.Windows.Forms.FolderBrowserDialog;if($d.ShowDialog() -eq 'OK'){{$outputBox.Text=$d.SelectedPath}}}})
$go=New-Object System.Windows.Forms.Button;$go.Text='开始取件';$go.Location=(P 32 478);$go.Size=(S 496 46);$go.FlatStyle='Flat';$go.BackColor=$bd;$go.ForeColor=$bg;$go.Font=New-Object Drawing.Font($fn,(F 11),[Drawing.FontStyle]::Bold);$go.FlatAppearance.BorderSize=0;$f.AcceptButton=$go;$f.Controls.Add($go)
$go.Add_Click({{
if($drvBox.Items.Count -eq 0 -or $null -eq $drvBox.SelectedItem){{[Windows.Forms.MessageBox]::Show('未检测到安全信鸽 U 盘，请插入后重试。','还差一步')|Out-Null;return}}
$storeSel=$drvBox.SelectedItem
if([string]::IsNullOrWhiteSpace($outputBox.Text)){{[Windows.Forms.MessageBox]::Show('请填写导出位置。','还差一步')|Out-Null;$outputBox.Select();return}}
$hasKey=(-not [string]::IsNullOrWhiteSpace($keyBox.Text)) -and (-not [string]::IsNullOrWhiteSpace($password.Text))
$hasTotp=(-not [string]::IsNullOrWhiteSpace($totpBox.Text))
if(-not $hasKey -and -not $hasTotp){{[Windows.Forms.MessageBox]::Show('请填写密钥文件+密码，或输入验证码。','还差一步')|Out-Null;if([string]::IsNullOrWhiteSpace($keyBox.Text)){{$keyBox.Select()}}else{{$totpBox.Select()}};return}}
if($hasKey){{
$f.Tag='OK|author|'+[Convert]::ToBase64String([Text.Encoding]::Unicode.GetBytes($storeSel))+'|'+[Convert]::ToBase64String([Text.Encoding]::Unicode.GetBytes($keyBox.Text))+'|'+[Convert]::ToBase64String([Text.Encoding]::Unicode.GetBytes($outputBox.Text))+'|'+[Convert]::ToBase64String([Text.Encoding]::Unicode.GetBytes($password.Text))
}}else{{
$f.Tag='OK|temporary|'+[Convert]::ToBase64String([Text.Encoding]::Unicode.GetBytes($storeSel))+'||'+[Convert]::ToBase64String([Text.Encoding]::Unicode.GetBytes($outputBox.Text))+'|'+[Convert]::ToBase64String([Text.Encoding]::Unicode.GetBytes($totpBox.Text))
}}
$f.Close()
}})
$f.Add_Shown({{if([string]::IsNullOrWhiteSpace($keyBox.Text)){{$totpBox.Select()}}else{{$password.Select()}}}})
[void]$f.ShowDialog();if($f.Tag){{Write-Output $f.Tag}}
"""
    result = ps(script)
    for line in result.stdout.splitlines():
        line = line.strip()
        if not line.startswith("OK|"):
            continue
        try:
            parts = line.split("|")
            if len(parts) < 4:
                continue
            mode = parts[1]
            values = [
                base64.b64decode(v).decode("utf-16le")
                for v in parts[2:]
            ]
            if len(values) < 4:
                continue
        except (ValueError, IndexError, UnicodeDecodeError):
            continue
        try:
            store_path = resolve_store(values[0])
        except MailboxError as exc:
            notify("无法取件", str(exc), True)
            return None
        return {
            "mode": mode,
            "store": store_path,
            "key": Path(values[1]) if values[1] else None,
            "output": Path(values[2]),
            "credential": values[3],
        }
    return None


def main() -> int:
    if not _ensure_powershell():
        return 1
    params = request_retrieval()
    if params is None:
        return 0

    store = params["store"]
    output = params["output"]
    mode = params["mode"]

    if mode == "author":
        progress = RetrievalProgress()
        try:
            files, corrupted = extract_all(
                store,
                load_private_key(params["key"], params["credential"]),
                output,
                progress=progress.update,
            )
        except (MailboxError, OSError, ValueError) as exc:
            progress.close()
            notify("无法取件", str(exc), True)
            return 1
        progress.close()
        detail = f"已安全导出 {len(files)} 个文件。\n\n保存位置：\n{output}"
        if corrupted:
            detail += f"\n\n（另有 {corrupted} 个记录损坏已跳过，可联系作者处理。）"
        notify("取件完成", detail)
        return 0

    if mode == "temporary":
        seed = None
        private_key = None
        try:
            root = store.parent
            if not root.exists():
                raise MailboxError("U 盘已拔出，本次取件权限已失效。")
            seed, private_key, counter = verify_and_consume(
                root,
                params["credential"],
            )
            progress = RetrievalProgress()
            try:
                if not root.exists():
                    raise MailboxError("U 盘已拔出，本次取件权限已失效。")
                files, corrupted = extract_all(
                    store,
                    private_key,
                    output,
                    True,
                    progress=progress.update,
                )
            except BaseException:
                progress.close()
                raise
            progress.close()
            if not root.exists():
                raise MailboxError("U 盘已拔出，本次取件权限已失效。")
            detail = f"已安全复制 {len(files)} 个文件。拔出 U 盘后，临时取件码即失效。"
            if corrupted:
                detail += f"\n\n（另有 {corrupted} 个记录损坏已跳过，可联系作者处理。）"
            notify("取件完成", detail)
        except (MailboxError, OSError) as exc:
            notify("取件已结束", str(exc), True)
            return 1
        finally:
            del seed
            del private_key
        return 0
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except SystemExit:
        raise
    except Exception as exc:
        try:
            notify("取件异常", f"发生意外错误。\n\n{exc}", True)
        except Exception:
            pass
        raise SystemExit(1)
