@echo off

SETLOCAL

:�����œn���ꂽ�t�@�C���Q�̃p�X��؂��u������del�R�}���h�ɓn��
set OUTFILES=%*
del /F /Q %OUTFILES:/=\%

ENDLOCAL
exit /b
