@echo off
setlocal enabledelayedexpansion
chcp 65001 >nul 2>nul

rem --- UAC Elevation ---
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo [UAC] Demande d'elevation administrateur...
    powershell -Command "Start-Process '%~f0' -Verb RunAs"
    exit /b 0
)

set "ROOT=%~dp0"
set "DLL_PATH=%ROOT%build\ida_plugins\Release\ida_discord_rpc.dll"
set "FOUND=0"

echo         IDA RPC SETUP
echo ============================================
echo.

echo [BUILD] Compilation de la DLL...
if not exist "%ROOT%build\CMakeCache.txt" (
    echo   Configuration CMake...
    cmake -B "%ROOT%build" -G "Visual Studio 17 2022" -A x64 "-DCMAKE_POLICY_VERSION_MINIMUM=3.5" >nul 2>&1
    if !errorlevel! neq 0 (
        echo   [ERREUR] Echec configuration CMake
        pause
        exit /b 1
    )
)

cmake --build "%ROOT%build" --config Release
if %errorlevel% neq 0 (
    echo [ERREUR] Echec compilation
    pause
    exit /b 1
)
echo   [OK] DLL generee

if not exist "%DLL_PATH%" (
    echo [ERREUR] DLL introuvable apres build
    pause
    exit /b 1
)

echo.

echo [CLEAN] Suppression des anciennes DLLs...
for %%p in (
    "%APPDATA%\Hex-Rays\IDA Pro\plugins"
    "%APPDATA%\Hex-Rays\IDA\plugins"
    "%APPDATA%\Hex-Rays\IDA Pro\plugins64"
    "C:\Program Files\IDA Professional 9.3\plugins"
) do (
    if exist "%%~p\ida_discord_rpc.dll" (
        del /F /Q "%%~p\ida_discord_rpc.dll" >nul 2>&1
        echo   [DEL] %%~p
    )
)
for %%r in ("%ProgramFiles%" "%ProgramFiles(x86)%") do (
    for /d %%d in ("%%~r\IDA*") do (
        if exist "%%d\plugins\ida_discord_rpc.dll" (
            del /F /Q "%%d\plugins\ida_discord_rpc.dll" >nul 2>&1
            echo   [DEL] %%d\plugins\
        )
    )
)

echo.

rem --- 1. AppData Roaming ---
echo [1/3] AppData\Hex-Rays...
for %%p in (
    "%APPDATA%\Hex-Rays\IDA Pro\plugins"
    "%APPDATA%\Hex-Rays\IDA\plugins"
    "%APPDATA%\Hex-Rays\IDA Pro\plugins64"
) do (
    if exist "%%~p\" (
        copy /Y "%DLL_PATH%" "%%~p\" >nul
        if !errorlevel! equ 0 (
            echo   [OK] %%~p
            set "FOUND=1"
        )
    )
)

rem --- 2. Program Files ---
echo [2/3] Program Files...
for %%r in ("%ProgramFiles%" "%ProgramFiles(x86)%") do (
    for /d %%d in ("%%~r\IDA*") do (
        if exist "%%d\plugins\" (
            copy /Y "%DLL_PATH%" "%%d\plugins\" >nul
            if !errorlevel! equ 0 (
                echo   [OK] %%d\plugins\
                set "FOUND=1"
            )
        )
    )
)

rem --- 3. Recherche par ida.exe ---
echo [3/3] Recherche par executable...
for %%s in ("%ProgramFiles%" "%ProgramFiles(x86)%") do (
    for %%f in (ida.exe ida64.exe) do (
        for /f "delims=" %%d in ('dir /s /b "%%~s\%%f" 2^>nul') do (
            set "PLUGIN_DIR=%%~dpdplugins"
            if exist "!PLUGIN_DIR!\" (
                copy /Y "%DLL_PATH%" "!PLUGIN_DIR!\" >nul
                if !errorlevel! equ 0 (
                    echo   [OK] !PLUGIN_DIR!
                    set "FOUND=1"
                )
            )
        )
    )
)

echo.
if "%FOUND%"=="0" (
    echo [ERREUR] Aucune installation IDA trouvee.
    echo Copie manuelle :
    echo   copy /Y "%DLL_PATH%" "%%APPDATA%%\Hex-Rays\IDA Pro\plugins\"
) else (
    echo [SUCCES] Installation terminee ! Redemarre IDA.
)

echo.
pause
