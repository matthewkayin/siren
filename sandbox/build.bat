REM Build script for siren sandbox
@ECHO OFF
SetLocal EnableDelayedExpansion

REM Get a list of all the c files
SET cFilenames=
FOR /R %%f in (*.cpp) do (
    SET cFilenames=!cFilenames! %%f
)

REM echo "Files: " %cFilenames%

SET assembly=sandbox
SET compilerFlags=-g 
REM -Wall -Werror
SET includeFlags=-Isrc -I../engine/src/
SET linkerFlags=-L../bin/ -lsiren.lib
SET defines=-D_DEBUG -DSIREN_EXPORT 

ECHO "Building %assembly%..."
clang++ %cFilenames% %compilerFlags% -o ../bin/%assembly%.exe %defines% %includeFlags% %linkerFlags%