#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "cli.h"
#include "tga_editor.h"

Action_Stats ACTION_STATS_TABLE[] = {
    { .name = "resize", .args = 2 },
    { .name = "crop",   .args = 4 },
    { .name = "copy",   .args = 6 },
    { .name = "move",   .args = 2 },
    { .name = "bw",     .args = 0 },
    { .name = "info",   .args = 0 },
    { .name = "save",   .args = 0 },
    { .name = "exit",   .args = 0 }
};

Action_ID get_action_id_by_name(const char* str)
{
    for(int i = 0; i < ACTION_COUNT; i++)
    {
        if(strcmp(ACTION_STATS_TABLE[i].name, str) == 0){
            return i;
        }
    }
    return NONE;
}

Action_Status load_action(Action* action){
    for(int i = 0; i < MAX_ACTION_ARGS; i++)
        action->args[i] = 0;

    char buff[255];
    fgets(buff, sizeof(buff), stdin);
    int len = strlen(buff);
    buff[len - 1] = '\0';
    len--;

    if(len == 0) return ACTION_NO_INPUT_ERROR; 
    
    char* action_name = strtok(buff, " ");
    action->id = get_action_id_by_name(action_name);

    if(action->id == NONE) return ACTION_INVALID_NAME_ERROR;

    char* token = strtok(NULL, ",");
    int i = 0;
    while(token != NULL)
    {
        if(i == ACTION_STATS_TABLE[action->id].args) return ACTION_TOO_MANY_ARGS_ERROR;
        action->args[i] = atoi(token);
        if(action->args[i] > MAX_ACTION_ARG_VALUE) return ACTION_ARG_TOO_BIG_ERROR;
        token = strtok(NULL, ",");
        i++;
    }
    return ACTION_SUCCESS;
}

Action_Status run_action(Action* action, TGA_Data* data, const char* output_file_name)
{
    TGA_Result result;

    switch(action->id)
    {
        case RESIZE:
            result = tga_resize(data, action->args[0], action->args[1]);
            break;
        case CROP:
            result = tga_crop(data, action->args[0], action->args[1], action->args[2], action->args[3]);
            break;
        case COPY:
            result = tga_copy(data, action->args[0], action->args[1], action->args[2], action->args[3], action->args[4], action->args[5]);
            break;
        case MOVE:
            result = tga_move(data, action->args[0], action->args[1]);
            break;
        case BW:
            result = tga_bw(data);
            break;
        case INFO:
            result = tga_info(data);
            break;
        case SAVE:
            result = tga_save(data, output_file_name);
            if(result == TGA_SAVE_FAIL_ERROR) return ACTION_OUTPUT_FILE_ERROR;
            break;
        case EXIT:
            result = tga_save(data, output_file_name);   
            if(result == TGA_SAVE_FAIL_ERROR) return ACTION_OUTPUT_FILE_ERROR;
            return ACTION_EXIT;                                                             
    }

    print_error_message(result);
    return ACTION_SUCCESS;
}

void print_error_message(TGA_Result result)
{
    if(result != TGA_SUCCESS) printf("[TGA_ERROR] ");
    switch(result)
    {
        case TGA_WRONG_ARGS_ERROR:
            printf("Invalid action arguments\n");
            break;
        case TGA_BIGGER_THAN_ORIGINAL_ERROR:
            printf("The new image size cannot be bigger then the original\n");
            break;
        case TGA_CROP_OFF_BOUNDS_ERROR:
            printf("You cannot crop an area off of the image bounds\n");
            break;
        case TGA_COPY_OFF_BOUNDS_ERROR:
            printf("You cannot copy nor paste an are off of the image bounds\n");
            break;
    }
}

void cli_start(const char* output_path){

}