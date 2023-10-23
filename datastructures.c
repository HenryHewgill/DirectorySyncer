//PROGRAM HEADERS
#include "datastructures.h"
//LIBRARY HEADERS
#include <stdlib.h>
#include <stdio.h>

//SHARED VARIABLES
char **top_level_directories; //array of strings, implemented as pointers to char pointers.
int n_top_level_directories;
SWITCH_CONDITIONS switch_conditions;
DIR_FILE *all_files; //array of DIR_FILEs
int n_all_files;
DIR_FILE *unique_files; //array of DIR_FILEs
int n_unique_files;

//SHARED FUNCTIONS
void test_for_memory_allocation_error(void *pointer) {
    if(pointer == NULL) {
        fprintf(stderr, "Memory allocation error.\n");
        exit(EXIT_FAILURE);
    }
}
void store_string(char ***location, int *n_at_location, char *string_to_store) { //triple pointer dereferencing is to circumvent passing pointer by value (thereby enabling reassignment with realloc)
    *location = (char **) realloc(*location, (*n_at_location + 1) * sizeof(char *));
    test_for_memory_allocation_error(*location);
    (*location)[*n_at_location] = strdup(string_to_store);
    test_for_memory_allocation_error((*location)[*n_at_location]);
    ++*n_at_location;
}
void store_file(DIR_FILE **location_to_store, int *n_files_at_location, char *top_level_directory, time_t modification_time, char *location_and_name, mode_t permissions) { //double pointer dereferencing is to circumvent passing pointer by value (thereby enabling reassignment with realloc)
    //printf("file to store: %s\n", full_path); //DEBUG
    //printf("file to store's top level directory: %s\n", top_level_directory); //DEBUG
    //printf("file to store's location and name: %s\n", &full_path[strlen(top_level_directory)]); //DEBUG
    //printf("file to store's modification time: %li\n", stat_buffer->st_mtime); //DEBUG
    //printf("file to store's modification time from time_t: %li\n", (time_t) stat_buffer->st_mtime); //DEBUG
    //CREATE THE DIR_FILE ENTRY TO STORE
    DIR_FILE *file_to_store = (DIR_FILE *) malloc(sizeof(DIR_FILE));
    test_for_memory_allocation_error(file_to_store);
    file_to_store->top_level_directory = strdup(top_level_directory);
    file_to_store->location_and_name = location_and_name;
    file_to_store->modification_time = modification_time;
    file_to_store->permissions = permissions;
    printf("file to store's top level directory: %s\n", file_to_store->top_level_directory); //DEBUG
    printf("file to store's location and name: %s\n", file_to_store->location_and_name); //DEBUG
    printf("file to store's modification time from time_t: %li\n", file_to_store->modification_time); //DEBUG
    printf("file to store's permissions: %o\n", file_to_store->permissions); //DEBUG
    //STORE AT location_to_store
    *location_to_store = (DIR_FILE *) realloc(*location_to_store, (*n_files_at_location + 1) * sizeof(DIR_FILE));
    test_for_memory_allocation_error(*location_to_store);
    (*location_to_store)[*n_files_at_location] = *file_to_store;
    ++*n_files_at_location;
}

void print_switch_conditions() { //DEBUG
    printf("include_hiddens:\t\t%d\n", switch_conditions.include_hiddens);
    printf("synchronize:\t\t\t%d\n", switch_conditions.synchronize);
    printf("report_activity:\t\t%d\n", switch_conditions.report_activity);
    printf("use_recursive_processing:\t%d\n", switch_conditions.use_recursive_processing);
    printf("use_old_file_details:\t\t%d\n", switch_conditions.use_old_file_details);
    for(int i = 0; i < switch_conditions.n_ignore_regex; ++i) {
        printf("-i pattern: %s\n", switch_conditions.ignore_regex[i]);
    }
    for(int o = 0; o < switch_conditions.n_only_regex; ++o) {
        printf("-o pattern: %s\n", switch_conditions.only_regex[o]);
    }
}

void print_all_files() { //DEBUG
    printf("printing all_files:\n");
    for(int f = 0; f < n_all_files; ++f) {
        printf("stored file: %s\n", all_files[f].location_and_name);
    }
    printf("n_all_files: %i\n", n_all_files);
}

void print_unique_files() { //DEBUG
    printf("printing unique_files:\n");
    for(int f = 0; f < n_unique_files; ++f) {
        printf("unique file: %s\n", unique_files[f].location_and_name);
    }
    printf("n_unique_files: %i\n", n_unique_files);
}

void print_top_level_directories() { //DEBUG
    for(int d = 0; d < n_top_level_directories; ++d) {
        printf("top level directory: %s\n", top_level_directories[d]);
    }
}

void initialize_datastructures() {
    top_level_directories = NULL;
    n_top_level_directories = 0;
    switch_conditions.include_hiddens = false;
    switch_conditions.synchronize = true;
    switch_conditions.report_activity = false;
    switch_conditions.use_recursive_processing = false;
    switch_conditions.use_old_file_details = false;
    switch_conditions.ignore_regex = NULL;
    switch_conditions.n_ignore_regex = 0;
    switch_conditions.only_regex = NULL;
    switch_conditions.n_only_regex = 0;
    all_files = NULL;
    n_all_files = 0;
    unique_files = NULL;
    n_unique_files = 0;
}