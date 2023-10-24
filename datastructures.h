//PROGRAM HEADERS
#include "mysync.h" //contains shared library headers
//LIBRARY HEADERS
#include <stdbool.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

//SHARED STRUCTURES
typedef struct {
    bool include_hiddens; //-a switch
    bool synchronize; //not -n switch
    bool report_activity; //-v switch
    bool use_recursive_processing; //-r switch
    bool use_old_file_details; //-p switch
    char **ignore_regex; //-i [pattern] switch(s)
    int n_ignore_regex;
    char **only_regex; //-o [pattern] switch(s)
    int n_only_regex;
} SWITCH_CONDITIONS; //configuration options set using command line switches.
typedef struct {
    char *top_level_directory; //(eg. /Users/hhewgill/_DIR1)
    char *location_and_name; //excluding top_level_directory (eg. /SUBDIRa1/SUBDIRb1/IMG_2503.jpeg)
    time_t modification_time;
    mode_t permissions;
} DIR_FILE; //a file found in a directory.

//SHARED VARIABLES
extern char **top_level_directories; //array of strings, implemented as pointers to char pointers.
extern int n_top_level_directories;
extern SWITCH_CONDITIONS switch_conditions;
extern DIR_FILE *all_files; //array of DIR_FILEs
extern int n_all_files;
extern DIR_FILE *unique_files; //array of DIR_FILEs
extern int n_unique_files;

//SHARED FUNCTIONS
extern void initialize_datastructures();
extern void print_switch_conditions();
extern void print_top_level_directories();
extern void print_all_files();
extern void print_unique_files();
extern void store_string(char ***, int *, char *);
extern void store_file(DIR_FILE **, int *, char *, time_t, char *, mode_t);
extern void test_for_memory_allocation_error(void *);