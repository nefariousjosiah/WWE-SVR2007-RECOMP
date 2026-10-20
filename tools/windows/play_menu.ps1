# Local only (not in the repository): one-screen settings menu, then starts the game.
# Arrow keys: up/down choose a setting, left/right change it, Enter plays, Esc quits.
# Choices are remembered in play_settings.ini next to the game. Used by "Play WWE <year>.bat".
$Root = (Resolve-Path "$PSScriptRoot\..\..").Path
$Exe = Get-ChildItem "$Root\out\build\win-amd64-release\svr*.exe" | Select-Object -First 1
if (-not $Exe) { Write-Host "Build the game first (tools\windows\build.ps1)." -ForegroundColor Red; Read-Host; exit 1 }
$Id = $Exe.BaseName
$Year = $Id.Substring(3)
$Ini = "$Root\play_settings.ini"
$Tracker = "$PSScriptRoot\tracker.ps1"
[Console]::OutputEncoding = [Text.Encoding]::UTF8
$raw = $Host.UI.RawUI
$raw.WindowTitle = "WWE SmackDown vs. Raw $Year"
$raw.BackgroundColor = "Black"; $raw.ForegroundColor = "Gray"
try { [Console]::CursorVisible = $false } catch {}

# What "Auto" resolution means on this PC (same rule as the game: 1440p up to 1440p screens, 4K above).
Add-Type -AssemblyName System.Windows.Forms
$screenH = [System.Windows.Forms.Screen]::PrimaryScreen.Bounds.Height
$autoRes = if ($screenH -gt 1440) { "4K" } else { "1440p" }

$Settings = @(
  @{ Key = "display"; Name = "Display"; Default = 0
     Options = @(@("Fullscreen", "fullscreen"), @("Window 1600 x 900", "1600x900"), @("Window 1280 x 720", "1280x720"))
     Hint = "Fullscreen fills your screen. A window is handy for streaming or recording." },
  @{ Key = "scale"; Name = "Resolution / anti-aliasing"; Default = 0
     Options = @(@("Auto ($autoRes on this screen)", "0"), @("720p (original)", "1"), @("1440p", "2"), @("4K", "3"))
     Hint = "The game renders at this resolution and scales it to your screen. Higher = smoother, sharper edges; 4K needs a strong graphics card." },
  @{ Key = "fps"; Name = "Frame rate"; Default = 0; Options = @(@("60 fps", "true"), @("30 fps (original)", "false"))
     Hint = "60 fps uses the game's own 60 fps mode, so everything still plays at the right speed." },
  @{ Key = "shape"; Name = "Screen shape"; Default = 0; Options = @(@("16:9 (correct)", "5"), @("Stretch to fill", "6"))
     Hint = "Only matters on 16:10 screens such as the Steam Deck: 16:9 keeps thin bars, stretch fills the screen." },
  @{ Key = "counter"; Name = "FPS counter"; Default = 0; Options = @(@("Show", "true"), @("Hide", "false"))
     Hint = "Shows the frame rate in the corner. F2 shows or hides it while playing." },
  @{ Key = "sound"; Name = "Sound"; Default = 0; Options = @(@("On", "false"), @("Muted", "true"))
     Hint = "Mute the game completely." },
  @{ Key = "keyboard"; Name = "Keyboard controls"; Default = 0
     Options = @(@("On", "true"), @("Off (controllers only)", "false"))
     Hint = "The keyboard works as a controller (rebind with F4 in game). Not tested yet: a controller is recommended." }
)
if (Test-Path $Tracker) {
  $Settings += @{ Key = "tracker"; Name = "Live tracker terminal"; Default = 1; Options = @(@("On", "true"), @("Off", "false"))
                  Hint = "Opens a terminal next to the game that tracks FPS, CPU, memory and disc activity." }
}

# Remembered choices.
$Choice = @{}
foreach ($s in $Settings) { $Choice[$s.Key] = $s.Default }
if (Test-Path $Ini) {
  foreach ($line in Get-Content $Ini) {
    if ($line -match '^\s*([a-z]+)\s*=\s*(\d+)\s*$' -and $Choice.ContainsKey($Matches[1])) {
      $s = $Settings | Where-Object { $_.Key -eq $Matches[1] }
      $Choice[$Matches[1]] = [Math]::Min([int]$Matches[2], $s.Options.Count - 1)
    }
  }
}
function Save { ($Settings | ForEach-Object { "{0}={1}" -f $_.Key, $Choice[$_.Key] }) | Set-Content $Ini -Encoding ascii }

