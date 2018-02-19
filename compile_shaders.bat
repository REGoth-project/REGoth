echo "Output: %BUILD_DIR%..."
REM location of script
set SCRIPT_DIR=%~dp0

set SHADER_BACKEND=glsl
set BGFX_DIR=%SCRIPT_DIR%lib\bgfx-cmake\bgfx
set BUILD_DIR=%BUILD_DIR%\shaders\%SHADER_BACKEND%
set "SHADER_FILES=%*"

REM replace slashes with backslashes
set "SHADER_BACKEND=%SHADER_BACKEND:/=\%"
set "BGFX_DIR=%BGFX_DIR:/=\%"
set "BUILD_DIR=%BUILD_DIR:/=\%"
set "SHADER_FILES=%SHADER_FILES:/=\%"

SETLOCAL ENABLEDELAYEDEXPANSION

for %%F in (%SHADER_FILES%) do (
    set TYPE=""
    set VS_FLAGS="--platform windows -p vs_4_0 -O 3"
    set FS_FLAGS="--platform windows -p ps_4_0 -O 3"
    set XS_FLAGS=""
    set FNAME=%%~nxF
    if "!FNAME:~0,2!"=="vs" (
        set TYPE=vertex
        set XS_FLAGS=%VS_FLAGS%
    )
    if "!FNAME:~0,2!"=="fs" (
        set TYPE=fragment
        set XS_FLAGS=%FS_FLAGS%
    )
    if !TYPE!=="" (
        echo "error: %%F is not a shader file"
        exit 1
    )
    if not exist %BUILD_DIR% md %BUILD_DIR%
    
    set SHADER_BIN_NAME=%%~nF.bin
    echo Building shader !FNAME! ...
    %SHADERC% !XS_FLAGS! --type !TYPE! --depends -i %BGFX_DIR%\src\ -o "%BUILD_DIR%\!SHADER_BIN_NAME!" -f "%%F" --disasm -i %BGFX_DIR%\examples\common\ || exit /b 1
    echo successful!
)
