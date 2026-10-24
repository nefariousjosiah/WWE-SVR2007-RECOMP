# List the shaders the native renderer didn't have in its cache (generated\native\shader_cache.cpp),
# from a game log: what's missing, and what the game was doing when it first asked for each one.
#   powershell -ExecutionPolicy Bypass -File tools\native\shader_misses.ps1 [-Log logs\game.prev.log]
# A miss means that shader's draws are skipped (something isn't drawn); it doesn't slow the game.
# To fill the gaps: replay those scenes in the shader-dump build, then run
# tools\native\build_shader_cache.ps1 and tools\windows\build.ps1 again.
param([string]$Log = "logs\game.log")
$ErrorActionPreference = "Stop"
$Root = (Resolve-Path "$PSScriptRoot\..\..").Path
if (-not [IO.Path]::IsPathRooted($Log)) { $Log = Join-Path $Root $Log }
if (-not (Test-Path $Log)) { throw "no log at $Log (run the game first)" }

$types = @{ "7" = "vertex"; "8" = "pixel" }
$misses = [ordered]@{}
$start = $null
$context = "menus / loading"
$gameplayCount = 0
# The game may hold the log open: read it shared.
$stream = [IO.File]::Open($Log, 'Open', 'Read', 'ReadWrite, Delete')
$reader = New-Object IO.StreamReader($stream)
while ($null -ne ($line = $reader.ReadLine())) {
  if ($line -notmatch '^\[(\d{4}-\d\d-\d\d \d\d:\d\d:\d\d\.\d+)\]') { continue }
  $time = [datetime]::ParseExact($Matches[1], "yyyy-MM-dd HH:mm:ss.fff", $null)
  if (-not $start) { $start = $time }
  if ($line -match '\[fps\] entering gameplay') {
    $gameplayCount++
    $context = "gameplay #$gameplayCount (entrance or match, from {0:N0} s)" -f ($time - $start).TotalSeconds
  } elseif ($line -match '\[fps\] leaving gameplay') {
    $context = "menus / loading"
  } elseif ($line -match 'Shader cache miss: hash=0x([0-9A-Fa-f]+) len=(\d+) type=(\d+)') {
    $hash = $Matches[1].ToUpper()
    if ($misses.Contains($hash)) {
      $misses[$hash].Count++
    } else {
      $kind = if ($types.ContainsKey($Matches[3])) { $types[$Matches[3]] } else { "type $($Matches[3])" }
      $misses[$hash] = [pscustomobject]@{
        Hash = $hash; Kind = $kind; Bytes = [int]$Matches[2]
        FirstSeen = "{0:HH:mm:ss} (+{1:N0} s)" -f $time, ($time - $start).TotalSeconds
        During = $context; Count = 1
      }
    }
  }
}
$reader.Close()

if ($misses.Count -eq 0) {
  Write-Host "No shader cache misses in $Log : everything this session drew was in the cache." -ForegroundColor Green
  Write-Host "(Only the native renderer build uses the cache; a shader-dump build's log never has misses.)"
  exit 0
}
$list = @($misses.Values)
$vs = @($list | Where-Object { $_.Kind -eq "vertex" }).Count
$ps = @($list | Where-Object { $_.Kind -eq "pixel" }).Count
Write-Host ("{0} shaders missing from the cache in {1} ({2} vertex, {3} pixel):" -f $list.Count, $Log, $vs, $ps) -ForegroundColor Yellow
$list | Format-Table Hash, Kind, Bytes, FirstSeen, During, Count -AutoSize
Write-Host "Replay those scenes in the shader-dump build, then rebuild the cache and the game."