$L = [char]0x25C0; $R = [char]0x25B6; $Up = [char]0x2191; $Dn = [char]0x2193; $Lt = [char]0x2190; $Rt = [char]0x2192
$Line = [string][char]0x2500
$W = 76
function Put($text, $color = "Gray", [switch]$NoNewline) {
  if ($NoNewline) { Write-Host $text -ForegroundColor $color -NoNewline } else { Write-Host $text -ForegroundColor $color }
}
function Draw($sel) {
  [Console]::SetCursorPosition(0, 0)
  Put (" " * $W)
  Put ("  WWE SMACKDOWN VS. RAW $Year".PadRight($W)) Green
  Put ("  native PC  $([char]0x00B7)  60 fps  $([char]0x00B7)  up to 4K".PadRight($W)) DarkGray
  Put ("  " + ($Line * 60)).PadRight($W) DarkGreen
  Put (" " * $W)
  # Row 0: PLAY.
  if ($sel -eq 0) { Put "   $R " Green -NoNewline; Put ("PLAY".PadRight($W - 6)) Green }
  else { Put ("     PLAY".PadRight($W)) White }
  Put (" " * $W)
  for ($i = 0; $i -lt $Settings.Count; $i++) {
    $s = $Settings[$i]; $value = $s.Options[$Choice[$s.Key]][0]
    $active = ($sel -eq $i + 1)
    $pointer = if ($active) { "   $R " } else { "     " }
    Put $pointer Yellow -NoNewline
    Put ($s.Name.PadRight(28)) $(if ($active) { "White" } else { "DarkGray" }) -NoNewline
    if ($active) { Put "$L " Yellow -NoNewline; Put $value Cyan -NoNewline; Put " $R" Yellow -NoNewline; Put (" " * [Math]::Max(0, $W - 38 - $value.Length)) }
    else { Put ("  " + $value).PadRight($W - 33) Cyan }
  }
  Put (" " * $W)
  Put ("  " + ($Line * 60)).PadRight($W) DarkGreen
  $hint = if ($sel -eq 0) { "Press Enter to play with these settings. Your choices are saved for next time." }
          else { $Settings[$sel - 1].Hint }
  # Wrap the hint to two lines.
  $words = $hint -split ' '; $lines = @(""); foreach ($w in $words) { if (($lines[-1] + " " + $w).Length -gt 70) { $lines += $w } else { $lines[-1] = ($lines[-1] + " " + $w).Trim() } }
  while ($lines.Count -lt 3) { $lines += "" }
  foreach ($l in $lines[0..2]) { Put ("  " + $l).PadRight($W) Gray }
  Put (" " * $W)
  Put "  $Up$Dn " Yellow -NoNewline; Put "choose   " DarkGray -NoNewline
  Put "$Lt$Rt " Yellow -NoNewline; Put "change   " DarkGray -NoNewline
  Put "Enter " Green -NoNewline; Put "play   " DarkGray -NoNewline
  Put "Esc " Yellow -NoNewline; Put ("quit".PadRight(30)) DarkGray
}

Clear-Host
$sel = 0
while ($true) {
  Draw $sel
  $k = [Console]::ReadKey($true)
  switch ($k.Key) {
    "UpArrow"    { $sel = ($sel + $Settings.Count) % ($Settings.Count + 1) }
    "DownArrow"  { $sel = ($sel + 1) % ($Settings.Count + 1) }
    "LeftArrow"  { if ($sel -gt 0) { $s = $Settings[$sel - 1]; $Choice[$s.Key] = ($Choice[$s.Key] + $s.Options.Count - 1) % $s.Options.Count; Save } }
    "RightArrow" { if ($sel -gt 0) { $s = $Settings[$sel - 1]; $Choice[$s.Key] = ($Choice[$s.Key] + 1) % $s.Options.Count; Save } }
    "Enter"      { break }
    "Escape"     { try { [Console]::CursorVisible = $true } catch {}; exit 0 }
  }
  if ($k.Key -eq "Enter") { break }
}
Save
try { [Console]::CursorVisible = $true } catch {}

# --- start the game ----------------------------------------------------------------------------
function Value($key) { $s = $Settings | Where-Object { $_.Key -eq $key }; $s.Options[$Choice[$key]][1] }
$display = Value "display"
$args2 = @("--svr_render_scale=$(Value 'scale')", "--svr_60fps=$(Value 'fps')", "--bd_aspect_ratio=$(Value 'shape')",
           "--svr_fps_counter=$(Value 'counter')", "--audio_mute=$(Value 'sound')", "--mnk_mode=$(Value 'keyboard')")
if ($display -eq "fullscreen") { $args2 += "--fullscreen=true" }
else { $w, $h = $display -split "x"; $args2 += @("--fullscreen=false", "--window_width=$w", "--window_height=$h") }

Clear-Host
if ($Choice.ContainsKey("tracker") -and (Value "tracker") -eq "true") {
  & $Tracker @args2
  exit 0
}
$Log = "$Root\logs\game.log"
New-Item -ItemType Directory -Force "$Root\logs" | Out-Null
if (Test-Path $Log) { Move-Item $Log "$Root\logs\game.prev.log" -Force }
$gameArgs = @("`"--game_data_root=$Root\assets`"", "`"--user_data_root=$Root\userdata`"", "`"--log_file=$Log`"",
              "--gpu_backend=vulkan", "--texture_cache_memory_limit_soft=3072", "--texture_cache_memory_limit_hard=4096",
              "--anisotropic_override=5") + $args2
Start-Process $Exe.FullName -ArgumentList $gameArgs | Out-Null
Put "`n  Starting WWE SmackDown vs. Raw $Year..." Green
Start-Sleep -Seconds 2
