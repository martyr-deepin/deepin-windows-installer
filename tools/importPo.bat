@ECHO off

SET PATH=D:\Dev\QtStatic\5.3\vs2010\bin\;%PATH%

lupdate.exe %1

lrelease.exe %1
