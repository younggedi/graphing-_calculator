# Graphing Calculator (C++ / SFML)

A simple graphing calculator that parses math expressions, evaluates them for a range of `x` values, and renders the resulting curves in an interactive SFML window (pan + zoom).

## Features
=======

- Multiple equations at once (each with a color)
- Expression parser + AST evaluator (supports `x`)
- SFML line batching for fast drawing
- Cursor-relative zoom (mouse wheel)
- Panning (middle mouse drag)
- Reset view (`Home`) and quit (`Esc`)

## Controls

- Mouse wheel: zoom in/out at the cursor
- Middle mouse drag: pan
- `Home`: reset view
- `Esc`: close window

<<<<<<< HEAD
## Supported Input

### Operators
=======
- `+`, `-`, `*`, `/`, `^`
- Parentheses: `(` `)`

<<<<<<< HEAD
## Functions

- `sin(...)`, `cos(...)`, `tan(...)`
- `log(...)` (base 10)
- `ln(...)` (natural log)
- `sqrt(...)`
<<<<<<< HEAD

## Colors

=======
The built-in color names are:

- `white`, `black`, `blue`, `red`, `yellow`, `green`

Unknown color names fall back to `white`.

## Build (CMake)

This project uses CMake and fetches **SFML 3.0.2** automatically via `FetchContent` (so you don’t need to install SFML separately).

## Requirements

- CMake 3.28+
- A C++23 compiler toolchain
  - Windows: Visual Studio 2022 (or newer)
  - Linux/macOS: a recent Clang or GCC

## Configure + Build

```powershell
cmake -S . -B build
cmake --build build --config Debug
```

The executable is placed under:

- `build/bin/Debug/main.exe` (Windows, Debug)
- `build/bin/Release/main.exe` (Windows, Release)

On non-Windows platforms, it will typically be under `build/bin/main`.

## Run

Run the executable, then follow the prompts:

1. Number of equations
2. Each equation string (uses `x` as the variable)
3. A color name for each equation
4. Start / stop / step for the x-range sampling
4. Start/stop

## Project Layout

- `src/main.cpp`: program entry (user input + window loop + controls)
- `src/graph_logic/graph.hpp`: SFML rendering + camera math
- `src/graph_logic/tokenizer.hpp`: tokenization + evaluation traversal helpers
- `src/graph_logic/AST.hpp`: AST node definitions + AST builder + operator evaluation

