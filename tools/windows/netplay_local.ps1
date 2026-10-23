# Two game instances on this PC playing each other online (recomp-to-recomp netplay test).
#   powershell -ExecutionPolicy Bypass -File tools\windows\netplay_local.ps1 [-Seconds 600] [-Exe <path>]
#
# A (host, say): your controller; net_port 3074; saves in userdata\;   log logs\net_a.log
# B (joiner):    keyboard only (input_backend=none; -BInput controller changes that), so the controller doesn't drive both; it
#                only gets keys while its window has focus (defaults: README, F4 to rebind);
#                net_port 3075 (port offset 1); its own identity and userdata_b\; logs\net_b.log
# Each instance answers session searches on UDP 3070 + its offset; a search asks this PC, the
# LAN broadcast and net_host_ip. When both exit (or after -Seconds), the [net] lines that tell
# how far they got are summarized here.
param(
  [int]$Seconds = 0,
  [string]$Exe = "",
  # B's input: "keyboard" (default; B ignores the controller) or "controller" (B reads the same
  # controller as A; only the window with focus gets it, like any SDL game).
  [ValidateSet("keyboard", "controller")][string]$BInput = "keyboard",
  [string]$Level = "trace"  # trace: every packet (sendto/recvfrom) is logged
)
$ErrorActionPreference = "Stop"
$Root = (Resolve-Path "$PSScriptRoot\..\..").Path
if (-not $Exe) { $Exe = "$Root\out\build\win-amd64-release\svr2007.exe" }
if (-not (Test-Path $Exe)) { throw "no game at $Exe (build it first: tools\windows\build.ps1)" }
New-Item -ItemType Directory -Force "$Root\logs" | Out-Null
# Shaders either build is missing go where build_shader_cache.ps1 reads them.
if (-not $env:SVR_DUMP_SHADERS) { $env:SVR_DUMP_SHADERS = "$Root\logs\shaders" }
# In out\build both renderers are named svr2007.exe, so the "auto" graphics API hand-off
# (src/graphics_api.h) would start this same exe again as a second process. Name the API of
# the exe being run instead.
$Api = if ($Exe -match 'd3d12') { "d3d12" } else { "vulkan" }

function Start-Instance($tag, [string[]]$extra) {
  $log = "$Root\logs\net_$tag.log"
  if (Test-Path $log) { Move-Item $log "$Root\logs\net_$tag.prev.log" -Force }
  $argv = @(
    "`"--game_data_root=$Root\assets`"", "`"--log_file=$log`"", "--log_level=$Level",
    "--fullscreen=false", "--window_width=960", "--window_height=540",
    "--svr_graphics_api=$Api", "--svr_60fps=true"
  ) + $extra
  $p = Start-Process $Exe -ArgumentList $argv -PassThru
  Write-Host "instance $tag started (pid $($p.Id)), log $log" -ForegroundColor Cyan
  return $p
}

$a = Start-Instance "a" @("`"--user_data_root=$Root\userdata`"", "--net_port=3074")
# Let A take its ports first, so B's errors (if any) are about B.
Start-Sleep -Seconds 3
$b = Start-Instance "b" @(
  "`"--user_data_root=$Root\userdata_b`"", "--net_port=3075",
  "--net_xuid=2533274790395906", "--net_gamertag=SuperstarB"
) + $(if ($BInput -eq "keyboard") { @("--input_backend=none", "--mnk_mode=true") } else { @() })

Write-Host "`nOn one instance: Xbox LIVE > host a match (create). On the other: Xbox LIVE > find a match."
Write-Host "Close both windows when done$(if ($Seconds) { ", or wait $Seconds s" })."
$deadline = if ($Seconds) { (Get-Date).AddSeconds($Seconds) } else { [datetime]::MaxValue }
while (-not ($a.HasExited -and $b.HasExited) -and (Get-Date) -lt $deadline) { Start-Sleep -Milliseconds 500 }
# /T: the whole tree, in case an instance started another program.
foreach ($p in @($a, $b)) { if (-not $p.HasExited) { & taskkill /F /T /PID $p.Id | Out-Null } }

# The milestones, in order; the first one missing is where to look.
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
  "match started"        = '\[fps\] entering gameplay'
}
foreach ($tag in @("a", "b")) {
  $log = "$Root\logs\net_$tag.log"
  Write-Host "`n=== instance $tag ($log)" -ForegroundColor Cyan
  if (-not (Test-Path $log)) { Write-Host "  no log"; continue }
  $text = Get-Content $log
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
}
