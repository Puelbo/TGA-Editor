#!/bin/bash
gcc main.c cli.c tga_editor.c -lm -g -o photoshop

if [ $? -eq 0 ]; then
    echo "Build complete"
fi