#ifndef CLI_H
#define CLI_H
#include "tga_editor.h"
#include <stdbool.h>

#define ACTION_COUNT 8
#define MAX_ACTION_ARGS 6
#define MAX_ACTION_ARG_VALUE 25000

typedef struct {
    char* name;
    int args;
} Action_Stats;

typedef enum {
    NONE = -1,
    RESIZE,
    CROP,
    COPY,
    MOVE,
    BW,
    INFO,
    SAVE,
    EXIT,
} Action_ID;

typedef enum
{
    ACTION_SUCCESS,
    ACTION_OUTPUT_FILE_ERROR,
    ACTION_EXIT,
    ACTION_NO_INPUT_ERROR,
    ACTION_ARG_TOO_BIG_ERROR,
    ACTION_TOO_MANY_ARGS_ERROR,
    ACTION_INVALID_NAME_ERROR
} Action_Status;

typedef struct {
    Action_ID id;
    int args[MAX_ACTION_ARGS];
} Action;

extern Action_Stats ACTION_STATS_TABLE[];

Action_ID get_action_id_by_name(const char* str);
Action_Status load_action();
Action_Status run_action(Action* action, TGA_Data* data, const char* output_file_name);
void print_error_message(TGA_Result result);
#endif