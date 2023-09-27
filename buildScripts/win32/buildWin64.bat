@echo off

::create current dir variable
SET CURRENT_DIR=%CD%
echo %CURRENT_DIR%

::remove old dirs
rmdir /Q /S %CURRENT_DIR%\build_x86_64
rmdir /Q /S %CURRENT_DIR%\install_x86_64
rmdir /Q /S %CURRENT_DIR%\deploy_x86_64

::create new dirs
mkdir %CURRENT_DIR%\build_x86_64
mkdir %CURRENT_DIR%\install_x86_64
mkdir %CURRENT_DIR%\deploy_x86_64

::set dirs variables
SET BUILD_DIR=%CURRENT_DIR%\build_x86_64
SET INSTALL_DIR=%CURRENT_DIR%\install_x86_64
SET DEPLOY_DIR=%CURRENT_DIR%\deploy_x86_64

::create qt/cmake/make variables
SET TRANSLATION_PATH="C:/Qt/Qt5.14.2/5.14.2/msvc2017_64/translations"
SET QMAKE_PATH="C:/Qt/Qt5.14.2/5.14.2/msvc2017_64/bin/qmake.exe"
SET Qt5_DIR="C:/Qt/Qt5.14.2/5.14.2/msvc2017_64/lib/cmake/Qt5"
SET CMAKE_PREFIX_PATH="C:/Qt/Qt5.14.2/5.14.2/msvc2017_64"
SET CMAKE_PATH="C:/Program Files/CMake/bin/cmake.exe"
SET MAKE_PATH="C:/Qt/Ninja/ninja.exe"

::vs options
SET CMAKE_C_COMPILER="C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Tools/MSVC/14.16.27023/bin/Hostx64/x64/cl.exe"
SET CMAKE_CXX_COMPILER="C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Tools/MSVC/14.16.27023/bin/Hostx64/x64/cl.exe"
SET VC_VARS_PATH_BAT="C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Auxiliary/Build/vcvars64.bat"
SET VC_REDISTR_DIR="C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Redist/MSVC/14.16.27012"

::openssl options
SET  OPENSSL_ROOT_DIR="C:/Program Files/OpenSSL-Win64"
SET  OPENSSL_LIBRARIES="C:/Program Files/OpenSSL-Win64/lib"

::call vs script
call %VC_VARS_PATH_BAT%

::change dir to 'build'
cd %BUILD_DIR%

::run cmake
%CMAKE_PATH% ../../../ -GNinja^
 -DCMAKE_BUILD_TYPE:String=Release^
 -DCMAKE_INSTALL_PREFIX=%INSTALL_DIR%^
 -DQt5_DIR=%Qt5_DIR%^
 -DQT_QMAKE_EXECUTABLE:STRING=%QMAKE_PATH%^
 -DCMAKE_MAKE_PROGRAM=%MAKE_PATH%^
 -DCMAKE_PREFIX_PATH:STRING=%CMAKE_PREFIX_PATH%^
 -DCMAKE_C_COMPILER:STRING=%CMAKE_C_COMPILER%^
 -DCMAKE_CXX_COMPILER:STRING=%CMAKE_CXX_COMPILER%^
 -DOPENSSL_ROOT_DIR=%OPENSSL_ROOT_DIR%^
 -DOPENSSL_LIBRARIES=%OPENSSL_LIBRARIES%

%CMAKE_PATH% --build . --parallel --target all
%CMAKE_PATH% --install .

::deployer options
SET CQTDEPLOYER_DIR="C:/Qt/CQtDeployer/1.5"

::deploy
call %CQTDEPLOYER_DIR%\cqtdeployer.bat -qmake %QMAKE_PATH% -bin %INSTALL_DIR%\bin\Client.exe -targetDir %DEPLOY_DIR%\bin noTranslations
call %CQTDEPLOYER_DIR%\cqtdeployer.bat -qmake %QMAKE_PATH% -bin %INSTALL_DIR%\bin\Simulator.exe -targetDir %DEPLOY_DIR%\bin noTranslations

::cd to -up- dir
cd ..

::remove 'build' and 'install' dirs
rmdir /Q /S %CURRENT_DIR%\build_x86_64
rmdir /Q /S %CURRENT_DIR%\install_x86_64

pause
exit
