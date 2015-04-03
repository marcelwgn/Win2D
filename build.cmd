@echo off

IF "%VisualStudioVersion%" LSS "12.0" (
    ECHO Please run this script from a Visual Studio 2013 Command Prompt.
    PAUSE
    GOTO END
)

WHERE /Q msbuild >NUL
IF %ERRORLEVEL% NEQ 0 ( 
    ECHO Error: It appears that 'msbuild' is not available in this environment. 
    ECHO Please run this script from a Visual Studio 2013 Command Prompt.
    GOTO END
)

msbuild "%~dp0Win2D.proj" /v:m /maxcpucount /p:BuildTests=false /p:BuildTools=false /p:BuildDocs=false /p:BuildUAP=false

IF %ERRORLEVEL% NEQ 0 (
    ECHO Build failed; aborting.
    GOTO END
)

msbuild "%~dp0tools\docs\BuildDocs.proj" /nologo /v:m /p:IntellisenseOnly=true

IF %ERRORLEVEL% NEQ 0 (
    ECHO Build failed; aborting.
    GOTO END
)

ECHO.

SETLOCAL
SET OVERRIDE_NUGET_PACKAGE=

CALL "%~dp0build\nuget\build-nupkg.cmd" local

:END