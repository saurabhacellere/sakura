:: �r���h�c�[���̃p�X��������
:: 
:: ===7z.exe�̃p�X��T������菇===
:: 1. CMD_7Z���Z�b�g����Ă���Η��p����
:: 2. �p�X���ʂ��Ă���΂�����g��
:: 3. �f�t�H���g�̃C���X�g�[���p�X�Ō�����΂�����g��
:: 4. 1�`3�Ō�����Ȃ����CMD_7Z���폜����

@echo off

if defined FIND_TOOLS_CALLED (
    echo find-tools.bat already called
    exit /b
)

:: CMD_*����`����Ă��Ȃ���ΒT�����[�`����
echo find-tools.bat
if not defined CMD_7Z call :7z 2> nul
if not defined CMD_HHC call :hhc 2> nul
if not defined CMD_ISCC call :iscc 2> nul
if not defined CMD_CPPCHECK call :cppcheck 2> nul
if not defined CMD_DOXYGEN call :doxygen 2> nul
if not defined CMD_MSBUILD call :msbuild 2> nul
echo ���� CMD_7Z=%CMD_7Z%
echo ���� CMD_HHC=%CMD_HHC%
echo ���� CMD_ISCC=%CMD_ISCC%
echo ���� CMD_CPPCHECK=%CMD_CPPCHECK%
echo ���� CMD_DOXYGEN=%CMD_DOXYGEN%
echo ���� CMD_MSBUILD=%CMD_MSBUILD%
set FIND_TOOLS_CALLED=1
exit /b

:7z
setlocal
PATH=%PATH%;%ProgramFiles%\7-Zip\;%ProgramFiles(x86)%\7-Zip\;%ProgramW6432%\7-Zip\;
:: where�̏o�͂�1�s�ڂ�CMD_7Z�ɑ��
for /f "usebackq delims=" %%a in (`where 7z`) do ( 
    endlocal && set "CMD_7Z=%%a"
    exit /b
)
endlocal
exit /b

:hhc
setlocal
PATH=%PATH%;%ProgramFiles%\HTML Help Workshop\;%ProgramFiles(x86)%\HTML Help Workshop\;%ProgramW6432%\HTML Help Workshop\;
for /f "usebackq delims=" %%a in (`where hhc.exe`) do ( 
    endlocal && set "CMD_HHC=%%a"
    exit /b
)
endlocal
exit /b

:iscc
setlocal
PATH=%PATH%;%ProgramFiles%\Inno Setup 5\;%ProgramFiles(x86)%\Inno Setup 5\;%ProgramW6432%\Inno Setup 5\;
for /f "usebackq delims=" %%a in (`where ISCC.exe`) do ( 
    endlocal && set "CMD_ISCC=%%a"
    exit /b
)
endlocal
exit /b

:cppcheck
setlocal
PATH=%PATH%;%ProgramFiles%\cppcheck\;%ProgramFiles(x86)%\cppcheck\;%ProgramW6432%\cppcheck\;
for /f "usebackq delims=" %%a in (`where cppecheck.exe`) do ( 
    endlocal && set "CMD_CPPCHECK=%%a"
    exit /b
)
endlocal
exit /b

:doxygen
setlocal
PATH=%PATH%;%ProgramFiles%\doxygen\bin\;%ProgramFiles(x86)%\doxygen\bin\;%ProgramW6432%\doxygen\bin\;
for /f "usebackq delims=" %%a in (`where doxygen.exe`) do ( 
    endlocal && set "CMD_DOXYGEN=%%a"
    exit /b
)
endlocal
exit /b

:msbuild
:: ref https://github.com/Microsoft/vswhere
setlocal
PATH=%PATH%;%ProgramFiles%\Microsoft Visual Studio\Installer\;%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\;%ProgramW6432%\Microsoft Visual Studio\Installer\;
for /f "usebackq tokens=*" %%i in (`vswhere -latest -products * -requires Microsoft.Component.MSBuild -property installationPath`) do (
    endlocal && set "CMD_MSBUILD=%%i\MSBuild\15.0\Bin\MSBuild.exe"
    exit /b
)
endlocal
exit /b
