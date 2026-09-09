# Chip8 Emulator

Built from scratch, this repo is a chip8 emulator I built circa 2018.

You'll find all of the specs for this, let's say "virtual cpu", [here](https://en.wikipedia.org/wiki/CHIP-8).

## Quick Start

To quickly get up and running make sure you are on MacOS, then:

1. Run `make mac` to build an executable;

2. Then simply `make run` to start the emulator.

Et voilà!

## Revamping this Old Repo

As I allured to before, I built this emulator back in 2018 and at the time, to be very honest, I did know quite a lot about the ins and outs of raw coding, but I wasn't very experienced with package management, version control or build systems. That led the repo to be very unfriendly, since I committed a bunch of specific visual studio unnecessary garbage, and no instructions in sight on how to even run the thing. Ever since I've been playing a lot with C and decided to revamp the repo. Mainly: clean up unnecessary files, give it a more intuitive structure and lastly, give it reproducible steps. What follows is the steps I took to turn this repo from a Windows / Visual Studio project into an easily runnable codebase on MacOS.

1. After reopening the thing and browsing through a lot of garbage files / folders, I quickly noticed the core C++ source files I cared about: `Chip8.cpp`, `Chip8.h` and `Chip8Emulator.cpp`;

2. I wasn't exactly sure of what I needed to do next to get this thing up and running, but one thing I was certain: from my previous experiments with C, it could not be THAT complicated to start attempting compilation, and with that, I quickly just decided to do something very naive... a simple build of the main file:

```
clang++ -o chip8 Chip8Emulator.cpp
```

3. Obviously I knew the code relied on SDL to handle graphics and keyboard and thus, due to a very crude command, the compilation would break and.. it did. Which is fine, I knew I had relied heavily on Visual Studio to do the heavy lifting of dependency management and build for me (and ofc I was on windows) so I didn't want to reuse much, it shouldn't be that hard to bring in my own version of SDL for MacOS this time.... and, then it wasn't. I quickly realized at the time I used SDL version 2 and found binaries for it here:

https://github.com/libsdl-org/SDL/releases/tag/release-2.32.0

4. Ofc I quickly noticed a `dmg` artifact on the list, but before simply accepting the situation I went on a little crusade to understand libraries (and in this case dynamic libraries) on MacOS and found out that in the apple ecosystem, things are distributed as _frameworks_. Which is nothing more than a distribution that bundles the libraries headers and object files (for compilation and linkage). In this case, I could've simply installed the `dmg` mentioned earlier, and it'd be available system-wide for me, but for teaching purposes I decided to include it in the repo and resolve it locally so I could fully understand what was going on. Download the `dmg` I did, then I moved it into the folder:

```
└── packages/
    ├── mac/
    │   ├── SDL2.framework <------- The mac binary for SDL went here
    └── linux/
    └── windows/           // In the future I can place binaries for other platforms next to it
```

5. Okay, now I have the _framework_ (or the library binaries), what now? Ofc I needed to pass it during compilation, so my code could be resolved against it, and so I found out how to point the compiler to a folder of libraries and then, within that folder, instruct it which ones to lookup for:

```
clang++ -o chip8 Chip8Emulator.cpp -Fpackages/mac -framework SDL2
```

The `-F` parameter instructs which folder to lookup for libraries, while on apple compilers, the `-framework` flag exists to specifically instruct it to load certain libraries. In this case we only pass the libraries prefix (remember when we download SDL it extracted into a folder called `SDL2.framework`... this flag is what ties it together).

6. Still to no avail things failed. When including the library, previously I only included `#include <SDL.h>` which on apple's platform won't resolve since we are not specifying from which library (or framework) to load the header from... easy fix, just write it as:

```
#include <SDL2/SDL.h>
```

Where `SDL2/*` in MacOS land will lookup a linked framework of the same name (the `-framework` flag we used earlier).

7. And then..... something happened and the errors shifted away from SDL and into something very obvious. I was not passing the Chip8.cpp source file! Which is fine, that's literally what I wanted, to break through the barrier of bringing a third party library and now only having to deal with my own code wiring. Thus:

```
clang++ -o chip8 Chip8Emulator.cpp Chip8.cpp -Fpackages/mac -framework SDL2
```

Got me to the next step....

8. By now I could compile successfully, I felt very confident, and then, during runtime...

```
Library not loaded: @rpath/SDL2.framework/Versions/A/SDL2
```

The last piece of the puzzle came at runtime, to remind me why SDL was being distributed as a dynamically linked library... sure, my code has been compiled fine, but during runtime, I was simply not providing it the actual library it required. My compilation step was never bundling the thirdparty code in it, simply making sure all the linkage happened fine such that during runtime things could be properly discovered.... and since I wanted to make an app that felt self-sufficient (not relying on system-wide installations and the like), I had to provide during compilation time where the app was supposed to look for dynamic loaded libraries, and then the final command came to life:

```
clang++ -o chip8 Chip8Emulator.cpp Chip8.cpp -Fpackages/mac -framework SDL2 -Wl,-rpath,@executable_path/packages/mac
```

The wiring looks a bit more complicating, but in short the `-rpath` param passes a relative path where the final executable should lookup for dynamic libraries. Prepending `@executable_path` to `packages/mac` means that, alongside the executable we should place a folder called `packages/mac` that contains the libraries we require (in this instance, SDL2).

And with that... finally, voilà! I had the emulator running once more, and more importantly, through a very simplistic Makefile containing only a couple of ultra simple recipes to get it done.

## Immediate Improvements

1. First act of improvement (well, ofc, right after the whole revamping story I just told) was renaming the source files to something more friendly and more portable, ie no capitalization (`main.cpp`, `chip8.cpp`, etc, instead of the things we saw during the revamp, eg `Chip8Emulator.cpp`); and `.h` became `.hpp`;

2. Second and a huge win, the emulator's framebuffer is 64x32 and as you can imagine that resulted in a tiiiny window.. luckily SDL allows us to scale the window independently from the framebuffer size, so by setting this config:

```
SDL_RenderSetLogicalSize(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
```

Allowed me to multiply the window size by 10 (or any value actually):

```
SDL_CreateWindowAndRenderer(WINDOW_WIDTH * 10, WINDOW_HEIGHT * 10, 0, &window, &renderer);
```

Bringing us to a more manageable 640x320 window without any framebuffer logic change!! 

## Wishlist

A few things I still wish to add to the emulator are:

1. Add a way to pass the path to a rom file to load as arguments;

2. Make sure keyboard handling is working... apparently it's broken?

3. There are still some uninplemented opcodes ... doesn't fully prevent the emulator from running many games that don't depend on those but still, cool to have a full list of working opcodes..

4. Refactor the code a bit, maybe extracting keyboard and window handling out of the main file, just to compartmentalize things a bit more!

5. Improve logging;

6. Improve compilation times (maybe I'm rebuilding stuff unnecessarily even when just certain source files change ?).
