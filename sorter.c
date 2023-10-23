//PROGRAM HEADERS
#include "sorter.h"
#include "datastructures.h"
//LIBRARY HEADERS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int comparison_function_all_files(const void *a, const void *b) {
    const DIR_FILE *dir_file_a = (const DIR_FILE *)a;
    const DIR_FILE *dir_file_b = (const DIR_FILE *)b;
    //COMPARE BY location_and_name
    return strcmp(dir_file_a->location_and_name, dir_file_b->location_and_name);
}

void sort_all_files_by_location_and_name() {
    printf("sorting all_files!\n"); //DEBUG
    qsort(all_files, n_all_files, sizeof(DIR_FILE), comparison_function_all_files);
}