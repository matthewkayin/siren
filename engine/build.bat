REM Build script for siren engine
@ECHO OFF
SetLocal EnableDelayedExpansion

REM Get a list of all the c files
SET cFilenames=
FOR /R %%f in (*.cpp) do (
    SET cFilenames=!cFilenames! %%f
)

REM echo "Files: " %cFilenames%

SET assembly=siren
SET compilerFlags=-g -shared -Wvarargs -Wall -Werror
REM -Wall -Werror
SET includeFlags=-Isrc -I%VULKAN_SDK%/Include
SET linkerFlags=-luser32 -lvulkan-1 -L%VULKAN_SDK%/Lib
SET defines=-D_DEBUG -DSIREN_EXPORT -D_CRT_SECURE_NO_WARNINGS

ECHO "Building %assembly%..."
clang++ %cFilenames% %compilerFlags% -o ../bin/%assembly%.dll %defines% %includeFlags% %linkerFlags%