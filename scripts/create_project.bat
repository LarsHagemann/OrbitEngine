@echo off
:: Project Creation Batch Script
set project_name=%1

echo Building project '%project_name%'
if exist %project_name% (
	echo Project '%project_name%' does already exist.
	goto lbl_end
) else (
	mkdir %project_name%
	mkdir %project_name%\assets
	mkdir %project_name%\assets\textures
	mkdir %project_name%\assets\orbs
	mkdir %project_name%\assets\models
	
	pushd __general\shader\Raw
	call build.bat
	popd
	
	xcopy __general %project_name% /E
	
	powershell -Command "(gc %project_name%\source\main.cpp) -replace '###PROJECT_NAME###', '%project_name%' | Out-File -encoding ASCII %project_name%\source\main.cpp"
	powershell -Command "(gc %project_name%\source\CMakeLists.txt) -replace '###PROJECT_NAME###', '%project_name%' | Out-File -encoding ASCII %project_name%\source\CMakeLists.txt"
	
	cmake -B %project_name%\build -S %project_name%\source
	
	start %project_name%\build\%project_name%.sln
)

:lbl_end
