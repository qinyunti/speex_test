#! /bin/sh

riscv64-linux-gnu-gcc libspeexdsp/*.c speexecho.c -static -Os -Iinclude -I. -DHAVE_CONFIG_H -lm -o speexecho
