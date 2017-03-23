ECHO on
SET PATH=D:\Develop\Qt\Qt5.8.0\5.8\mingw53_32\bin\;%PATH%
SET PROFILE=%1
SET ProName=%PROFILE:~0,-4%
SET TsFile=po/%ProName%.ts
SET PotFileName=deepin-windows-installer.pot
lupdate.exe %1 -target-language en_US -ts %TsFile%
lconvert.exe -i %TsFile% -if ts -o po\%PotFileName% -of po
rm %TsFile%
