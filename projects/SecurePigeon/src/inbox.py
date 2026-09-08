"""收件箱 — 将文件存入安全信鸽 U 盘（Secure Pigeon Inbox）."""

from __future__ import annotations

import base64
import os
import subprocess
import sys
import tempfile
import time
import uuid
from pathlib import Path

from activation import (
    get_cached_code,
    get_device_id,
    is_activated,
    start_trial,
    trial_started,
    verify_and_save,
    verify_public_keys,
    days_remaining,
)


# ---------------------------------------------------------------------------
# U 盘上的固定位置。exe 被预期放在 U 盘根目录下的一级子目录里：
# USB_ROOT = exe 所在目录的上一级（即 U 盘根）。
# ---------------------------------------------------------------------------
PUBLIC_NAME = '.pigeon.temp.public.pem'

USB_ROOT = Path(sys.argv[0]).resolve().parent.parent
AUTHOR_PUBLIC = USB_ROOT / '.pigeon.public.pem'
TEMPORARY_PUBLIC = USB_ROOT / PUBLIC_NAME
STORE = USB_ROOT / '.pigeon.store'
CREATE_NO_WINDOW = 134217728


def _ps_temp_file(script: str) -> Path:
    """Write *script* to a UTF-8-BOM temp ``.ps1`` and return its path.

    Using ``-File`` instead of ``-EncodedCommand`` drops the base64+utf-16le
    command-line signature that heuristic AV engines flag on suspicious
    one-liners.
    """
    fd, name = tempfile.mkstemp(suffix='.ps1', prefix='pigeon-ps-')
    os.write(fd, b'\xef\xbb\xbf')
    os.write(fd, script.encode('utf-8'))
    os.close(fd)
    return Path(name)


def run_powershell(script: str) -> subprocess.CompletedProcess[str]:
    """Run a PowerShell script (UTF-8-BOM temp file, ``-File``) and wait.
    The temp ``.ps1`` is deleted once the child has finished.
    """
    path = _ps_temp_file(script)
    command = [
        'powershell.exe',
        '-NoProfile',
        '-STA',
        '-ExecutionPolicy',
        'RemoteSigned',
        '-File',
        str(path),
    ]
    try:
        try:
            result = subprocess.run(
                command,
                capture_output=True,
                text=True,
                encoding='utf-8',
                errors='replace',
                creationflags=CREATE_NO_WINDOW,
                stdin=subprocess.DEVNULL,
            )
        except FileNotFoundError:
            result = subprocess.CompletedProcess(
                command, -1, stdout='', stderr='PowerShell not found'
            )
    finally:
        try:
            path.unlink()
        except OSError:
            pass
    return result


def ps_text(value: str) -> str:
    return base64.b64encode(value.encode('utf-16le')).decode('ascii')


