# The Calamus 2D game engine

Yet another project I probably won't ever get to a usable state!

# Progress

- [x] TOML configuration loader
- [ ] Basic resource management systems
  - [x] Textures
  - [x] Tile maps
  - [x] Fonts
  - [ ] Sounds
  - [ ] Entities
- [ ] (De,)serialization
- [ ] Level loading
- [ ] Player movement
- [ ] UI system
  - [x] Labels
  - [x] Buttons
  - [x] Mouse events
  - [ ] Keyboard events
  - [ ] Automatic layout
- [ ] Scripting engine
  - [x] Basic functionality (classes, functions, etc.)
  - [ ] Not utterly scuffed and unusable
  - [ ] Interop with other engine components

# Notes

- The built-in scripting language is largely based on [lox from the Crafting Interpreters](https://craftinginterpreters.com/) book, with some minor changes and additions.
My implementation of it is horribly bad and filled with bugs (see below).
- You literally can't do anything with this "engine" right now.
- It's just an abstraction layer over [raylib](https://github.com/raysan5/raylib), which itself is a layer over SDL and similar libraries. If you want to actually get something done, use those instead.

# Scripting language

I offer you this fine selection of bugs and implementation mistakes:

- Allocating more than a couple objects at once causes a `SIGFPE` exception and crashes the program.
- The code is not designed to be even slightly modular. Everything points to and interacts with everything else. There is one singular VM.
- The "C++" implementation is mostly just glorified C. There's `reinterpret_cast`s, macros, pointers and more of your favorite headaches _everywhere_.
- There's no interop with any other component of the engine yet, your scripts are totally alone and cannot interact with anything on the screen.

Some changes over the original language, Lox:

- Built-in `gc()` (forces immediate garbage collection) and `memory()` (returns total number of bytes allocated) functions for debugging purposes.
- Inheriting classes uses the `:` token, not `<`.
- It understands the keyword `import`, but don't be fooled - there's no modules yet.
- _Some_ work is being done modernizing the code to "proper" C++, but it's far from finished:
  - The internal hashtable is now a wrapper over `std::unordered_map`.
  - A few components are now in a class with their associated member functions instead of C-style freestanding ones.
  - Namespaces. Exciting changes, I know.
  - C++-style casts, and there's lots of them! In fact, there's 94 casts.
  - Some components, especially the compiler, use (const-)references instead of raw pointers.
  - There's some `std::optional` sprinkled in too, somewhere.
  - Deduced types `(const) auto` wherever it makes sense.
  - ... This all doesn't matter. The entire scripting subsystem desperately needs to be rewritten.

# Build system

**Only Linux and macOS are buildable.** MSVC/Windows is totally untested and will be rejected by cmake. (I have no idea what all the equivalent compile flags are.)

The project is built using cmake. Following options are exposed:

- `NATIVE_OPTS`: Toggles processor-native optimizations (default: on)
- `LTO`: Toggles link-time optimizations (default: on)
- `USE_MOLD`: Forces using the modern, fast [mold](https://github.com/rui314/mold) linker (default: off)
- Build types:
  - `Release`: Debug macros disabled, highest optimization level
  - `Debug` (recommended): Debug macros **enabled**, lowest optimization level
  - `RelWithDebInfo`: Debug macros disabled, high optimization level, not stripped (debuggable in `gdb` or whatever)
  - `MinSizeRel`: Debug macros disabled, optimizes for smallest binary size

Dependencies are automatically managed by [CPM](https://github.com/cpm-cmake/CPM.cmake).

Using a fresh tree, you may build this abomination as follows (not recommended, better `rm -rf .` and run):

```shell
mkdir build
cd build
cmake -GNinja -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --parallel=$(nproc)
```
