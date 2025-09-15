@echo off
echo ==============================
echo 项目诊断工具
echo ==============================
echo.

echo 1. 检查项目结构...
if exist "CMakeLists.txt" (
    echo   ✓ 根目录CMakeLists.txt存在
) else (
    echo   ✗ 根目录CMakeLists.txt不存在
)

if exist "main\CMakeLists.txt" (
    echo   ✓ main\CMakeLists.txt存在
) else (
    echo   ✗ main\CMakeLists.txt不存在
)

if exist "main\main.c" (
    echo   ✓ main\main.c存在
) else (
    echo   ✗ main\main.c不存在
)

if exist "sdkconfig" (
    echo   ✓ sdkconfig存在
) else (
    echo   ✗ sdkconfig不存在
)
echo.

echo 2. 检查组件目录...
if exist "components" (
    echo   ✓ components目录存在
    echo   检查组件:
    
    if exist "components\Data_declaration" (
        echo     ✓ Data_declaration组件存在
    ) else (
        echo     ✗ Data_declaration组件不存在
    )
    
    if exist "components\flight_control" (
        echo     ✓ flight_control组件存在
    ) else (
        echo     ✗ flight_control组件不存在
    )
    
    if exist "components\sensor" (
        echo     ✓ sensor组件存在
    ) else (
        echo     ✗ sensor组件不存在
    )
    
    if exist "components\inside_communication" (
        echo     ✓ inside_communication组件存在
    ) else (
        echo     ✗ inside_communication组件不存在
    )
    
    if exist "components\External_communication" (
        echo     ✓ External_communication组件存在
    ) else (
        echo     ✗ External_communication组件不存在
    )
    
    if exist "components\LED" (
        echo     ✓ LED组件存在
    ) else (
        echo     ✗ LED组件不存在
    )
    
    if exist "components\VBAT" (
        echo     ✓ VBAT组件存在
    ) else (
        echo     ✗ VBAT组件不存在
    )
) else (
    echo   ✗ components目录不存在
)
echo.

echo 3. 检查可能的兼容性问题...
echo   检查main.c中的头文件包含:
findstr /n "unistd.h" main\main.c 2>nul
if %ERRORLEVEL% EQU 0 (
    echo     ✗ 发现unistd.h包含 - 需要修复
) else (
    echo     ✓ 未发现unistd.h包含
)

findstr /n "sys/time.h" main\main.c 2>nul
if %ERRORLEVEL% EQU 0 (
    echo     ✗ 发现sys/time.h包含 - 需要修复
) else (
    echo     ✓ 未发现sys/time.h包含
)
echo.

echo 4. 检查构建目录...
if exist "build" (
    echo   ✓ build目录存在
    echo   清理构建目录可能有助于解决构建问题
) else (
    echo   - build目录不存在（正常，首次构建时会创建）
)
echo.

echo ==============================
echo 建议的解决步骤:
echo ==============================
echo 1. 确保在ESP-IDF环境中运行构建命令
echo 2. 如果仍有问题，尝试清理构建目录:
echo    idf.py fullclean
echo 3. 然后重新构建:
echo    idf.py build
echo 4. 如果还有错误，请提供具体的错误信息
echo.

pause
