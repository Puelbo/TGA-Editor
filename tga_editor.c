#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "tga_editor.h"

TGA_Data* tga_get_data(const char* file_path)
{
    TGA_Data* data = (TGA_Data*) malloc(sizeof(TGA_Data));
    FILE* file = fopen(file_path, "rb");
    if(file == NULL){
        free(data);
        return NULL;
    }

    fread(&data->header, sizeof(TGA_Header), 1, file);

    int width = 0;
    int height = 0;

    memcpy(&width, data->header.width, 2);
    memcpy(&height, data->header.height, 2);

    data->pixel_buff = (Pixel*) malloc(sizeof(Pixel) * width * height);
    fread(data->pixel_buff, sizeof(Pixel) * width * height, 1, file);
    fclose(file);

    return data;
}

void tga_destroy_data(TGA_Data* data)
{

    free(data->pixel_buff);
    free(data);
}

TGA_Result tga_resize(TGA_Data* data, int width, int height)
{
    if(width <= 0 || height <= 0) return TGA_WRONG_ARGS_ERROR;

    int old_width = 0;
    int old_height = 0;

    memcpy(&old_width, data->header.width, 2);
    memcpy(&old_height, data->header.height, 2);

    Pixel* new_pixel_buff = (Pixel*) malloc(width * height * sizeof(Pixel));
    bool* isOriginalPixel = (bool*) calloc(width * height, sizeof(bool));

    for(int row = 0; row < old_height; row++)
    {
        for(int col = 0; col < old_width; col++)
        {
            int col_pos =  (col == 0)? 0 : (int) round((float) ((width - 1) * col) / old_width);
            int row_pos =  (row == 0)? 0 : (int) round((float) ((height - 1) * row) / old_height);

            Pixel* pixel = data->pixel_buff + (row * old_width + col);

            int index = row_pos * width + col_pos;
            new_pixel_buff[index] = *pixel;
            isOriginalPixel[index] = true;
        }
    }

    for(int row = 0; row < height; row++)
    {
        for(int col = 0; col < width; col++)
        {
            if(isOriginalPixel[row * width])
            {
                int pixel_index = row * width + col;
                int i = 0;
                while(!isOriginalPixel[pixel_index - i])
                    i++;
                
                new_pixel_buff[pixel_index] = new_pixel_buff[pixel_index - i];
                isOriginalPixel[pixel_index] = true;
            }
        }
    }

    for(int row = 0; row < height; row++)
    {
        for(int col = 0; col < width; col++)
        {
            if(!isOriginalPixel[row * width + col])
            {
                int i = 0;
                while(!isOriginalPixel[(row - i) * width + col] && row != 0)
                    i++;
                
                new_pixel_buff[row * width + col] = new_pixel_buff[(row - i) * width + col];
            }
        }
    }

    free(data->pixel_buff);
    free(isOriginalPixel);
    data->pixel_buff = new_pixel_buff;
    memcpy(data->header.width, &width, 2);
    memcpy(data->header.height, &height, 2);
    return TGA_SUCCESS;
}

TGA_Result tga_crop(TGA_Data* data, int x, int y, int width, int height)
{
    int original_width = 0;
    int original_height = 0;

    memcpy(&original_width, data->header.width, 2);
    memcpy(&original_height, data->header.height, 2);

    if(x < 0 || y < 0 || width <= 0 || height <= 0) return TGA_WRONG_ARGS_ERROR;

    if(width > original_width || height > original_height) return TGA_BIGGER_THAN_ORIGINAL_ERROR;

    if(
        x + width > original_width ||
        y + height > original_height
    ) return TGA_CROP_OFF_BOUNDS_ERROR;
    

    Pixel* crop = (Pixel*) malloc(sizeof(Pixel) * width * height);

    for(int row = 0; row < height; row++)
    {
        for(int col = 0; col < width; col++)
        {
            crop[row * width + col] = *(data->pixel_buff + ((row + y) * original_width + col + x));
        }
    }

    free(data->pixel_buff);
    data->pixel_buff = crop;
    memcpy(data->header.width, &width, 2);
    memcpy(data->header.height, &height, 2);
    return TGA_SUCCESS;
}

