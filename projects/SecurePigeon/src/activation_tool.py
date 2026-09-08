"""赋码工具 — 为安全信鸽 U 盘生成激活码。

两种模式：

命令行模式（脚本/批处理使用）::

    activation_tool.exe --device-id ABCD1234 --order ORD-001 --master-key <HEX> [--output orders.csv] [--note ...]

GUI 模式（双击运行）：弹出图形表单，输入设备 ID 与授权密钥（支持从部署时
生成的授权密钥文件一键导入）后生成激活码，可一键复制并选填记录到 CSV。
"""

from __future__ import annotations

import argparse
import base64
import csv
import subprocess
import sys
from datetime import datetime
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from activation import generate_activation_code
# subprocess.CREATE_NO_WINDOW (0x08000000)
NO_WINDOW = 134217728

# The GUI is a PowerShell WinForms script.  It is written next to the tool
# as a kept product file and launched with ``-File``, so the UI code stays
# inspectable on disk and no %TEMP% litter is created or left behind.
#
# Chinese UI text is passed to the script as UTF-16LE base64 blobs which
# the script decodes at runtime (helper S), keeping the quoting safe; the
# script file itself is written as UTF-8 with BOM for Windows PowerShell.
PS_UI_SCRIPT_NAME = "pigeon_ui.ps1"


def _ps_temp_file(script: str) -> str:
    """Write *script* to the activation tool's own folder as a product ``.ps1``.

    This replaces the previous %TEMP% + random-name + unlink pattern.  Using
    ``-File`` instead of ``-EncodedCommand`` keeps the payload inspectable
    on disk and avoids the command-line length limit; the UTF-8 BOM makes
    Windows PowerShell 5.1 read the file as UTF-8 so the Chinese UI text
    survives any ANSI code page.  The file is a kept product artifact: it
    is simply rewritten in-place on each run, never deleted.
    """
    directory = Path(sys.argv[0]).resolve().parent
    directory.mkdir(parents=True, exist_ok=True)
    path = directory / PS_UI_SCRIPT_NAME
    with open(path, "wb") as handle:
        handle.write(b"\xef\xbb\xbf")
        handle.write(script.encode("utf-8"))
    return str(path)


def ps(script: str) -> subprocess.CompletedProcess[str]:
    """Run a PowerShell script (UTF-8-BOM product file, ``-File``) and wait.

    ``pigeon_ui.ps1`` is a kept product file in the activation tool's folder
    and is rewritten in-place on each run; it is executed synchronously.
    """
    path = _ps_temp_file(script)
    command = ["powershell.exe", "-NoProfile", "-STA", "-ExecutionPolicy", "RemoteSigned",
               "-File", path]
    try:
        try:
            return subprocess.run(command, capture_output=True, text=True, encoding="utf-8",
                                  errors="replace", creationflags=NO_WINDOW, stdin=subprocess.DEVNULL)
        except FileNotFoundError:
            return subprocess.CompletedProcess(command, -1, stdout="", stderr="PowerShell not found")
    finally:
        pass


def field(value: str) -> str:
    return base64.b64encode(value.encode("utf-16le")).decode("ascii")


