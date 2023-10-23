//PROGRAM HEADERS
#include "patternprocessor.h"
//LIBRARY HEADERS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <stdbool.h>

//  CONVERT A FILENAME PATTERN (a glob) TO A REGULAR EXPRESSION.
//  FILENAME PATTERNS MAY NOT INCLUDE DIRECTORY SEPARATORS.
//	ON SUCCESS - A REGULAR EXPRESSION WILL BE RETURNED IN DYNAMICALLY ALLOCATED MEMORY.
//	ON FAILURE - A NULL POINTER WILL BE RETURNED.
char *glob_to_regex(char *glob) {
    char *re = NULL;
    if(glob != NULL) {
        re = calloc(strlen(glob) * 2 + 4, sizeof(char));
        if(re == NULL) {
            return NULL;
        }
        char *r = re;
        *r++ = '^';
        while(*glob != '\0') {
            switch (*glob) {
                case '.':
                case '\\':
                case '$':
                    *r++ = '\\';
                    *r++ = *glob++;
                    break;
                case '*':
                    *r++ = '.';
                    *r++ = *glob++;
                    break;
                case '?':
                    *r++ = '.';
                    glob++;
                    break;
                case '/':
                    free(re);
                    re = NULL;
                    break;
                default:
                    *r++ = *glob++;
                    break;
            }
        }
        if(re) {
            *r++ = '$';
            *r = '\0';
        }
    }
    return re;
}

bool filename_matches_regex(char *full_path, char *regex_string) {
    //CONVERT full_path TO filename
    char *filename = NULL;
    for(int c = (strlen(full_path) - 1); c >= 0; --c) {
        if(full_path[c] == '/') {
            filename = &full_path[c+1];
            break;
        }
    }
    printf("filename conversion in regex test: %s\n", filename); //DEBUG
    //COMPILE THE REGULAR EXPRESSION
    regex_t regex;
    int ret = regcomp(&regex, regex_string, REG_EXTENDED);
    if(ret) {
        //FAILED TO COMPILE
        fprintf(stderr, "Could not compile regex: %s\n", regex_string);
        exit(EXIT_FAILURE);
    }
    //EXECUTE THE REGULAR EXPRESSION
    ret = regexec(&regex, filename, 0, NULL, 0);
    if(!ret) {
        //MATCH
        printf("file %s matches regex %s\n", filename, regex_string); //DEBUG
        regfree(&regex);
        free(regex_string);
        return true;

    } 
    else if(ret == REG_NOMATCH) {
        //NO MATCH
        printf("file %s does not match regex %s\n", filename, regex_string); //DEBUG
        regfree(&regex);
        free(regex_string);
        return false;
    } 
    else {
        //ERROR
        fprintf(stderr, "regexec failed comparing file %s to regex %s\n", filename, regex_string);
        exit(EXIT_FAILURE);
    }
}