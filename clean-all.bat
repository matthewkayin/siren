@ECHO OFF
REM Clean Everything

ECHO "Cleaning everything..."

REM Engine
make -f "makefile.library.windows.mak" clean ASSEMBLY="engine"
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

REM Testbed
make -f "makefile.executable.windows.mak" clean ASSEMBLY="sandbox"
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

ECHO "All assemblies cleaned successfully."