def gui_collect() -> dict | None:
    """弹出赋码表单收集输入，返回 dict；取消返回 ``None``。"""
    values = {
        "@T1@": field("安全信鸽 · 赋码工具"),
        "@T2@": field("为安全信鸽 U 盘生成激活码。可点「导入密钥文件」读取部署时保存的授权密钥文件。"),
        "@L1@": field("设备 ID"),
        "@H1@": field("买家收件箱过期时屏幕上显示的 8 位十六进制设备 ID"),
        "@L2@": field("授权密钥 (HEX)"),
        "@H2@": field("部署时保存的 64 位授权密钥，或点击按钮从密钥文件导入"),
        "@L3@": field("订单号（选填）"),
        "@L4@": field("备注（选填）"),
        "@L5@": field("记录文件（选填）"),
        "@H5@": field("填入路径则生成后追加写入 CSV 记录"),
        "@IMPORT@": field("导入密钥文件"),
        "@BROWSE@": field("浏览"),
        "@GEN@": field("生成激活码"),
        "@NEED@": field("请填写设备 ID 和授权密钥。"),
    }
    script = r"""
Add-Type -AssemblyName System.Windows.Forms;Add-Type -AssemblyName System.Drawing
$bd=[Drawing.ColorTranslator]::FromHtml('#1677FF');$bg=[Drawing.Color]::White;$t1c=[Drawing.ColorTranslator]::FromHtml('#1C1C1E');$t2c=[Drawing.ColorTranslator]::FromHtml('#6E6E73');$fn='Microsoft YaHei UI'
function S([string]$b){[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String($b))}
$wa=[System.Windows.Forms.Screen]::PrimaryScreen.WorkingArea;$DW=600;$DH=475;$FW=[Math]::Min($DW,[Math]::Max(320,$wa.Width-24));$FH=[Math]::Min($DH,[Math]::Max(240,$wa.Height-24));$f=New-Object System.Windows.Forms.Form;$f.Text='安全信鸽 · 赋码工具';$f.Size=New-Object Drawing.Size($FW,$FH);$f.StartPosition='CenterScreen';$f.BackColor=$bg;$f.FormBorderStyle='FixedDialog';$f.MaximizeBox=$false
$tl=New-Object System.Windows.Forms.Label;$tl.Text=(S '@T1@');$tl.Location=New-Object Drawing.Point(32,20);$tl.Size=New-Object Drawing.Size(536,28);$tl.Font=New-Object Drawing.Font($fn,15,[Drawing.FontStyle]::Bold);$tl.ForeColor=$t1c;$f.Controls.Add($tl)
$st=New-Object System.Windows.Forms.Label;$st.Text=(S '@T2@');$st.Location=New-Object Drawing.Point(32,52);$st.Size=New-Object Drawing.Size(536,40);$st.Font=New-Object Drawing.Font($fn,10);$st.ForeColor=$t2c;$f.Controls.Add($st)
# 设备 ID
$dl=New-Object System.Windows.Forms.Label;$dl.Text=(S '@L1@');$dl.Location=New-Object Drawing.Point(32,104);$dl.Size=New-Object Drawing.Size(120,20);$dl.Font=New-Object Drawing.Font($fn,10,[Drawing.FontStyle]::Bold);$f.Controls.Add($dl)
$devBox=New-Object System.Windows.Forms.TextBox;$devBox.Location=New-Object Drawing.Point(160,100);$devBox.Size=New-Object Drawing.Size(200,28);$devBox.Font=New-Object Drawing.Font($fn,12);$devBox.MaxLength=8;$f.Controls.Add($devBox)
$imp=New-Object System.Windows.Forms.Button;$imp.Text=(S '@IMPORT@');$imp.Location=New-Object Drawing.Point(372,99);$imp.Size=New-Object Drawing.Size(170,30);$imp.Font=New-Object Drawing.Font($fn,9);$imp.FlatStyle='Flat';$imp.BackColor=$bg;$imp.ForeColor=$bd;$imp.FlatAppearance.BorderColor=$bd;$imp.FlatAppearance.BorderSize=1;$f.Controls.Add($imp)
$h=New-Object System.Windows.Forms.Label;$h.Text=(S '@H1@');$h.Location=New-Object Drawing.Point(32,128);$h.Size=New-Object Drawing.Size(536,18);$h.ForeColor=$t2c;$h.Font=New-Object Drawing.Font($fn,8);$f.Controls.Add($h)
# 授权密钥
$kl=New-Object System.Windows.Forms.Label;$kl.Text=(S '@L2@');$kl.Location=New-Object Drawing.Point(32,162);$kl.Size=New-Object Drawing.Size(120,20);$kl.Font=New-Object Drawing.Font($fn,10,[Drawing.FontStyle]::Bold);$f.Controls.Add($kl)
$keyBox=New-Object System.Windows.Forms.TextBox;$keyBox.Location=New-Object Drawing.Point(160,158);$keyBox.Size=New-Object Drawing.Size(382,28);$keyBox.Font=New-Object Drawing.Font('Consolas',11);$f.Controls.Add($keyBox)
$h=New-Object System.Windows.Forms.Label;$h.Text=(S '@H2@');$h.Location=New-Object Drawing.Point(32,190);$h.Size=New-Object Drawing.Size(536,18);$h.ForeColor=$t2c;$h.Font=New-Object Drawing.Font($fn,8);$f.Controls.Add($h)
# 订单号
$ol=New-Object System.Windows.Forms.Label;$ol.Text=(S '@L3@');$ol.Location=New-Object Drawing.Point(32,222);$ol.Size=New-Object Drawing.Size(120,20);$ol.Font=New-Object Drawing.Font($fn,10);$f.Controls.Add($ol)
$orderBox=New-Object System.Windows.Forms.TextBox;$orderBox.Location=New-Object Drawing.Point(160,218);$orderBox.Size=New-Object Drawing.Size(382,28);$orderBox.Font=New-Object Drawing.Font($fn,10);$f.Controls.Add($orderBox)
# 备注
$nl=New-Object System.Windows.Forms.Label;$nl.Text=(S '@L4@');$nl.Location=New-Object Drawing.Point(32,256);$nl.Size=New-Object Drawing.Size(120,20);$nl.Font=New-Object Drawing.Font($fn,10);$f.Controls.Add($nl)
$noteBox=New-Object System.Windows.Forms.TextBox;$noteBox.Location=New-Object Drawing.Point(160,252);$noteBox.Size=New-Object Drawing.Size(382,28);$noteBox.Font=New-Object Drawing.Font($fn,10);$f.Controls.Add($noteBox)
# 记录文件
$cl=New-Object System.Windows.Forms.Label;$cl.Text=(S '@L5@');$cl.Location=New-Object Drawing.Point(32,290);$cl.Size=New-Object Drawing.Size(120,20);$cl.Font=New-Object Drawing.Font($fn,10);$f.Controls.Add($cl)
$csvBox=New-Object System.Windows.Forms.TextBox;$csvBox.Location=New-Object Drawing.Point(160,286);$csvBox.Size=New-Object Drawing.Size(300,28);$csvBox.Font=New-Object Drawing.Font($fn,10);$f.Controls.Add($csvBox)
$br=New-Object System.Windows.Forms.Button;$br.Text=(S '@BROWSE@');$br.Location=New-Object Drawing.Point(468,285);$br.Size=New-Object Drawing.Size(74,30);$br.Font=New-Object Drawing.Font($fn,9);$br.FlatStyle='Flat';$br.BackColor=$bg;$br.ForeColor=$bd;$br.FlatAppearance.BorderColor=$bd;$br.FlatAppearance.BorderSize=1;$f.Controls.Add($br)
$h=New-Object System.Windows.Forms.Label;$h.Text=(S '@H5@');$h.Location=New-Object Drawing.Point(32,318);$h.Size=New-Object Drawing.Size(536,18);$h.ForeColor=$t2c;$h.Font=New-Object Drawing.Font($fn,8);$f.Controls.Add($h)
# 导入密钥文件：解析部署时生成的「激活主密钥-设备ID.txt」
$imp.Add_Click({
$d=New-Object System.Windows.Forms.OpenFileDialog;$d.Filter='授权密钥文件 (*.txt)|*.txt|所有文件|*.*';$d.Title='选择部署时生成的授权密钥文件'
if($d.ShowDialog() -eq 'OK'){
$lines=Get-Content -LiteralPath $d.FileName -Encoding UTF8
foreach($ln in $lines){
if($ln -match '设备 ID：\s*([0-9A-Fa-f]{8})'){$devBox.Text=$Matches[1]}
if($ln -match '授权密钥 \(HEX\)：\s*([0-9A-Fa-f]{64})'){$keyBox.Text=$Matches[1]}
}
}
})
# 浏览记录文件
$br.Add_Click({$d=New-Object System.Windows.Forms.SaveFileDialog;$d.Filter='CSV 文件 (*.csv)|*.csv';if($d.ShowDialog() -eq 'OK'){$csvBox.Text=$d.FileName}})
# 生成
$go=New-Object System.Windows.Forms.Button;$go.Text=(S '@GEN@');$go.Location=New-Object Drawing.Point(32,350);$go.Size=New-Object Drawing.Size(536,40);$go.FlatStyle='Flat';$go.BackColor=$bd;$go.ForeColor=$bg;$go.Font=New-Object Drawing.Font($fn,11,[Drawing.FontStyle]::Bold);$go.FlatAppearance.BorderSize=0;$f.AcceptButton=$go;$f.Controls.Add($go)
$go.Add_Click({
$dev=$devBox.Text.Trim();$key=$keyBox.Text.Trim()
if($dev -eq '' -or $key -eq ''){[Windows.Forms.MessageBox]::Show((S '@NEED@'),'安全信鸽 · 赋码工具')|Out-Null;return}
$f.Tag='OK|'+[Convert]::ToBase64String([Text.Encoding]::Unicode.GetBytes($dev))+'|'+[Convert]::ToBase64String([Text.Encoding]::Unicode.GetBytes($key))+'|'+[Convert]::ToBase64String([Text.Encoding]::Unicode.GetBytes($orderBox.Text.Trim()))+'|'+[Convert]::ToBase64String([Text.Encoding]::Unicode.GetBytes($noteBox.Text.Trim()))+'|'+[Convert]::ToBase64String([Text.Encoding]::Unicode.GetBytes($csvBox.Text.Trim()))
$f.Close()
})
[void]$f.ShowDialog();if($f.Tag){Write-Output $f.Tag}
"""
    for placeholder, value in values.items():
        script = script.replace(placeholder, value)
    result = ps(script)
    for line in result.stdout.splitlines():
        line = line.strip()
        if not line.startswith("OK|"): continue
        parts = line.split("|")
        if len(parts) < 6:
            continue
        try:
            values_decoded = [base64.b64decode(v).decode("utf-16le") for v in parts[1:6]]
        except (ValueError, UnicodeDecodeError):
            continue
        return {
            "device_id": values_decoded[0],
            "master_key": values_decoded[1],
            "order": values_decoded[2],
            "note": values_decoded[3],
            "output": values_decoded[4],
        }
    return None


