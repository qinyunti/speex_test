#! /bin/sh

gcc libspeexdsp/*.c speexecho.c -Iinclude -I. -DHAVE_CONFIG_H -lm -o speexecho