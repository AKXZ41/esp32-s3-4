@echo off
echo ==============================
echo ESP-IDF 环境检查工具
echo ==============================
echo.

echo 1. 检查IDF_PATH环境变量...
if defined IDF_PATH (
    echo   ✓ IDF_PATH已设置: %IDF_PATH%
    if exist "%IDF_PATH%" (
        echo   ✓ ESP-IDF目录存在
    ) else (
        echo   ✗ ESP-IDF目录不存在
    )
) else (
    echo   ✗ IDF_PATH未设置
)
echo.

echo 2. 检查常见ESP-IDF安装位置...
if exist "C:\esp-idf" (
    echo   ✓ 找到ESP-IDF安装: C:\esp-idf
    set FOUND_IDF=C:\esp-idf
)
if exist "C:\Users\%USERNAME%\esp\esp-idf" (
    echo   ✓ 找到ESP-IDF安装: C:\Users\%USERNAME%\esp\esp-idf
    set FOUND_IDF=C:\Users\%USERNAME%\esp\esp-idf
)
if exist "C:\espressif\esp-idf" (
    echo   ✓ 找到ESP-IDF安装: C:\espressif\esp-idf
    set FOUND_IDF=C:\espressif\esp-idf
)
echo.

echo 3. 检查Python环境...
python --version 2>nul
if %ERRORLEVEL% EQU 0 (
    echo   ✓ Python可用
) else (
    echo   ✗ Python不可用
)
echo.

echo 4. 检查idf.py命令...
idf.py --version 2>nul
if %ERRORLEVEL% EQU 0 (
    echo   ✓ idf.py命令可用
) else (
    echo   ✗ idf.py命令不可用
)
echo.

echo 5. 检查项目文件...
if exist "CMakeLists.txt" (
    echo   ✓ 项目根目录CMakeLists.txt存在
) else (
    echo   ✗ 项目根目录CMakeLists.txt不存在
)

if exist "main\CMakeLists.txt" (
    echo   ✓ main目录CMakeLists.txt存在
) else (
    echo   ✗ main目录CMakeLists.txt不存在
)

if exist "sdkconfig" (
    echo   ✓ sdkconfig文件存在
) else (
    echo   ✗ sdkconfig文件不存在
)
echo.

echo ==============================
echo 解决建议:
echo ==============================
if defined FOUND_IDF (
    echo 1. 运行ESP-IDF环境设置脚本:
    echo    %FOUND_IDF%\export.bat
    echo.
    echo 2. 然后重新运行此脚本检查环境
    echo.
    echo 3. 环境设置成功后运行:
    echo    idf.py build
) else (
    echo 1. 请确认ESP-IDF 5.5已正确安装
    echo 2. 运行ESP-IDF安装目录下的export.bat
    echo 3. 或使用ESP-IDF命令提示符
)
echo.

pause