def gui_show_result(device_id: str, code: str, order: str, note: str, output: str) -> None:
    body = f"设备 ID：{device_id}\n激活码：  {code}"
    if order:
        body += f"\n订单号：  {order}"
    if note:
        body += f"\n备注：    {note}"
    if output:
        body += f"\n\n已记录到：\n{output}"
    values = {
        "@R_T@": field("安全信鸽 · 激活码已生成"),
        "@R_B@": field(body),
        "@COPY@": field("复制激活码"),
        "@OK@": field("确定"),
        "@CODE@": field(code),
        "@COPIED@": field("已复制到剪贴板 ✓"),
    }
    script = """
Add-Type -AssemblyName System.Windows.Forms;Add-Type -AssemblyName System.Drawing
$accent=[Drawing.ColorTranslator]::FromHtml('#34C759')
function S([string]$b){[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String($b))}
$wa=[System.Windows.Forms.Screen]::PrimaryScreen.WorkingArea;$DW=440;$DH=345;$FW=[Math]::Min($DW,[Math]::Max(320,$wa.Width-24));$FH=[Math]::Min($DH,[Math]::Max(240,$wa.Height-24));$f=New-Object System.Windows.Forms.Form;$f.Text='';$f.Size=New-Object Drawing.Size($FW,$FH);$f.StartPosition='CenterScreen';$f.FormBorderStyle='FixedDialog';$f.MaximizeBox=$false;$f.MinimizeBox=$false;$f.BackColor=[Drawing.Color]::White
$s=New-Object System.Windows.Forms.Panel;$s.Size=New-Object Drawing.Size(440,4);$s.Location=New-Object Drawing.Point(0,0);$s.BackColor=$accent;$f.Controls.Add($s)
$l=New-Object System.Windows.Forms.Label;$l.Text=(S '@R_T@');$l.Location=New-Object Drawing.Point(32,28);$l.Size=New-Object Drawing.Size(376,24);$l.Font=New-Object Drawing.Font('Microsoft YaHei UI',13,[Drawing.FontStyle]::Bold);$l.ForeColor=[Drawing.ColorTranslator]::FromHtml('#1C1C1E');$f.Controls.Add($l)
$m=New-Object System.Windows.Forms.Label;$m.Text=(S '@R_B@');$m.Location=New-Object Drawing.Point(32,60);$m.Size=New-Object Drawing.Size(376,120);$m.Font=New-Object Drawing.Font('Microsoft YaHei UI',11);$m.ForeColor=[Drawing.ColorTranslator]::FromHtml('#1C1C1E');$f.Controls.Add($m)
$cp=New-Object System.Windows.Forms.Button;$cp.Text=(S '@COPY@');$cp.Location=New-Object Drawing.Point(32,200);$cp.Size=New-Object Drawing.Size(180,36);$cp.FlatStyle='Flat';$cp.BackColor=[Drawing.ColorTranslator]::FromHtml('#1677FF');$cp.ForeColor=[Drawing.Color]::White;$cp.Font=New-Object Drawing.Font('Microsoft YaHei UI',10);$cp.FlatAppearance.BorderSize=0;$f.Controls.Add($cp)
$stt=New-Object System.Windows.Forms.Label;$stt.Location=New-Object Drawing.Point(224,209);$stt.Size=New-Object Drawing.Size(184,20);$stt.ForeColor=[Drawing.ColorTranslator]::FromHtml('#6E6E73');$stt.Font=New-Object Drawing.Font('Microsoft YaHei UI',9);$f.Controls.Add($stt)
$cp.Add_Click({[System.Windows.Forms.Clipboard]::SetText((S '@CODE@'));$stt.Text=(S '@COPIED@')})
$ok=New-Object System.Windows.Forms.Button;$ok.Text=(S '@OK@');$ok.Location=New-Object Drawing.Point(316,244);$ok.Size=New-Object Drawing.Size(96,32);$ok.FlatStyle='Flat';$ok.BackColor=[Drawing.Color]::White;$ok.ForeColor=[Drawing.ColorTranslator]::FromHtml('#1677FF');$ok.FlatAppearance.BorderColor=[Drawing.ColorTranslator]::FromHtml('#1677FF');$ok.FlatAppearance.BorderSize=1;$ok.DialogResult=[Windows.Forms.DialogResult]::OK;$f.AcceptButton=$ok;$f.Controls.Add($ok)
[void]$f.ShowDialog()
"""
    for placeholder, value in values.items():
        script = script.replace(placeholder, value)
    ps(script)