def message(title: str, body: str, icon: str = 'Information') -> None:
    title64 = ps_text(title)
    body64 = ps_text(body)
    ok64 = ps_text('确定')
    success = icon != 'Error'
    accent = '#34C759' if success else '#FF3B30'

    script = (
        "Add-Type -AssemblyName System.Windows.Forms;Add-Type -AssemblyName System.Drawing;$accent=[Drawing.ColorTranslator]::FromHtml('"
        + accent
        + "');$t=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
        + title64
        + "'));$b=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
        + body64
        + "'));$ok=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
        + ok64
        + "'));$wa=[System.Windows.Forms.Screen]::PrimaryScreen.WorkingArea;$DW=420;$DH=285;$FW=[Math]::Min($DW,[Math]::Max(320,$wa.Width-24));$FH=[Math]::Min($DH,[Math]::Max(240,$wa.Height-24));$f=New-Object Windows.Forms.Form;$f.Text='';$f.Size=New-Object Drawing.Size($FW,$FH);$f.StartPosition='CenterScreen';$f.FormBorderStyle='FixedDialog';$f.MaximizeBox=$false;$f.MinimizeBox=$false;$f.ShowInTaskbar=$false;$f.BackColor=[Drawing.Color]::White;$s=New-Object Windows.Forms.Panel;$s.Size=New-Object Drawing.Size(420,4);$s.Location=New-Object Drawing.Point(0,0);$s.BackColor=$accent;$f.Controls.Add($s);$l=New-Object Windows.Forms.Label;$l.Text=$t;$l.Location=New-Object Drawing.Point(32,28);$l.Size=New-Object Drawing.Size(356,24);$l.Font=New-Object Drawing.Font('Microsoft YaHei UI',13,[Drawing.FontStyle]::Bold);$l.ForeColor=[Drawing.ColorTranslator]::FromHtml('#1C1C1E');$f.Controls.Add($l);$m=New-Object Windows.Forms.Label;$m.Text=$b;$m.Location=New-Object Drawing.Point(32,60);$m.Size=New-Object Drawing.Size(356,100);$m.Font=New-Object Drawing.Font('Microsoft YaHei UI',10);$m.ForeColor=[Drawing.ColorTranslator]::FromHtml('#6E6E73');$f.Controls.Add($m);$k=New-Object Windows.Forms.Button;$k.Text=$ok;$k.Location=New-Object Drawing.Point(310,180);$k.Size=New-Object Drawing.Size(84,32);$k.Font=New-Object Drawing.Font('Microsoft YaHei UI',10);$k.FlatStyle='Flat';$k.BackColor=[Drawing.Color]::White;$k.ForeColor=[Drawing.ColorTranslator]::FromHtml('#1677FF');$k.FlatAppearance.BorderSize=0;$k.DialogResult=[Windows.Forms.DialogResult]::OK;$f.AcceptButton=$k;$f.Controls.Add($k);[void]$f.ShowDialog()"
    )
    try:
        run_powershell(script)
    except Exception:
        pass


def choose_files() -> list[Path]:
    title = ps_text('选择需要投递的文件')
    script = (
        "Add-Type -AssemblyName System.Windows.Forms;$d=New-Object Windows.Forms.OpenFileDialog;$d.Title=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
        + title
        + "'));$d.Multiselect=$true;if($d.ShowDialog() -eq [Windows.Forms.DialogResult]::OK){$d.FileNames|ForEach-Object{[Convert]::ToBase64String([Text.Encoding]::Unicode.GetBytes($_))}}"
    )
    result = run_powershell(script)
    paths = []
    for line in result.stdout.splitlines():
        if not line.strip():
            continue
        paths.append(Path(base64.b64decode(line.strip()).decode('utf-16le')))
    return paths


