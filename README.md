# TGA EDITOR

A CLI program that can edit .tga images

## Installation

> Download the directory
> Compile the code in the directory by running the 'build.sh' script in the source folder (you need gcc for this)

```bash
./build.sh
```

or like this

```bash
gcc main.c cli.c tga_editor.c -lm -g -o photoshop
```

## How to use

--input <file path> - path to the input file
--output <file path> - path for the newly created output

**resize w,h**              Resizes the image to the dimensions (w, h), w stands for width, h stands for height
**crop x,y,w,h**            crops the image starting from (x, y) with the image dimensions (w, h)
**copy x1,y1,w,h x2,y2**    cutouts an area starting from (x1, y1) with the image dimensions (w, h), and copies them at the start of (x2, y2)
**move x,y**                Shifts the image pixels by the x value (x>0 = right, x<0 = left) and y value (y>0 = down, x<0 = up)
**bw**                      converts the image to grayscale
**info**                    prints out the current dimensions of the image
**save**                    saves the current version of the image to the output file
**exit**                    same as save but also exits the program

Example

```bash
$ ./build.sh
$ ./photoshop --input input.tga --output output

resize 256,256
move -20,0
crop 20,20,128,128
copy 5,5,10,10 35,20
save
move 20,35
bw

```