def gui_error(message: str) -> None:
    values = {"@E_T@": field("安全信鸽 · 赋码工具"), "@E_B@": field(message), "@OK@": field("确定")}
    script = """
Add-Type -AssemblyName System.Windows.Forms;Add-Type -AssemblyName System.Drawing
function S([string]$b){[Text.Encoding]::Unicode.GetString([Convert]::FromBase64String($b))}
$wa=[System.Windows.Forms.Screen]::PrimaryScreen.WorkingArea;$DW=420;$DH=255;$FW=[Math]::Min($DW,[Math]::Max(320,$wa.Width-24));$FH=[Math]::Min($DH,[Math]::Max(240,$wa.Height-24));$f=New-Object System.Windows.Forms.Form;$f.Text='';$f.Size=New-Object Drawing.Size($FW,$FH);$f.StartPosition='CenterScreen';$f.FormBorderStyle='FixedDialog';$f.MaximizeBox=$false;$f.MinimizeBox=$false;$f.BackColor=[Drawing.Color]::White
$s=New-Object System.Windows.Forms.Panel;$s.Size=New-Object Drawing.Size(420,4);$s.Location=New-Object Drawing.Point(0,0);$s.BackColor=[Drawing.ColorTranslator]::FromHtml('#FF3B30');$f.Controls.Add($s)
$l=New-Object System.Windows.Forms.Label;$l.Text=(S '@E_T@');$l.Location=New-Object Drawing.Point(32,28);$l.Size=New-Object Drawing.Size(356,24);$l.Font=New-Object Drawing.Font('Microsoft YaHei UI',13,[Drawing.FontStyle]::Bold);$l.ForeColor=[Drawing.ColorTranslator]::FromHtml('#1C1C1E');$f.Controls.Add($l)
$m=New-Object System.Windows.Forms.Label;$m.Text=(S '@E_B@');$m.Location=New-Object Drawing.Point(32,60);$m.Size=New-Object Drawing.Size(356,80);$m.Font=New-Object Drawing.Font('Microsoft YaHei UI',10);$m.ForeColor=[Drawing.ColorTranslator]::FromHtml('#6E6E73');$f.Controls.Add($m)
$k=New-Object System.Windows.Forms.Button;$k.Text=(S '@OK@');$k.Location=New-Object Drawing.Point(300,152);$k.Size=New-Object Drawing.Size(88,32);$k.FlatStyle='Flat';$k.BackColor=[Drawing.Color]::White;$k.ForeColor=[Drawing.ColorTranslator]::FromHtml('#1677FF');$k.FlatAppearance.BorderColor=[Drawing.ColorTranslator]::FromHtml('#1677FF');$k.FlatAppearance.BorderSize=1;$k.DialogResult=[Windows.Forms.DialogResult]::OK;$f.AcceptButton=$k;$f.Controls.Add($k)
[void]$f.ShowDialog()
"""
    for placeholder, value in values.items():
        script = script.replace(placeholder, value)
    ps(script)