class DeliveryProgress:
    def __init__(self, total: int) -> None:
        self._ps_script = None
        self.path = (
            Path(tempfile.gettempdir())
            / f'secure-pigeon-{uuid.uuid4().hex}.status'
        )
        self.total = total
        self._cur = 0
        self._name = ''
        self._write('RUN', 0, '')

        path64 = ps_text(str(self.path))
        title64 = ps_text('正在安全投递')
        prefix64 = ps_text('正在写入 U 盘')
        cancel64 = ps_text('取消投递')

        script = (
            "$p=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
            + path64
            + "'));$title=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
            + title64
            + "'));$prefix=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
            + prefix64
            + "'));$cancelTxt=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
            + cancel64
            + "'));Add-Type -AssemblyName System.Windows.Forms;Add-Type -AssemblyName System.Drawing;$wa=[System.Windows.Forms.Screen]::PrimaryScreen.WorkingArea;$DW=420;$DH=230;$FW=[Math]::Min($DW,[Math]::Max(320,$wa.Width-24));$FH=[Math]::Min($DH,[Math]::Max(200,$wa.Height-24));$f=New-Object Windows.Forms.Form;$f.Text=$title;$f.Size=New-Object Drawing.Size($FW,$FH);$f.StartPosition='CenterScreen';$f.FormBorderStyle='FixedDialog';$f.ControlBox=$false;$f.BackColor=[Drawing.Color]::White;$tl=New-Object Windows.Forms.Label;$tl.Text=$title;$tl.Location=New-Object Drawing.Point(28,20);$tl.Size=New-Object Drawing.Size(364,24);$tl.Font=New-Object Drawing.Font('Microsoft YaHei UI',13,[Drawing.FontStyle]::Bold);$tl.ForeColor=[Drawing.ColorTranslator]::FromHtml('#1C1C1E');$f.Controls.Add($tl);$pct=New-Object Windows.Forms.Label;$pct.Location=New-Object Drawing.Point(28,50);$pct.Size=New-Object Drawing.Size(100,36);$pct.Font=New-Object Drawing.Font('Microsoft YaHei UI',24,[Drawing.FontStyle]::Bold);$pct.ForeColor=[Drawing.ColorTranslator]::FromHtml('#1677FF');$pct.Text='0%';$f.Controls.Add($pct);$l=New-Object Windows.Forms.Label;$l.Location=New-Object Drawing.Point(136,54);$l.Size=New-Object Drawing.Size(256,30);$l.Font=New-Object Drawing.Font('Microsoft YaHei UI',9);$l.ForeColor=[Drawing.ColorTranslator]::FromHtml('#6E6E73');$f.Controls.Add($l);$track=New-Object Windows.Forms.Panel;$track.Size=New-Object Drawing.Size(364,8);$track.Location=New-Object Drawing.Point(28,96);$track.BackColor=[Drawing.ColorTranslator]::FromHtml('#F0F0F0');$f.Controls.Add($track);$fill=New-Object Windows.Forms.Label;$fill.Size=New-Object Drawing.Size(0,8);$fill.Location=New-Object Drawing.Point(0,0);$fill.BackColor=[Drawing.ColorTranslator]::FromHtml('#1677FF');$track.Controls.Add($fill);$c=New-Object Windows.Forms.Label;$c.Location=New-Object Drawing.Point(28,110);$c.Size=New-Object Drawing.Size(364,18);$c.Font=New-Object Drawing.Font('Microsoft YaHei UI',9);$c.ForeColor=[Drawing.ColorTranslator]::FromHtml('#AEAEB2');$f.Controls.Add($c);$cancel=New-Object System.Windows.Forms.Button;$cancel.Text=$cancelTxt;$cancel.Location=New-Object Drawing.Point(290,142);$cancel.Size=New-Object Drawing.Size(102,32);$cancel.FlatStyle='Flat';$cancel.BackColor=[Drawing.Color]::White;$cancel.ForeColor=[Drawing.ColorTranslator]::FromHtml('#FF3B30');$cancel.Font=New-Object Drawing.Font('Microsoft YaHei UI',9);$cancel.FlatAppearance.BorderColor=[Drawing.ColorTranslator]::FromHtml('#FF3B30');$cancel.FlatAppearance.BorderSize=1;$f.Controls.Add($cancel);$t=New-Object Windows.Forms.Timer;$t.Interval=180;$t.Add_Tick({if(Test-Path -LiteralPath $p){$v=([IO.File]::ReadAllText($p)).Split([char]9);if($v[0] -eq 'CANCEL'){$t.Stop();$f.Close()}elseif($v.Length -ge 6){$max=[int]$v[2];$cur=[Math]::Min([int]$v[1],$max);$fDone=[int64]$v[4];$fTotal=[int64]$v[5];$fileRatio=$(if($fTotal -gt 0){$fDone/$fTotal}else{0});$ratio=$(if($max -gt 0){[Math]::Min(1.0,[Math]::Max(0.0,($cur+$fileRatio)/$max))}else{0});$fillW=[Math]::Floor(364*$ratio);$fill.Size=New-Object Drawing.Size($fillW,8);$pctTxt=[Math]::Floor($ratio*100);$pct.Text=($pctTxt.ToString()+'%');$name=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String($v[3]));$l.Text=$name;$c.Text=($cur.ToString()+' / '+$max.ToString()+' 个文件');if($v[0] -ne 'RUN'){$t.Stop();$f.Close()}}}});$cancel.Add_Click({Set-Content -LiteralPath $p -Value 'CANCEL' -Encoding UTF8;$cancel.Enabled=$false;$cancel.Text=$cancelTxt;});$f.Add_Shown({$t.Start()});[void]$f.ShowDialog()"
        )

        self._ps_script = _ps_temp_file(script)
        self.process = subprocess.Popen(
            [
                'powershell.exe',
                '-NoProfile',
                '-STA',
                '-ExecutionPolicy',
                'RemoteSigned',
                '-File',
                str(self._ps_script),
            ],
            creationflags=CREATE_NO_WINDOW,
        )

    def _write(
        self,
        state: str,
        current: int,
        name: str,
        file_done: int = 0,
        file_total: int = 0,
    ) -> None:
        encoded_name = base64.b64encode(name.encode('utf-16le')).decode('ascii')
        self.path.write_text(
            f'{state}\t{current}\t{self.total}\t{encoded_name}\t{file_done}\t{file_total}',
            encoding='utf-8',
        )

    def update(self, current: int, name: str) -> None:
        self._cur = current
        self._name = name
        self._write('RUN', current, name)

    def update_file(self, done: int, total: int) -> None:
        """文件内分块进度回调（由 append_file 调用）。"""
        self._write('RUN', self._cur, self._name, done, total)

    def cancelled(self) -> bool:
        """用户是否点了取消投递。"""
        try:
            state = self.path.read_text(encoding='utf-8').split('\t')[0]
        except OSError:
            return False
        return state == 'CANCEL'

    def close(self, failed: bool = False) -> None:
        self._write('ERROR' if failed else 'DONE', self.total, '')
        try:
            self.process.wait(timeout=3)
        except subprocess.TimeoutExpired:
            self.process.kill()
            self.process.wait()
        except OSError:
            pass
        try:
            self.path.unlink()
        except OSError:
            pass
        if self._ps_script is not None:
            try:
                self._ps_script.unlink()
            except OSError:
                pass


