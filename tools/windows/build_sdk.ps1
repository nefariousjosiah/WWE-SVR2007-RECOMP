# Rebuild and reinstall this project's ReXGlue SDK (third_party\rexglue-sdk -> sdk\) after
# changing its source. The build folder is third_party\rexglue-sdk\out\build\win-amd64; it must have
# been configured for THIS project (its install prefix is checked, so it can't install elsewhere).
#   powershell -ExecutionPolicy Bypass -File tools\windows\build_sdk.ps1
$ErrorActionPreference = "Stop"
. "$PSScriptRoot\env.ps1"
$Sdk = "$Root\third_party\rexglue-sdk"
$Build = "$Sdk\out\build\win-amd64"
$Cache = "$Build\CMakeCache.txt"
if (Test-Path $Cache) {
  $prefix = (Select-String -Path $Cache -Pattern "^CMAKE_INSTALL_PREFIX:PATH=(.*)$").Matches[0].Groups[1].Value
  if ((Resolve-Path $prefix -ErrorAction SilentlyContinue).Path -ne (Resolve-Path "$Root\sdk").Path) {
    throw "$Build installs into $prefix, not this project's sdk folder"
  }
}
Push-Location $Sdk
if (-not (Test-Path "$Build\build.ninja")) {
  Run cmake @("--preset", "win-amd64", "-DREXGLUE_USE_VULKAN=ON",
              "-DCMAKE_C_COMPILER=$Clang", "-DCMAKE_CXX_COMPILER=$ClangXX",
              "-DCMAKE_INSTALL_PREFIX=$Root\sdk")
}
Run cmake @("--build", $Build, "--config", "Release", "--target", "install", "--parallel")
Pop-Location
"sdk build done"
