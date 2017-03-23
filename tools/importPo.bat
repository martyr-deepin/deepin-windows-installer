@ECHO off

SET PATH=D:\Develop\Qt\Qt5.8.0\5.8\mingw53_32\bin\;%PATH%

lupdate.exe %1


lrelease.exe %1
