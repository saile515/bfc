# BFC - BrainFuck Compiler
A Brainfuck compiler targeting x86-64 NASM assembly for Linux.

## Build
```bash
    cmake -S . -B build
    cmake --build build
```

## Usage
```bash
    bfc <file.bf>
    nasm -felf64 build/out.asm -o build/out.o
    ld build/out.o -o build/out
```
