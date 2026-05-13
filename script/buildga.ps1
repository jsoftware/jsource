param(
    [Parameter(Mandatory=$true)]
    [ValidateSet("x86","x64","arm64")]
    [string]$arch
)

Write-Host "Build windows on github actions"

Write-Host "Environment variables:"
Get-ChildItem Env:

clang-cl --version

# Check environment variables USE_EMU_AVX and USE_PYXES
if (($env:USE_EMU_AVX -eq "0") -or ($env:USE_PYXES -eq "0")) {
    Write-Host "Skipping build avx2 avx512 due to USE_EMU_AVX or USE_PYXES being 0"
}

Get-ChildItem -Recurse -Filter 'libomp.dll' -File -ErrorAction SilentlyContinue -Path 'C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\'
Get-ChildItem -Recurse -Filter 'libomp.lib' -File -ErrorAction SilentlyContinue -Path 'C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\'

Write-Host $env:USE_EMU_AVX
Write-Host $env:USE_PYXES

$A = "jlibrary"
$B = "jlibrary\bin"
$C = "jlibrary\bin32"

Write-Host $A
Write-Host $B
Write-Host $C

if ($arch -ne "x86" -and $arch -ne "arm64" -and $arch -ne "x64") {
    exit 1
}

switch ($arch) {
    "x86" {
        New-Item -ItemType Directory -Force -Path "j32" | Out-Null
        New-Item -ItemType Directory -Force -Path $C | Out-Null
        New-Item -ItemType Directory -Force -Path "bin\windows\j32" | Out-Null

        Copy-Item -Path "$B\profile.ijs" -Destination $C -ErrorAction SilentlyContinue
        # Copy pthreads4w\x86\pthreadVC3.dll $C\pthreadVC3.dll (commented out)
        Copy-Item -Path "mpir\windows\x86\mpir.dll" -Destination "$C\mpir32.dll" -Force
        # Copy openmp\obj\windows\x86\libomp.dll $C (commented out)
        Copy-Item -Path "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Tools\Llvm\bin\libomp.dll" -Destination $C -Force
        Copy-Item -Path "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Tools\Llvm\lib\libomp.lib" -Destination $C -Force
        Copy-Item -Path "pcre2\windows\x86\jpcre2.dll" -Destination "$A\tools\regex\jpcre2_32.dll" -Force
        Invoke-WebRequest -Uri "https://www.jsoftware.com/download/lapackbin/x86/libopenblas.dll" -OutFile "$C\libopenblas.dll"
    }
    "arm64" {
        New-Item -ItemType Directory -Force -Path "j64" | Out-Null
        New-Item -ItemType Directory -Force -Path "bin\windows\j64arm" | Out-Null
        # Copy pthreads4w\arm64\pthreadVC3.dll $B\pthreadVC3.dll (commented out)
        Copy-Item -Path "mpir\windows\arm64\mpir.dll" -Destination $B -Force
        # Copy openmp\obj\windows\arm64\libomp.dll $B (commented out)
        Copy-Item -Path "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Tools\Llvm\ARM64\bin\libomp.dll" -Destination $B -Force
        Copy-Item -Path "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Tools\Llvm\ARM64\lib\libomp.lib" -Destination $B -Force
        Copy-Item -Path "pcre2\windows\arm64\jpcre2.dll" -Destination "$A\tools\regex\jpcre2_arm64.dll" -Force
        Invoke-WebRequest -Uri "https://www.jsoftware.com/download/lapackbin/arm64/libopenblas.dll" -OutFile "$B\libopenblas.dll"
    }
    "x64" {
        New-Item -ItemType Directory -Force -Path "j64" | Out-Null
        New-Item -ItemType Directory -Force -Path "bin\windows\j64" | Out-Null
        New-Item -ItemType Directory -Force -Path "bin\windows\j64avx2" | Out-Null
        New-Item -ItemType Directory -Force -Path "bin\windows\j64avx512" | Out-Null
        # Copy pthreads4w\x64\pthreadVC3.dll $B (commented out)
        Copy-Item -Path "mpir\windows\x64\mpir.dll" -Destination $B -Force
        # Copy openmp\obj\windows\x64\libomp.dll $B (commented out)
        Copy-Item -Path "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Tools\Llvm\x64\bin\libomp.dll" -Destination $B -Force
        Copy-Item -Path "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Tools\Llvm\x64\lib\libomp.lib" -Destination $B -Force
        Copy-Item -Path "pcre2\windows\x64\jpcre2.dll" -Destination "$A\tools\regex\" -Force
        Invoke-WebRequest -Uri "https://www.jsoftware.com/download/lapackbin/x64/libopenblas.dll" -OutFile "$B\libopenblas.dll"
    }
    default {
        Write-Error "Unsupported architecture: $arch"
        exit 1
    }
}