def prompt_activation(device_id: str) -> str | None:
    """Show an activation dialog with the device ID. Returns the entered code."""
    title64 = ps_text('激活 U 盘')
    hint64 = ps_text('试用期已过，请输入激活码。')
    dev64 = ps_text('设备 ID：' + device_id)
    fmt64 = ps_text('XXXX-XXXX-XXXX-XXXX')
    ok64 = ps_text('激活')
    cancel64 = ps_text('取消')

    script = (
        "$t=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
        + title64
        + "'));$hint=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
        + hint64
        + "'));$dev=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
        + dev64
        + "'));$fmt=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
        + fmt64
        + "'));$ok=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
        + ok64
        + "'));$cancel=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
        + cancel64
        + "'));Add-Type -AssemblyName System.Windows.Forms;Add-Type -AssemblyName System.Drawing;$wa=[System.Windows.Forms.Screen]::PrimaryScreen.WorkingArea;$DW=420;$DH=265;$FW=[Math]::Min($DW,[Math]::Max(320,$wa.Width-24));$FH=[Math]::Min($DH,[Math]::Max(240,$wa.Height-24));$f=New-Object Windows.Forms.Form;$f.Text='';$f.Size=New-Object Drawing.Size($FW,$FH);$f.StartPosition='CenterScreen';$f.FormBorderStyle='FixedDialog';$f.MaximizeBox=$false;$f.MinimizeBox=$false;$f.BackColor=[Drawing.Color]::White;$s=New-Object Windows.Forms.Panel;$s.Size=New-Object Drawing.Size(420,4);$s.Location=New-Object Drawing.Point(0,0);$s.BackColor=[Drawing.ColorTranslator]::FromHtml('#FF3B30');$f.Controls.Add($s);$l=New-Object Windows.Forms.Label;$l.Text=$t;$l.Location=New-Object Drawing.Point(32,28);$l.Size=New-Object Drawing.Size(356,24);$l.Font=New-Object Drawing.Font('Microsoft YaHei UI',13,[Drawing.FontStyle]::Bold);$l.ForeColor=[Drawing.ColorTranslator]::FromHtml('#1C1C1E');$f.Controls.Add($l);$h=New-Object Windows.Forms.Label;$h.Text=$hint;$h.Location=New-Object Drawing.Point(32,56);$h.Size=New-Object Drawing.Size(356,20);$h.Font=New-Object Drawing.Font('Microsoft YaHei UI',9);$h.ForeColor=[Drawing.ColorTranslator]::FromHtml('#6E6E73');$f.Controls.Add($h);$d=New-Object Windows.Forms.Label;$d.Text=$dev;$d.Location=New-Object Drawing.Point(32,78);$d.Size=New-Object Drawing.Size(356,20);$d.Font=New-Object Drawing.Font('Microsoft YaHei UI',10);$d.ForeColor=[Drawing.ColorTranslator]::FromHtml('#1C1C1E');$f.Controls.Add($d);$x=New-Object Windows.Forms.TextBox;$x.Location=New-Object Drawing.Point(32,108);$x.Size=New-Object Drawing.Size(356,32);$x.Font=New-Object Drawing.Font('Microsoft YaHei UI',14);$x.TextAlign='Center';$x.MaxLength=19;$x.Text=$fmt;$x.ForeColor=[Drawing.ColorTranslator]::FromHtml('#AEAEB2');$f.Controls.Add($x);$x.Add_GotFocus({if($x.Text -eq $fmt){$x.Text='';$x.ForeColor=[Drawing.ColorTranslator]::FromHtml('#1C1C1E')}});$x.Add_LostFocus({if([string]::IsNullOrWhiteSpace($x.Text)){$x.Text=$fmt;$x.ForeColor=[Drawing.ColorTranslator]::FromHtml('#AEAEB2')}});$b=New-Object Windows.Forms.Button;$b.Text=$ok;$b.Location=New-Object Drawing.Point(314,158);$b.Size=New-Object Drawing.Size(80,32);$b.FlatStyle='Flat';$b.BackColor=[Drawing.ColorTranslator]::FromHtml('#1677FF');$b.ForeColor=[Drawing.Color]::White;$b.Font=New-Object Drawing.Font('Microsoft YaHei UI',10);$b.FlatAppearance.BorderSize=0;$f.AcceptButton=$b;$f.Controls.Add($b);$b.Add_Click({$f.Tag=[Convert]::ToBase64String([Text.Encoding]::Unicode.GetBytes($x.Text));$f.Close();});$c=New-Object Windows.Forms.Button;$c.Text=$cancel;$c.Location=New-Object Drawing.Point(226,158);$c.Size=New-Object Drawing.Size(80,32);$c.FlatStyle='Flat';$c.BackColor=[Drawing.Color]::White;$c.ForeColor=[Drawing.ColorTranslator]::FromHtml('#6E6E73');$c.Font=New-Object Drawing.Font('Microsoft YaHei UI',10);$c.FlatAppearance.BorderSize=0;$f.Controls.Add($c);$c.Add_Click({$f.Tag='';$f.Close();});[void]$f.ShowDialog();if($f.Tag){Write-Output $f.Tag}"
    )

    result = run_powershell(script)
    line = result.stdout.strip()
    if not line:
        return None
    code = base64.b64decode(line).decode('utf-16le').strip()
    if code and code != 'XXXX-XXXX-XXXX-XXXX':
        return code
    return None


