# Build svr2007.exe on the SDK's emulated Xenos GPU: no native renderer and no D3D hooks (bring-up:
# recompilation problems show up on their own before the renderer is mapped for this game).
#   powershell -ExecutionPolicy Bypass -File tools\windows\build_emulated.ps1
# Output: out\build\win-amd64-emu\svr2007.exe (run it with --gpu_backend=vulkan). Includes the
# shader dump: SVR_DUMP_SHADERS=<dir> writes every shader the game creates.
$ErrorActionPreference = "Stop"
. "$PSScriptRoot\env.ps1"
Set-Location $Root
$Dir = "out\build\win-amd64-emu"
Run cmake @("--preset", "win-amd64-release", "-B", $Dir, "-DCMAKE_PREFIX_PATH=$Root\sdk",
            "-DCMAKE_CXX_COMPILER=$ClangXX", "-DSVR_NATIVE_RENDERER=OFF", "-DSVR_D3D_CENSUS=OFF", "-DSVR_SHADER_DUMP=ON")
Run cmake (@("--build", $Dir) + $args)
Copy-Item "$Root\sdk\bin\*.dll" "$Root\$Dir\" -Force
