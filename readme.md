# Siren Game Engine

Siren is a 3D game engine developed in C++ / OpenGL for my own educational purposes. My current goal for the project is to develop a clone of the game Portal, which I hope will guide the creation of engine features. The idea is that if it is possible to implement Portal in the engine, then that means the engine is "feature complete" to a certain degree. I also just think it will be neat to make a Portal clone.

## Setting up the Engine
Currently only Windows is supported. MacOS support is planned for the future.
### Windows
**Pre-requisites:**
- Clang (https://releases.llvm.org/download.html)
- Visual Studio Community, which acts as a backend for Clang (https://visualstudio.microsoft.com/vs/community/)
- Make for Windows (https://gnuwin32.sourceforge.net/packages/make.htm)
- Git for Windows (https://gitforwindows.org/)

**Setup instructions:**

First, clone the repo and run the `build-all.bat` script.
```
git clone https://github.com/matthewkayin/siren.git
cd siren
build-all.bat
```
This should create an `engine.dll` file in the `bin` folder. Link your game against this library to create a game with the engine.

## Sample application

```cpp
#include <core/application.h>
#include <math/math.h>
#include <cstdio>

// Called when the application starts. Use to initialize your game state.
bool game_init() {
    return true;
}

// Called whenever the application updates. delta is the time elapsed between frames in seconds.
bool game_update(float delta) {
    return true;
}

// Called whenever the application renders.
bool game_render() {
    return true;
}

// The above functions return a boolean value to allow users to force their application to quit in the case of an error.

int main() {
    siren::ApplicationConfig config = (siren::ApplicationConfig) {
        .name = "Your Game Name",
        // Screen size is the game's internal resolution
        .screen_size = siren::ivec2(1280, 720),
        // Window size is the actual size of the window
        .window_size = siren::ivec2(1280, 720),

        // Used as a base path in loading resources
        .resource_path = "./res/",

        // The engine accepts function pointers for your init, update, and render function
        .init = &game_init,
        .update = &game_update,
        .render = &game_render
    };

    // This will return false if something bad happens
    // It will also return false if the ApplicationConfig init() returns false
    if (!siren::application_create(config)) {
        printf("Applicaiton failed to create!\n");
        return -1;
    }

    // This will return false if something bad happens
    // It will also return false if the ApplicationConfig update() or render() returns false
    if (!siren::application_run()) {
        printf("Application did not quit gracefully.\n);
        return -2;
    }

    return 0;
}
```