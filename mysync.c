//  CITS2002 Project 2 2023
//  Student1:   23012728   HENRY_HEWGILL
//  Student2:   N/A        N/A

//PROGRAM HEADERS
#include "directorymanager.h"
#include "datastructures.h"
#include "patternprocessor.h"
//LIBRARY HEADERS
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

#define OPTLIST "anvrpi:o:"

void usage_error(char *program_name) {
    fprintf(stderr, "Usage: %s [-a] [-n] [-v] [-r] [-p] [-i pattern] [-o pattern] directory1 directory2 [directory3...]\n", program_name);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    initialize_datastructures();
    //PROCESS COMMAND LINE SWITCHES WITH getopt()
    int opt; //holds options found in command line switches from getopt()
    opterr = 0; //disable getopt()'s own error messages
    while((opt = getopt(argc, argv, OPTLIST)) != -1) {
        //printf("%d\n", opt); //DEBUG
        switch (opt) {
            case 'a':
                switch_conditions.include_hiddens = true;
                //printf("-a was received by the switch case condition\n"); //DEBUG
                break;
            case 'n':
                switch_conditions.synchronize = false;
                switch_conditions.report_activity = true;
                break;
            case 'v':
                switch_conditions.report_activity = true;
                break;
            case 'r':
                switch_conditions.use_recursive_processing = true;
                break;
            case 'p':
                switch_conditions.use_old_file_details = true;
                break;
            case 'i': {
                //printf("option -i with argument: %s\n", optarg); //DEBUG
                //switch_conditions.ignore_regex = (char **) realloc(switch_conditions.ignore_regex, (switch_conditions.n_ignore_regex + 1) * sizeof(char *));
                //test_for_memory_allocation_error(switch_conditions.ignore_regex);
                //switch_conditions.ignore_regex[switch_conditions.n_ignore_regex] = strdup(optarg);
                //++switch_conditions.n_ignore_regex;
                char *regex = glob_to_regex(optarg);
                if(regex == NULL) {
                    fprintf(stderr, "Failed to convert %s to regex.\n", optarg);
                    exit(EXIT_FAILURE);
                }
                store_string(&switch_conditions.ignore_regex, &switch_conditions.n_ignore_regex, regex);
                break;
            }
            case 'o': {
                //printf("option -o with argument: %s\n", optarg); //DEBUG
                char *regex = glob_to_regex(optarg);
                if(regex == NULL) {
                    fprintf(stderr, "Failed to convert %s to regex.\n", optarg);
                    exit(EXIT_FAILURE);
                }
                store_string(&switch_conditions.only_regex, &switch_conditions.n_only_regex, regex);
                break;
            }
            default:
                usage_error(argv[0]);
        }
    }
    //PROCESS REMAINING COMMAND LINE ARGUMENTS
    for(int i = optind; i < argc; ++i) { //iterate over remaining non-switch arguments
        store_string(&top_level_directories, &n_top_level_directories, argv[i]);
        //printf("%s\n", argv[i]); //DEBUG
    }
    print_top_level_directories(); //DEBUG
    print_switch_conditions(); //DEBUG
    collect_every_directorys_files();
    print_all_files(); //DEBUG
    collect_newest_unique_files();
    write_files_to_directories();
    return EXIT_SUCCESS;
}