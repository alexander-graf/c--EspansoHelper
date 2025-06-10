@echo off
echo Copying Qt6 DLLs...
copy "C:\Qt\6.9.1\msvc2022_64\bin\Qt6Core.dll" build\Release\
copy "C:\Qt\6.9.1\msvc2022_64\bin\Qt6Gui.dll" build\Release\
copy "C:\Qt\6.9.1\msvc2022_64\bin\Qt6Widgets.dll" build\Release\
echo Copying Qt6 plugins...
xcopy "C:\Qt\6.9.1\msvc2022_64\plugins\platforms" build\Release\platforms\ /E /I /Y
echo Done!
pause 