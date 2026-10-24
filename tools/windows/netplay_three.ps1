# Three game instances on this PC, each one a player in an online match (recomp-to-recomp netplay test).
#   powershell -ExecutionPolicy Bypass -File tools\windows\netplay_three.ps1 [-Seconds 600] [-Exe <path>]
#
# Same layout as netplay_local.ps1 (read that for the two-instance details), plus instance C:
# A (host, say): net_port 3074; saves in userdata\;    log logs\net_a.log
# B:             net_port 3075; saves in userdata_b\;  log logs\net_b.log
# C:             net_port 3076; saves in userdata_c\;  log logs\net_c.log
# Each instance has its own XUID and gamertag, so none of them matches another as "ourselves".
# Each answers session searches on UDP 3070 + its offset (3070, 3071, 3072).
#
# Input: A reads the controllers (-AInput controller, the default). B and C are keyboard-only
# (input_backend=none, mnk_mode=true) so they don't take A's pads. The keyboard driver acts only
# on the focused window, so B and C can't both play from one keyboard. Use -BInput/-CInput
# "controller" to give them pads too; with -BackgroundPads, every window also reads the pads
# while unfocused. Every instance sees every connected pad, so this script doesn't pick a pad per
# instance; see the notes at the bottom of the printed summary.
param(
  [int]$Seconds = 0,
  [string]$Exe = "",
  [ValidateSet("keyboard", "controller")][string]$BInput = "keyboard",
  [ValidateSet("keyboard", "controller")][string]$CInput = "keyboard",
  [switch]$BackgroundPads,  # SDL reads the pads of unfocused windows too (SDL_JOYSTICK_ALLOW_BACKGROUND_EVENTS)
  [string]$Level = "trace"  # trace: every packet (sendto/recvfrom) is logged
)
$ErrorActionPreference = "Stop"
$Root = (Resolve-Path "$PSScriptRoot\..\..").Path
if (-not $Exe) { $Exe = "$Root\out\build\win-amd64-release\svr2007.exe" }
if (-not (Test-Path $Exe)) { throw "no game at $Exe (build it first: tools\windows\build.ps1)" }
New-Item -ItemType Directory -Force "$Root\logs" | Out-Null
if (-not $env:SVR_DUMP_SHADERS) { $env:SVR_DUMP_SHADERS = "$Root\logs\shaders" }
# See netplay_local.ps1: name the API of the exe being run, so "auto" doesn't start a second copy.
$Api = if ($Exe -match 'd3d12') { "d3d12" } else { "vulkan" }
if ($BackgroundPads) { $env:SDL_JOYSTICK_ALLOW_BACKGROUND_EVENTS = "1" } else { Remove-Item Env:SDL_JOYSTICK_ALLOW_BACKGROUND_EVENTS -ErrorAction SilentlyContinue }

function Start-Instance($tag, [string[]]$extra) {
  $log = "$Root\logs\net_$tag.log"
  if (Test-Path $log) { Move-Item $log "$Root\logs\net_$tag.prev.log" -Force }
  Get-ChildItem "$Root\logs" -Filter "net_$tag.*.log" -ErrorAction SilentlyContinue |
    Where-Object { $_.Name -match "^net_$tag\.\d+\.log$" } | Remove-Item -Force
  $argv = @(
    "`"--game_data_root=$Root\assets`"", "`"--log_file=$log`"", "--log_level=$Level",
    "--log_levels=gpu=info,apu=info,fs=info", "--log_max_file_size_mb=50", "--log_max_files=40",
    "--fullscreen=false", "--window_width=960", "--window_height=540",
    "--svr_graphics_api=$Api", "--svr_60fps=true"
  ) + $extra
  $p = Start-Process $Exe -ArgumentList $argv -PassThru
  Write-Host "instance $tag started (pid $($p.Id)), log $log" -ForegroundColor Cyan
  return $p
}

function Input-Args($mode) {
  if ($mode -eq "keyboard") { @("--input_backend=none", "--mnk_mode=true") } else { @() }
}

