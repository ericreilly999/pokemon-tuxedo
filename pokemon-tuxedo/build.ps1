# Build using MSYS2 bash to avoid Windows pipe issues
$env:DEVKITPRO = "C:/devkitPro"
$env:DEVKITARM = "C:/devkitPro/devkitARM"

# Remove Windows Store python stubs from PATH
$cleanPath = ($env:PATH -split ';' | Where-Object { $_ -notlike '*WindowsApps*' }) -join ';'
$env:PATH = "C:\devkitPro\msys2\usr\bin;C:\devkitPro\devkitARM\bin;C:\devkitPro\tools\bin;$cleanPath"

# Rebuild trainerproc first (GCC 15 fix), then build ROM
& "C:\devkitPro\msys2\usr\bin\bash.exe" -c "export DEVKITPRO=/c/devkitPro; export DEVKITARM=/c/devkitPro/devkitARM; export PATH=/c/devkitPro/msys2/usr/bin:/c/devkitPro/devkitARM/bin:/c/devkitPro/tools/bin:`$PATH; cd tools/trainerproc && make clean && make && cd ../.. && make modern -j4 2>&1"
