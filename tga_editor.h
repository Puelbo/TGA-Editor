#ifndef TGA_EDITOR_H
#define TGA_EDITOR_H

#include <stdbool.h>
#define ACTION_COUNT 8
typedef unsigned char byte;

typedef struct {
    byte id_length;
    byte color_map_type;
    byte image_type;
    byte color_map[5];
    byte x_origin[2];
    byte y_origin[2];
    byte width[2];
    byte height[2];
    byte depth;
    byte descriptor;
} TGA_Header;

typedef struct {
    byte blue;
    byte green;
    byte red;
} Pixel;

typedef struct {
    TGA_Header header;
    Pixel* pixel_buff;
} TGA_Data;

typedef enum {
    TGA_SUCCESS,
    TGA_WRONG_ARGS_ERROR,
    TGA_CROP_OFF_BOUNDS_ERROR,
    TGA_COPY_OFF_BOUNDS_ERROR,
    TGA_BIGGER_THAN_ORIGINAL_ERROR,
    TGA_SAVE_FAIL_ERROR
} TGA_Result;

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

TGA_Data* tga_get_data(const char* file_path);
void tga_destroy_data(TGA_Data* data);
TGA_Result tga_resize(TGA_Data* data, int width, int height);
TGA_Result tga_crop(TGA_Data* data, int x, int y, int width, int height);
TGA_Result tga_copy(TGA_Data* data, int copy_x, int copy_y, int width, int height, int paste_x, int paste_y);
TGA_Result tga_move(TGA_Data* data, int x, int y);
TGA_Result tga_bw(TGA_Data* data);
TGA_Result tga_info(TGA_Data* data);
TGA_Result tga_save(TGA_Data* data, const char* output_file_name);

#endif