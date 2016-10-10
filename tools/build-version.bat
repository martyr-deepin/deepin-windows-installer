for /f %%i in ('git rev-parse HEAD') do set GitVersion=%%i

set ENV_HEADER=%1

ECHO.>"%ENV_HEADER%"
ECHO #ifndef BUILD_ENV_H>>"%ENV_HEADER%"
ECHO #define BUILD_ENV_H>>"%ENV_HEADER%"
ECHO #define VER_GIT_COMMITID_STR %GitVersion%>>"%ENV_HEADER%"
ECHO #endif // BUILD_ENV_H>>"%ENV_HEADER%"