def prompt_trial_or_activate() -> str | None:
    """First-launch dialog: choose the free 7-day trial or activate now.

    Returns ``"trial"``, ``"activate"``, or ``None`` if the user cancels.
    """
    title64 = ps_text('选择激活方式')
    hint64 = ps_text('欢迎使用安全信鸽，请选择激活方式：')
    trial64 = ps_text('免费试用 7 天')
    act64 = ps_text('立即激活永久')
    cancel64 = ps_text('取消')

    script = (
        "$t=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
        + title64
        + "'));$hint=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
        + hint64
        + "'));$trial=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
        + trial64
        + "'));$act=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
        + act64
        + "'));$cancel=[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String('"
        + cancel64
        + "'));Add-Type -AssemblyName System.Windows.Forms;Add-Type -AssemblyName System.Drawing;$wa=[System.Windows.Forms.Screen]::PrimaryScreen.WorkingArea;$DW=420;$DH=250;$FW=[Math]::Min($DW,[Math]::Max(320,$wa.Width-24));$FH=[Math]::Min($DH,[Math]::Max(240,$wa.Height-24));$f=New-Object System.Windows.Forms.Form;$f.Text='';$f.Size=New-Object Drawing.Size($FW,$FH);$f.StartPosition='CenterScreen';$f.FormBorderStyle='FixedDialog';$f.MaximizeBox=$false;$f.MinimizeBox=$false;$f.BackColor=[Drawing.Color]::White;$s=New-Object System.Windows.Forms.Panel;$s.Size=New-Object Drawing.Size(420,4);$s.Location=New-Object Drawing.Point(0,0);$s.BackColor=[Drawing.ColorTranslator]::FromHtml('#1677FF');$f.Controls.Add($s);$l=New-Object System.Windows.Forms.Label;$l.Text=$t;$l.Location=New-Object Drawing.Point(32,28);$l.Size=New-Object Drawing.Size(356,24);$l.Font=New-Object Drawing.Font('Microsoft YaHei UI',13,[Drawing.FontStyle]::Bold);$l.ForeColor=[Drawing.ColorTranslator]::FromHtml('#1C1C1E');$f.Controls.Add($l);$h=New-Object System.Windows.Forms.Label;$h.Text=$hint;$h.Location=New-Object Drawing.Point(32,60);$h.Size=New-Object Drawing.Size(356,36);$h.Font=New-Object Drawing.Font('Microsoft YaHei UI',10);$h.ForeColor=[Drawing.ColorTranslator]::FromHtml('#6E6E73');$f.Controls.Add($h);$b1=New-Object System.Windows.Forms.Button;$b1.Text=$trial;$b1.Location=New-Object Drawing.Point(32,108);$b1.Size=New-Object Drawing.Size(168,38);$b1.FlatStyle='Flat';$b1.BackColor=[Drawing.ColorTranslator]::FromHtml('#1677FF');$b1.ForeColor=[Drawing.Color]::White;$b1.Font=New-Object Drawing.Font('Microsoft YaHei UI',10);$b1.FlatAppearance.BorderSize=0;$f.Controls.Add($b1);$b1.Add_Click({$f.Tag='trial';$f.Close()});$b2=New-Object System.Windows.Forms.Button;$b2.Text=$act;$b2.Location=New-Object Drawing.Point(220,108);$b2.Size=New-Object Drawing.Size(168,38);$b2.FlatStyle='Flat';$b2.BackColor=[Drawing.Color]::White;$b2.ForeColor=[Drawing.ColorTranslator]::FromHtml('#1677FF');$b2.Font=New-Object Drawing.Font('Microsoft YaHei UI',10);$b2.FlatAppearance.BorderColor=[Drawing.ColorTranslator]::FromHtml('#1677FF');$b2.FlatAppearance.BorderSize=1;$f.Controls.Add($b2);$b2.Add_Click({$f.Tag='activate';$f.Close()});$c=New-Object System.Windows.Forms.Button;$c.Text=$cancel;$c.Location=New-Object Drawing.Point(32,158);$c.Size=New-Object Drawing.Size(356,30);$c.FlatStyle='Flat';$c.BackColor=[Drawing.Color]::White;$c.ForeColor=[Drawing.ColorTranslator]::FromHtml('#6E6E73');$c.Font=New-Object Drawing.Font('Microsoft YaHei UI',9);$c.FlatAppearance.BorderSize=0;$f.Controls.Add($c);$c.Add_Click({$f.Tag='';$f.Close();});[void]$f.ShowDialog();if($f.Tag){Write-Output $f.Tag}"
    )

    result = run_powershell(script)
    line = result.stdout.strip()
    if line in {'trial', 'activate'}:
        return line
    return None


