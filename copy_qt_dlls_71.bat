@echo off
echo Copying Qt DLLs for Espanso Helper with 71.ico icon...

REM Copy Qt DLLs to the build directory
copy "C:\Qt\6.5.3\msvc2019_64\bin\Qt6Core.dll" "build\Debug\"
copy "C:\Qt\6.5.3\msvc2019_64\bin\Qt6Widgets.dll" "build\Debug\"
copy "C:\Qt\6.5.3\msvc2019_64\bin\Qt6Gui.dll" "build\Debug\"

REM Copy platform plugin
mkdir "build\Debug\platforms" 2>nul
copy "C:\Qt\6.5.3\msvc2019_64\plugins\platforms\qwindows.dll" "build\Debug\platforms\"

echo Done! You can now run espanso_helper.exe from the build\Debug directory.
pause 