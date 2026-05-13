param(
    [Parameter(Mandatory=$true)]
    [ValidateSet("x86", "x64", "arm64")]
    [string]$arch
)

Write-Output $env:USE_EMU_AVX
Write-Output $env:USE_PYXES

$A = "jlibrary"
$B = "jlibrary\bin"
$C = "jlibrary\bin32"

Write-Output $A
Write-Output $B
Write-Output $C

if ($arch -ne "x86" -and $arch -ne "arm64" -and $arch -ne "x64") {
    exit 1
}

# Show system info
systeminfo

switch ($arch) {
    "x86" {
        Write-Output (Get-ChildItem -Path $C)
        & "$C\jconsole.exe" "script\testga.ijs"
        if ($LASTEXITCODE -ne 0) { exit 1 }
        break
    }
    "arm64" {
        Write-Output (Get-ChildItem -Path $B)
        if ($env:_DEBUG -eq "3") {
            & lldb -b -o run -k bt -k quit -- "$B\jconsole.exe" -lib j.dll "script\testga.ijs"
            if ($LASTEXITCODE -ne 0) { exit 1 }
        }
        else {
            & "$B\jconsole.exe" "script\testga.ijs"
            if ($LASTEXITCODE -ne 0) { exit 1 }
        }
        break
    }
    "x64" {
        Write-Output (Get-ChildItem -Path $B)
        if ($env:USE_EMU_AVX -eq "0" -or $env:USE_PYXES -eq "0") {
            # Skip AVX2 test
            if ($env:_DEBUG -eq "3") {
                & lldb -b -o run -k bt -k quit -- "$B\jconsole.exe" -lib j.dll "script\testga.ijs"
                if ($LASTEXITCODE -ne 0) { exit 1 }
            }
            else {
                & "$B\jconsole.exe" -lib j.dll "script\testga.ijs"
                if ($LASTEXITCODE -ne 0) { exit 1 }
            }
        }
        else {
            & "$B\jconsole.exe" -lib javx2.dll "script\testga.ijs"
            if ($LASTEXITCODE -ne 0) { exit 1 }
        }
        break
    }
}

