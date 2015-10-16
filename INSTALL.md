# Installation

If you want binary install, please see [README](README.md).

## Windows Platform

### Development Environment

- Windows 7 64bit
- Microsoft Visual C++ 2010
- DirectX SDK
- Qt5.4

### Build Qt static

```
set DXSDK_DIR="C:\Program Files (x86)\Windows Kits\8.0\Include\um"
configure -prefix "C:\Qt\QtStatic\5.3\vs2010" -release -platform win32-msvc2010 \
-no-qml-debug -confirm-license -opensource -static -qt-pcre -no-icu -no-sql-sqlite \
-no-nis -no-cups -no-iconv -no-dbus -nomake examples -no-sql-odbc -no-compile-examples \
-skip qtwebkit -skip qtwebkit-examples -skip qtactiveqt -no-openssl -qt-zlib \
-qt-libpng -qt-freetype -qt-libjpeg -opengl es2 -angle
```

### Build Deepin System Installer
```
qmake.exe -r deepin-system-installer.pro
nmake
```