def _ensure_powershell() -> bool:
    """PowerShell 缺失时弹原生对话框提示并返回 False（同 retrieve.py）。"""
    import shutil

    if shutil.which('powershell.exe'):
        return True
    try:
        import ctypes as _c

        _c.windll.user32.MessageBoxW(
            0,
            '未找到 PowerShell（powershell.exe），无法显示界面。\n请修复系统组件后重试。',
            '安全信鸽 · 收件箱',
            0x10,
        )
    except Exception:
        pass
    return False


def main() -> int:
    if not _ensure_powershell():
        return 1
    try:
        if is_activated(USB_ROOT):
            pass
        elif not trial_started(USB_ROOT):
            choice = prompt_trial_or_activate()
            if choice == 'trial':
                start_trial(USB_ROOT)
                message('试用已开始', '免费试用已开始，续期 7 天，到期后需激活。')
                return 0
            elif choice == 'activate':
                device_id = get_device_id(USB_ROOT)
                code = prompt_activation(device_id)
                if code is None:
                    return 0
                if not verify_and_save(USB_ROOT, code):
                    message('激活失败', '激活码不正确，请核对后重新输入。', 'Error')
                    return 1
                message('激活成功', '已永久激活，感谢使用！')
            else:
                return 0
        elif days_remaining(USB_ROOT) > 0:
            pass
        else:
            device_id = get_device_id(USB_ROOT)
            message(
                '试用已过期',
                '试用期 7 天已结束，请联系作者获取激活码。\n\n设备 ID：' + device_id,
                'Error',
            )
            code = prompt_activation(device_id)
            if code is None:
                return 0
            if not verify_and_save(USB_ROOT, code):
                message('激活失败', '激活码不正确，请核对后重新输入。', 'Error')
                return 1
            message('激活成功', '已永久激活，感谢使用！')
    except OSError as exc:
        message('无法投递', '无法读取激活信息。\n\n' + str(exc), 'Error')
        return 1

    from pigeon_store import (
        MailboxError,
        append_file,
        fingerprint,
        inspect_store,
        load_public_key,
    )

    try:
        author_key = load_public_key(AUTHOR_PUBLIC)
        temporary_key = load_public_key(TEMPORARY_PUBLIC)
        # 公钥替换检测：PLF3 激活凭据里钉有部署时两把公钥的指纹，
        # 盘上公钥被替换（投递内容会被重定向到攻击者）时在此拒绝。
        verify_public_keys(USB_ROOT, fingerprint(author_key), fingerprint(temporary_key))
        inspect_store(STORE, author_key, temporary_key)
    except MailboxError as exc:
        message('无法投递', str(exc), 'Error')
        return 1
    except OSError as exc:
        message('无法投递', '无法读取 U 盘数据，请确认 U 盘已正确连接。\n\n' + str(exc), 'Error')
        return 1
    except Exception as exc:
        message('无法投递', '发生未预期错误。\n\n' + str(exc), 'Error')
        return 1

    try:
        files = choose_files()
    except Exception:
        message('无法读取文件', '请重新选择需要投递的文件。', 'Error')
        return 1
    if not files:
        return 0

    progress = DeliveryProgress(len(files))
    used_hint = None
    try:
        for index, path in enumerate(files, start=1):
            if progress.cancelled():
                progress.close()
                message('投递已取消', '已停止投递，已写入的文件保留在 U 盘中。')
                return 0
            progress.update(index - 1, path.name)
            _total, _free, new_used = append_file(
                STORE,
                author_key,
                temporary_key,
                path,
                progress.update_file,
                used_hint=used_hint,
            )
            used_hint = new_used
        progress.close()
        message('投递完成', '文件已存入 U 盘。')
        return 0
    except MailboxError as exc:
        progress.close(failed=True)
        message('投递失败', str(exc), 'Error')
        return 1
    except Exception:
        progress.close(failed=True)
        message('投递失败', '操作未能完成，请勿拔出 U 盘，联系作者处理。', 'Error')
        return 1


if __name__ == '__main__':
    try:
        raise SystemExit(main())
    except SystemExit:
        raise
    except Exception as exc:
        try:
            message('投递失败', '发生未预期错误。\n\n' + str(exc), 'Error')
        except Exception:
            import ctypes as _ctypes

            try:
                _ctypes.windll.user32.MessageBoxW(0, str(exc), '安全信鸽 · 投递失败', 16)
            except Exception:
                pass
        raise SystemExit(1)