$a = Start-Instance "a" @("`"--user_data_root=$Root\userdata`"", "--net_port=3074")
Start-Sleep -Seconds 3
$b = Start-Instance "b" (@(
  "`"--user_data_root=$Root\userdata_b`"", "--net_port=3075",
  "--net_xuid=2533274790395906", "--net_gamertag=SuperstarB"
) + (Input-Args $BInput))
Start-Sleep -Seconds 3
$c = Start-Instance "c" (@(
  "`"--user_data_root=$Root\userdata_c`"", "--net_port=3076",
  "--net_xuid=2533274790395907", "--net_gamertag=SuperstarC"
) + (Input-Args $CInput))

Write-Host "`nOn one instance: Xbox LIVE > host a match (create). On the other two: Xbox LIVE > find a match."
Write-Host "Close all three windows when done$(if ($Seconds) { ", or wait $Seconds s" })."
$procs = @($a, $b, $c)
$deadline = if ($Seconds) { (Get-Date).AddSeconds($Seconds) } else { [datetime]::MaxValue }
while (($procs | Where-Object { -not $_.HasExited }) -and (Get-Date) -lt $deadline) { Start-Sleep -Milliseconds 500 }
foreach ($p in $procs) { if (-not $p.HasExited) { & taskkill /F /T /PID $p.Id | Out-Null } }

$steps = [ordered]@{
  "XNet up"              = '\[net\] XNetStartup'
  "discovery listening"  = '\[net\] discovery: listening'
  "hosted a session"     = '\[net\] XSessionCreate: hosting'
  "advertising"          = '\[net\] discovery: now advertising'
  "searched"             = '\[net\] XSessionSearch'
  "found a session"      = '\[net\] discovery: search found [1-9]'
  "QoS probe answered"   = '\[net\] XNetQosLookup #\d+: .* contacted'
  "joined a session"     = '\[net\] XSessionCreate: joining'
  "peer registered"      = '\[net\] XNetXnAddrToInAddr'
  "game traffic sent"    = '\[net\] (sendto|WSASendTo)'
  "game traffic received" = '\[net\] (recvfrom|WSARecvFrom)'
  "peer identified"      = '\[net\] XNetInAddrToXnAddr'
  "DemonWare join done"  = '\[dw\].*(Join allowed|Connected to all peers)'
  "match started"        = '\[fps\] entering gameplay'
}
foreach ($tag in @("a", "b", "c")) {
  $log = "$Root\logs\net_$tag.log"
  Write-Host "`n=== instance $tag ($log)" -ForegroundColor Cyan
  if (-not (Test-Path $log)) { Write-Host "  no log"; continue }
  $pieces = @(Get-ChildItem "$Root\logs" -Filter "net_$tag.*.log" | Where-Object { $_.Name -match "^net_$tag\.(\d+)\.log$" } |
    Sort-Object { [int]($_.Name -replace "^net_$tag\.(\d+)\.log$", '$1') } -Descending | ForEach-Object FullName) + $log
  $text = Get-Content $pieces
  foreach ($step in $steps.GetEnumerator()) {
    $hit = $text | Select-String -Pattern $step.Value | Select-Object -First 1
    if ($hit) {
      Write-Host ("  [x] {0,-22} {1}" -f $step.Key, $hit.Line.Substring(0, [Math]::Min(150, $hit.Line.Length)))
    } else {
      Write-Host ("  [ ] {0}" -f $step.Key) -ForegroundColor DarkGray
    }
  }
  $online = $text | Select-String -Pattern '\[online\] state' | Select-Object -First 12
  if ($online) { Write-Host "  online menu states:" -ForegroundColor DarkCyan; foreach ($l in $online) { Write-Host ("    " + $l.Line.Substring(26, [Math]::Min(150, $l.Line.Length - 26))) } }
  $problems = $text | Select-String -Pattern '\[net\].*(error|fail|unexpected|short buffer|no answer)|Unimplemented XGI|\[(critical)\]' |
    Select-Object -First 8
  foreach ($line in $problems) { Write-Host ("  ! " + $line.Line.Substring(0, [Math]::Min(170, $line.Line.Length))) -ForegroundColor Yellow }
  $game = $text | Select-String -Pattern '\] \[(dw|gss)\] ' | Select-Object -Last 12
  if ($game) { Write-Host "  game network messages (last 12):" -ForegroundColor DarkCyan; foreach ($l in $game) { Write-Host ("    " + $l.Line.Substring(26, [Math]::Min(170, $l.Line.Length - 26))) } }
}
