@ECHO OFF
REM Build Everything

ECHO "Building everything..."

REM Engine
make -f "makefile.library.windows.mak" all ASSEMBLY="engine" ADDL_INC_FLAGS="-Iengine\include" ADDL_LINK_FLAGS="-luser32 -lSDL2 -lSDL2_ttf -Lengine/lib/windows"
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

REM Sandbox
make -f "makefile.executable.windows.mak" all ASSEMBLY="sandbox" ADDL_INC_FLAGS="-Iengine/src" ADDL_LINK_FLAGS=""
IF %ERRORLEVEL% NEQ 0 (echo Error:%ERRORLEVEL% && exit)

ECHO "All assemblies built successfully."