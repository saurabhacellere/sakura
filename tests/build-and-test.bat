set platform=%1
set configuration=%2
set ERROR_RESULT=0

if "%platform%" == "MinGW" (
	@echo   test for MinGW will be skipped. ^(platform: %platform%, configuration: %configuration%^)
	exit /b 0
)

@echo ---- start create-project.bat ----
call %~dp0create-project.bat %platform% %configuration%
if errorlevel 1 (
	@echo ERROR in create-project.bat %errorlevel%
	exit /b %errorlevel%
)
@echo ---- end   create-project.bat ----

@echo ---- start build-project.bat ----
call %~dp0build-project.bat %platform% %configuration%
if errorlevel 1 (
	@echo ERROR in build-project.bat %errorlevel%
	exit /b %errorlevel%
)
@echo ---- end   build-project.bat ----

@echo ---- start run-tests.bat ----
call %~dp0run-tests.bat %platform% %configuration%
if errorlevel 1 (
	@echo ERROR in run-tests.bat %errorlevel%
	exit /b %errorlevel%
)
@echo ---- end   run-tests.bat ----
