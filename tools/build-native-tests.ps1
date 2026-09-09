param(
    [string]$Compiler = $env:PSEINT_HOST_CXX,
    [string]$CMake = $env:PSEINT_HOST_CMAKE
)
$ErrorActionPreference = 'Stop'
$workspace = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
Push-Location $workspace
try {
    if (-not $env:JAVA_HOME -or -not (Test-Path -LiteralPath (Join-Path $env:JAVA_HOME 'bin/java.exe'))) { $env:JAVA_HOME = 'C:/Program Files/Android/Android Studio/jbr' }
    if (-not $Compiler) {
        $Compiler = Join-Path $workspace 'app/build/native-tools/llvm-mingw-20260826-ucrt-x86_64/bin/clang++.exe'
    }
    if (-not (Test-Path -LiteralPath $Compiler)) { throw 'Indica -Compiler con un compilador C++17 de 64 bits (por ejemplo LLVM-MinGW).' }
    if (-not $CMake) {
        $CMake = Join-Path $env:LOCALAPPDATA 'Android/Sdk/cmake/3.22.1/bin/cmake.exe'
    }
    if (-not (Test-Path -LiteralPath $CMake)) { throw 'Indica -CMake con la ruta de cmake.exe.' }
    $ninja = Join-Path (Split-Path $CMake) 'ninja.exe'
    & ./gradlew.bat :app:preparePseintCore --console=plain
    if ($LASTEXITCODE -ne 0) { throw 'Falló la preparación de las fuentes.' }
    & $CMake -S app/src/main/cpp -B app/build/native-host -G Ninja "-DCMAKE_CXX_COMPILER=$Compiler" "-DCMAKE_MAKE_PROGRAM=$ninja" "-DJAVA_HOME=$env:JAVA_HOME" -DCMAKE_BUILD_TYPE=Release
    if ($LASTEXITCODE -ne 0) { throw 'Falló CMake.' }
    & $CMake --build app/build/native-host -j 2
    if ($LASTEXITCODE -ne 0) { throw 'Falló la compilación del motor para las pruebas.' }
} finally { Pop-Location }
