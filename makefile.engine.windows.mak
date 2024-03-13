DIR := $(subst /,\,${CURDIR})
BUILD_DIR := bin
OBJ_DIR := obj
LIB_DIR := engine/lib/windows

ASSEMBLY := engine
EXTENSION := .dll
COMPILER_FLAGS := -g -fdeclspec #-fPIC
INCLUDE_FLAGS := -Iengine\src -Iengine/include
LINKER_FLAGS := -g -shared -luser32 -L$(LIB_DIR) -lSDL2 -lSDL2_ttf -lassimp-vc143-mtd -L$(OBJ_DIR)\engine
DEFINES := -DSIREN_DEBUG -DSIREN_EXPORT -D_CRT_SECURE_NO_WARNINGS

# Make does not offer a recursive wildcard function, so here's one:
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

SRC_FILES := $(call rwildcard,$(ASSEMBLY)/src/,*.cpp) # Get all .c files
DIRECTORIES := \$(ASSEMBLY)\src $(subst $(DIR),,$(shell dir $(ASSEMBLY)\src /S /AD /B | findstr /i src)) # Get all directories under src.
OBJ_FILES := $(SRC_FILES:%=$(OBJ_DIR)/%.o) # Get all compiled .c.o objects for engine

all: scaffold compile link

.PHONY: scaffold
scaffold: # create build directory
	@echo Scaffolding folder structure...
	-@setlocal enableextensions enabledelayedexpansion && mkdir $(addprefix $(OBJ_DIR), $(DIRECTORIES)) 2>NUL || cd .
	-@setlocal enableextensions enabledelayedexpansion && mkdir $(BUILD_DIR) 2>NUL || cd .
	@echo Done.

.PHONY: link
link: scaffold $(OBJ_FILES) # link
	@echo Linking $(ASSEMBLY)...
	@clang++ $(OBJ_FILES) -o $(BUILD_DIR)\$(ASSEMBLY)$(EXTENSION) $(LINKER_FLAGS)

.PHONY: compile
compile: #compile .c files
	@echo Compiling...

.PHONY: clean
clean: # clean build directory
	if exist $(BUILD_DIR)\$(ASSEMBLY)$(EXTENSION) del $(BUILD_DIR)\$(ASSEMBLY)$(EXTENSION)
	rmdir /s /q $(OBJ_DIR)\$(ASSEMBLY)

.PHONY: libcopy
libcopy: # note, not using force copy so it should only copy libs over once
	@echo Copying libaries...
	cp -r engine/lib/windows/* bin

$(OBJ_DIR)/%.cpp.o: %.cpp # compile .c to .c.o object
	@echo   $<...
	@clang++ $< $(COMPILER_FLAGS) -c -o $@ $(DEFINES) $(INCLUDE_FLAGS)