TGA_Result tga_copy(TGA_Data* data, int copy_x, int copy_y, int width, int height, int paste_x, int paste_y)
{

    if(
        copy_x < 0  ||
        copy_y < 0  ||
        width <= 0  ||
        height <= 0 ||
        paste_x < 0 ||
        paste_y < 0 
    ) return TGA_WRONG_ARGS_ERROR;

    int original_width = 0;
    int original_height = 0;

    memcpy(&original_width, data->header.width, 2);
    memcpy(&original_height, data->header.height, 2);

    if(width > original_width || height > original_height) return TGA_BIGGER_THAN_ORIGINAL_ERROR;

    if(
        copy_x + width > original_width ||
        copy_y + height > original_height ||
        paste_x + width > original_width ||
         paste_y + height > original_height
    ) return TGA_COPY_OFF_BOUNDS_ERROR;


    Pixel* crop = (Pixel*) malloc(sizeof(Pixel) * width * height);

    for(int row = 0; row < height; row++)
    {
        for(int col = 0; col < width; col++)
        {
            crop[row * width + col] = *(data->pixel_buff + ((row + copy_y) * original_width + col + copy_x));
        }
    }

    for(int row = 0; row < height; row++)
    {
        for(int col = 0; col < width; col++)
        {
            data->pixel_buff[(row + paste_y) * original_width + col + paste_x] = crop[row * width + col];
        }
    }

    free(crop);
    return TGA_SUCCESS;
}


TGA_Result tga_move(TGA_Data* data, int x, int y)
{
    int width = 0;
    int height = 0;

    memcpy(&width, data->header.width, 2);
    memcpy(&height, data->header.height, 2);

    Direction x_dir = RIGHT;
    Direction y_dir = DOWN;

    if(x < 0)
    {
        x = abs(x);
        x_dir = LEFT;
    }

    if(y < 0)
    {
        y = abs(y);
        y_dir = UP;
    }

    if(x_dir == RIGHT)
    {
        for(int i = 0; i < x; i++)
        {
            for(int row = height - 1; row >= 0; row--)
            {
                for(int col = width - 1; col >= 0; col--)
                {
                    if(col == 0) data->pixel_buff[row * width + col] = data->pixel_buff[row * width + width - 1];
                    else data->pixel_buff[row * width + col] = data->pixel_buff[row * width + col - 1];
                }
            }
        }
    }
    else if(x_dir == LEFT)
    {
        for(int i = 0; i < x; i++)
        {
            for(int row = 0; row < height; row++)
            {
                for(int col = 0; col < width; col++)
                {
                    if(col == width - 1) data->pixel_buff[row * width + col] = data->pixel_buff[row * width];
                    else data->pixel_buff[row * width + col] = data->pixel_buff[row * width + col  + 1];
                }
            }
        }
    }

    if(y_dir == UP)
    {
        for(int i = 0; i < y; i++)
        {
            for(int row = height - 1; row >= 0; row--)
            {
                for(int col = width - 1; col >= 0; col--)
                {
                    if(row == 0) data->pixel_buff[row * width + col] = data->pixel_buff[(height - 1) * width + col];
                    else data->pixel_buff[row * width + col] = data->pixel_buff[(row - 1) * width + col];

                }
            }
        }
    }
    else if(y_dir == DOWN)
    {
        for(int i = 0; i < y; i++)
        {
            for(int row = 0; row < height; row++)
            {
                for(int col = 0; col < width; col++)
                {
                    if(row == height - 1) data->pixel_buff[row * width + col] = data->pixel_buff[col];
                    else data->pixel_buff[row * width + col] = data->pixel_buff[(row + 1) * width + col];
                }
            }
        }
    }
    return TGA_SUCCESS;
}

TGA_Result tga_bw(TGA_Data* data)
{
    int width = 0;
    int height = 0;

    memcpy(&width, data->header.width, 2);
    memcpy(&height, data->header.height, 2);

    for(int row = 0; row < height; row++)
    {
        for(int col = 0; col < width; col++)
        {
            Pixel* pixel = data->pixel_buff + row * width + col;
            byte rgb_avg = (byte) round((float) (pixel->green + pixel->red + pixel->blue) / 3);
            pixel->green = rgb_avg;
            pixel->red = rgb_avg;
            pixel->blue = rgb_avg;
        }
    }
    return TGA_SUCCESS;
}

TGA_Result tga_info(TGA_Data* data){
    int width = 0;
    int height = 0;

    memcpy(&width, data->header.width, 2);
    memcpy(&height, data->header.height, 2);

    printf("[INFO] width: %d, height: %d\n", width, height);
    return TGA_SUCCESS;
}

TGA_Result tga_save(TGA_Data* data, const char* output_file_name)
{
    int file_name_len = strlen(output_file_name);
    const char* cursor = output_file_name + file_name_len - 4;

    char buff[255];
    strcpy(buff, output_file_name);
    if(strcmp(cursor, ".tga") != 0) strcat(buff, ".tga");

    FILE* tga_output = fopen(buff, "wb");
    if(tga_output == NULL) return TGA_SAVE_FAIL_ERROR;
    
    int width = 0;
    int height = 0;

    memcpy(&width, data->header.width, 2);
    memcpy(&height, data->header.height, 2);
    fwrite(&data->header, sizeof(TGA_Header), 1, tga_output);
    fwrite(data->pixel_buff, sizeof(Pixel) * width * height, 1, tga_output);
    fclose(tga_output);
    return TGA_SUCCESS;
}