def run_cli(args: argparse.Namespace) -> int:
    try:
        master_key = bytes.fromhex(args.master_key.strip())
        if len(master_key) != 32:
            raise ValueError
    except (ValueError, TypeError):
        print("错误：授权密钥格式不正确，应为 64 位 HEX。", file=sys.stderr)
        return 1
    device_id = args.device_id.strip().upper()
    code = generate_activation_code(device_id, master_key)
    print()
    print(f"  设备 ID：{device_id}")
    print(f"  激活码：  {code}")
    if args.order:
        print(f"  订单号：  {args.order}")
    print()
    if args.output:
        write_csv(args.output, device_id, code, args.order, args.note)
        print(f"  已记录到 {args.output}")
    return 0


def run_gui(args: argparse.Namespace) -> int:
    data = gui_collect()
    if data is None:
        return 0
    device_id = data["device_id"].strip().upper()
    master_hex = data["master_key"].strip()
    try:
        master_key = bytes.fromhex(master_hex)
        if len(master_key) != 32:
            raise ValueError
    except (ValueError, TypeError):
        gui_error("授权密钥格式不正确，应为 64 位 HEX。")
        return 1
    if len(device_id) != 8:
        gui_error("设备 ID 应为 8 位十六进制（例如 ABC1234D）。")
        return 1
    code = generate_activation_code(device_id, master_key)
    if data["output"]:
        try:
            write_csv(data["output"], device_id, code, data["order"], data["note"])
        except OSError as exc:
            gui_error(f"无法写入记录文件：\n{exc}")
            return 1
    gui_show_result(device_id, code, data["order"], data["note"], data["output"])
    return 0


