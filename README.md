# BFC - BrainFuck Compiler
A Brainfuck compiler targeting x86-64 NASM assembly for Linux.

## Build
```bash
    cmake -S . -B build
    cmake --build build
```

## Usage
You need to have NASM and ld installed.
```bash
    bfc <input.bf> <output>
```
