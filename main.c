#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "cli.h"
#include "tga_editor.h"

#define MIN_ARGUMENT_COUNT 4

int main(int argc, char** argv)
{
    if(argc - 1 < MIN_ARGUMENT_COUNT)
    {
        printf("Invalid program arguments\n");
        return 1;
    }

    int output_flag_count = 0;
    int input_flag_count = 0;
    int param_values = 0;
    int i = 1;
    char* input_path;
    char* output_path;

    for(; i < argc; i++)
    {
        if(strcmp(argv[i], "--input") == 0)
        {
            input_flag_count++;
            if(i + 1 < argc) input_path = argv[i + 1];
            continue;
        }
        if(strcmp(argv[i], "--output") == 0)
        {
            output_flag_count++;
            if(i + 1 < argc) output_path = argv[i + 1];
            continue;
        }
        param_values++;
    }

    if(input_flag_count == 0)
    {
        printf("Input path was not provided\n");
        return 1;
    }
    else if(output_flag_count == 0)
    {
        printf("Output path was not provided\n");
        return 1;
    }
    else if(output_flag_count > 1)
    {
        printf("Parameter --input provided multiple times\n");
        return 1;
    }
    else if(input_flag_count > 1)
    {
        printf("Parameter --output provided multiple times\n");
        return 1;
    }
    else if(param_values > 2)
    {
        printf("Invalid program arguments\n");
        return 1;
    }

    TGA_Data* data = tga_get_data(input_path);
    if(data == NULL)
    {
        printf("Invalid input file\n");
        return 1;
    }

    printf("#### TGA EDITOR ####\n");

    while(true)
    {
        printf("> ");
        Action action;
        Action_Status status = load_action(&action);

        if(status == ACTION_NO_INPUT_ERROR) continue;

        if(status == ACTION_INVALID_NAME_ERROR)
        {
            printf("Action with that name doesnt exist\n");
            continue;
        }

        if(status == ACTION_TOO_MANY_ARGS_ERROR)
        {
            printf("Action has too many arguments\n");
            continue;
        }

        if(status == ACTION_ARG_TOO_BIG_ERROR)
        {
            printf("Action argument number over %d is invalid\n", MAX_ACTION_ARG_VALUE);
            continue;
        }

        status = run_action(&action, data, output_path);

        if(status == ACTION_OUTPUT_FILE_ERROR){
            printf("Failed to save the new file\n");
            tga_destroy_data(data);;
            return 1;
        }

        if(status == ACTION_EXIT)
        {
            tga_destroy_data(data);
            return 0;
        }
    }
    tga_destroy_data(data);
    return 0;
}