Copy-Item -Path "version.txt" -Destination "jsrc\jversion.h" -Force

Add-Content -Path "jsrc\jversion.h" -Value '#define jplatform "windows"'
Add-Content -Path "jsrc\jversion.h" -Value '#define jlicense "commercial"'
Add-Content -Path "jsrc\jversion.h" -Value '#define jbuilder "www.jsoftware.com"'

Set-Location -Path "jsrc"

$env:CC = "clang-cl"

switch ($arch) {
    "x86" {
        nmake -f ..\makemsvc\tsdll\makefile.win jplatform=windows j64x=j32 clean
        nmake -f ..\makemsvc\tsdll\makefile.win jplatform=windows j64x=j32
        if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    }
    "arm64" {
        nmake -f ..\makemsvc\tsdll\makefile.win jplatform=windows j64x=j64arm clean
        nmake -f ..\makemsvc\tsdll\makefile.win jplatform=windows j64x=j64arm
        if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    }
    "x64" {
        nmake -f ..\makemsvc\tsdll\makefile.win jplatform=windows j64x=j64 clean
        nmake -f ..\makemsvc\tsdll\makefile.win jplatform=windows j64x=j64
        if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    }
    default {
        Write-Error "Unsupported architecture: $arch"
        exit 1
    }
}

switch ($arch) {
    "x86" {
        nmake -f ..\makemsvc\jconsole\makefile.win jplatform=windows j64x=j32 clean
        nmake -f ..\makemsvc\jconsole\makefile.win jplatform=windows j64x=j32
        if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    }
    "arm64" {
        nmake -f ..\makemsvc\jconsole\makefile.win jplatform=windows j64x=j64arm clean
        nmake -f ..\makemsvc\jconsole\makefile.win jplatform=windows j64x=j64arm
        if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    }
    "x64" {
        nmake -f ..\makemsvc\jconsole\makefile.win jplatform=windows j64x=j64 clean
        nmake -f ..\makemsvc\jconsole\makefile.win jplatform=windows j64x=j64
        if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    }
    default {
        Write-Error "Unsupported architecture: $arch"
        exit 1
    }
}

switch ($arch) {
    "x86" {
        nmake -f ..\makemsvc\jdll\makefile.win jplatform=windows j64x=j32 clean
        nmake -f ..\makemsvc\jdll\makefile.win jplatform=windows j64x=j32
        if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    }
    "arm64" {
        nmake -f ..\makemsvc\jdll\makefile.win jplatform=windows j64x=j64arm NO_SHA_ASM=1 clean
        nmake -f ..\makemsvc\jdll\makefile.win jplatform=windows j64x=j64arm NO_SHA_ASM=1
        if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    }
    "x64" {
        if ("$env:USE_EMU_AVX" -ne "0" -and "$env:USE_PYXES" -ne "0") {
            nmake -f ..\makemsvc\jdll\makefile.win jplatform=windows j64x=j64avx512 clean
            nmake -f ..\makemsvc\jdll\makefile.win jplatform=windows j64x=j64avx512
            if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

            nmake -f ..\makemsvc\jdll\makefile.win jplatform=windows j64x=j64avx2 clean
            nmake -f ..\makemsvc\jdll\makefile.win jplatform=windows j64x=j64avx2
            if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
        }
        nmake -f ..\makemsvc\jdll\makefile.win jplatform=windows j64x=j64 clean
        nmake -f ..\makemsvc\jdll\makefile.win jplatform=windows j64x=j64
        if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    }
    default {
        Write-Error "Unsupported architecture: $arch"
        exit 1
    }
}

Set-Location ..
Write-Host Get-Location

switch ($arch) {
    "x86" {
        Copy-Item -Path "bin\windows\j32\jconsole.exe" -Destination "$env:C"
        Copy-Item -Path "bin\windows\j32\*.dll" -Destination "$env:C"
    }
    "arm64" {
        Copy-Item -Path "bin\windows\j64arm\jconsole.exe" -Destination "$env:B"
        Copy-Item -Path "bin\windows\j64arm\*.dll" -Destination "$env:B"
    }
    "x64" {
        Copy-Item -Path "bin\windows\j64\jconsole.exe" -Destination "$env:B"
        Copy-Item -Path "bin\windows\j64\*.dll" -Destination "$env:B"
        if ("$env:USE_EMU_AVX" -ne "0" -and "$env:USE_PYXES" -ne "0") {
            Copy-Item -Path "bin\windows\j64avx512\j.dll" -Destination "$env:B\javx512.dll"
            Copy-Item -Path "bin\windows\j64avx2\j.dll" -Destination "$env:B\javx2.dll"
        }
    }
    default {
        Write-Error "Unsupported architecture: $arch"
        exit 1
    }
}