def write_csv(path: str, device_id: str, code: str, order: str, note: str) -> None:
    csv_path = Path(path)
    write_header = not csv_path.exists()
    with csv_path.open("a", newline="", encoding="utf-8-sig") as f:
        writer = csv.writer(f)
        if write_header:
            writer.writerow(["时间", "订单号", "设备ID", "激活码", "备注"])
        writer.writerow([
            datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
            order, device_id, code, note,
        ])


def main() -> int:
    parser = argparse.ArgumentParser(description="安全信鸽 · 赋码工具")
    parser.add_argument("--device-id", default="", help="设备 ID（收件箱过期时显示在屏幕上）")
    parser.add_argument("--order", default="", help="订单号（选填，用于记录）")
    parser.add_argument("--master-key", default="", help="授权密钥（部署时保存的 64 位 HEX）")
    parser.add_argument("--output", default="", help="记录文件路径（选填，追加 CSV）")
    parser.add_argument("--note", default="", help="备注（选填）")
    args = parser.parse_args()


    if args.device_id and args.master_key:
        return run_cli(args)
    return run_gui(args)


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except SystemExit:
        raise
    except Exception as exc:
        try:
            gui_error(f"发生意外错误。\n\n{exc}")
        except Exception:
            pass
        raise SystemExit(1)
