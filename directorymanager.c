//PROGRAM HEADERS
#include "directorymanager.h"
#include "sorter.h"
#include "datastructures.h"
#include "patternprocessor.h"
//LIBRARY HEADERS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h> //DIR, struct dirent, opendir(), readdir()
#include <sys/stat.h> //struct stat, stat(), S_ISDIR(), S_ISREG(), fchmod()
#include <sys/types.h>
#include <sys/param.h> //MAXPATHLEN
#include <time.h>
#include <utime.h>
#include <fcntl.h>
#include <unistd.h>

#define FILE_WRITING_BUFFER_SIZE 10000

void validate_top_level_directory(char *directory) {
    printf("directory to validate: %s\n", directory); //DEBUG
    DIR *p_directory = opendir(directory);
    if(p_directory == NULL) {
        perror(directory);
        exit(EXIT_FAILURE);
    }
    closedir(p_directory);
}

char *step_down_hierarchy(char *directory) { //returns the directory path below the received directory
    char *lower_directory = strdup(directory);
    for(int c = (strlen(directory) - 1); c >= 0; --c) {
        if(directory[c] == '/') {
            lower_directory[c] = '\0';
            break;
        }
    }
    return lower_directory;
}

void ensure_directory_structure_exists(char *directory) {
    printf("directory_structure_to_ensure: %s\n", directory); //DEBUG
    DIR *p_directory = opendir(directory);
    if(p_directory == NULL) {
        //DIRECTORY DOESN'T EXIST
        char *lower_directory = step_down_hierarchy(directory);
        ensure_directory_structure_exists(lower_directory);
        if(mkdir(directory, 0777) == -1) {
            //directory creation failed
            perror(directory);
            exit(EXIT_FAILURE);
        }
        else {
            printf("created directory: %s\n", directory); //DEBUG
        }
    }
    else {
        //DIRECTORY EXISTS
        printf("directory exists: %s\n", directory); //DEBUG
        closedir(p_directory);
        printf("successfully closed %s\n", directory); //DEBUG
    }
}

void create_or_update_file_from_stored_file(bool create_not_update, char *full_path_new, char *full_path_stored, mode_t permissions_for_new_file, time_t modification_time_stored) {
    int file_descriptor_new;
    if(create_not_update) {
        //CREATING FILE
        file_descriptor_new = open(full_path_new, O_CREAT | O_WRONLY, permissions_for_new_file); //creates a file for writing with appropriate permissions
        if(file_descriptor_new == -1) {
            //NEW FILE FAILED TO OPEN
            perror(full_path_new);
            exit(EXIT_FAILURE);
        }
    }
    else {
        //UPDATING FILE
        file_descriptor_new = open(full_path_new, O_WRONLY | O_TRUNC); //opens the existing file for writing and truncates its original content to 0 bytes (clears it)
        if(file_descriptor_new == -1) {
            //EXISTING FILE FAILED TO OPEN
            perror(full_path_new);
            exit(EXIT_FAILURE);
        }
        //SET EXISTING FILE'S PERMISSIONS
        if(fchmod(file_descriptor_new, permissions_for_new_file) == -1) {
            //PERMISSION CHANGE FAILED
            fprintf(stderr, "Failed to update permissions for existing file: %s\n", full_path_new);
            exit(EXIT_FAILURE);
        }
    }
    int file_descriptor_stored = open(full_path_stored, O_RDONLY); //open the old file for reading
    if(file_descriptor_stored == -1) {
        //STORED FILE FAILED TO OPEN
        perror(full_path_stored);
        exit(EXIT_FAILURE);
    }
    //WRITE TO THE NEW FILE FROM THE STORED FILE
    char buffer[FILE_WRITING_BUFFER_SIZE];
    size_t got;
    while((got = read(file_descriptor_stored, buffer, sizeof buffer)) > 0) {  
        if(write(file_descriptor_new, buffer, got) != got) {  
            //WRITE FAILED
            close(file_descriptor_stored); 
            close(file_descriptor_new);
            fprintf(stderr, "File writing failed when writing from %s to %s.\n", full_path_stored, full_path_new);
            exit(EXIT_FAILURE);
        }
    }
    if(switch_conditions.use_old_file_details) {
        //UPDATE MODIFICATION TIME TO STORED FILE'S
        struct utimbuf stored_file_times;
        stored_file_times.actime = modification_time_stored;
        stored_file_times.modtime = modification_time_stored;
        if(utime(full_path_new, &stored_file_times) < 0) {
            //MODIFICATION TIME CHANGE FAILED
            fprintf(stderr, "Failed to update modification time for new / updated file: %s\n", full_path_new);
            exit(EXIT_FAILURE);
        }
    }
    close(file_descriptor_stored);
    close(file_descriptor_new);
}

void process_directory(char *directory, char *top_level_directory) { //top_level_directory will cascade down through recursive calls to be stored with files
    printf("directory to process: %s\n", directory); //DEBUG
    DIR *p_directory = opendir(directory);
    struct dirent *p_dir_entry;
    struct stat stat_buffer; //status information of a file gets populated in this stat
    //READ THE DIRECTORY
    while((p_dir_entry = readdir(p_directory)) != NULL) {
        char full_path[MAXPATHLEN]; //put the file on the end of the path eg. "directory/file"
        sprintf(full_path, "%s/%s", directory, p_dir_entry->d_name);
        //READ THE FILE INFO
        if(stat(full_path, &stat_buffer) != 0) { //!=0 from stat means it failed
            perror("stat");
            exit(EXIT_FAILURE);
        }
        else if(S_ISDIR(stat_buffer.st_mode)) {
            //IS A DIRECTORY
            //printf("%s is a directory\n", full_path); //DEBUG
            if(strcmp(p_dir_entry->d_name, ".") == 0 || strcmp(p_dir_entry->d_name, "..") == 0) {
                //IS SELF OR PARENT DIRECTORY
                //printf("self or parent directory to ignore: %s\n", p_dir_entry->d_name); //DEBUG
                continue; //(ignore it)
            }
            printf("%s is a directory\n", full_path); //DEBUG
            if(switch_conditions.use_recursive_processing) {
                //RECURSIVELY PROCESS THE SUB DIRECTORY
                //printf("directory to recursively process: %s\n", full_path); DEBUG
                process_directory(full_path, top_level_directory);
            }
        }
        else if(S_ISREG(stat_buffer.st_mode)) {
            //IS A FILE
            printf("%s is a regular file\n", full_path); //DEBUG
            if(p_dir_entry->d_name[0] == '.') {
                //IS A HIDDEN FILE
                printf("hidden file: %s\n", full_path); //DEBUG
                if(!switch_conditions.include_hiddens) {
                    printf("ignore hidden file: %s\n", full_path); //DEBUG
                    continue; //(ignore it)
                }
                printf("NOT IGNORED\n"); //DEBUG
            }
            //CHECK AGAINST IGNORE PATTERNS
            bool matches_ignore_pattern = false;
            for(int i = 0; i < switch_conditions.n_ignore_regex; ++i) {
                printf("ignore regex to test against: %s\n", switch_conditions.ignore_regex[i]);
                char *ignore_regex = strdup(switch_conditions.ignore_regex[i]);
                if(filename_matches_regex(full_path, ignore_regex)) {
                    matches_ignore_pattern = true;
                }
            }
            if(matches_ignore_pattern) {
                printf("ignore pattern match for file: %s, ignored\n", full_path); //DEBUG
                continue; //(ignore it)
            }
            //CHECK AGAINST ONLY PATTERNS
            if(switch_conditions.n_only_regex != 0) {
                //THERE ARE ONLY PATTERNS
                bool matches_only_pattern = false;
                for(int o = 0; o < switch_conditions.n_only_regex; ++o) {
                    printf("only regex to test against: %s\n", switch_conditions.only_regex[o]);
                    char *only_regex = strdup(switch_conditions.only_regex[o]);
                    if(filename_matches_regex(full_path, only_regex)) {
                        matches_only_pattern = true;
                    }
                }
                if(matches_only_pattern) {
                    //STORE THE FILE IN all_files
                    store_file(&all_files, &n_all_files, top_level_directory, stat_buffer.st_mtime, strdup(&full_path[strlen(top_level_directory)]), stat_buffer.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
                }
            }
            else { //if there aren't any only patterns it's safe to include the file
                //STORE THE FILE IN all_files
                store_file(&all_files, &n_all_files, top_level_directory, stat_buffer.st_mtime, strdup(&full_path[strlen(top_level_directory)]), stat_buffer.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
            }
        }
        else {
            fprintf(stderr, "%s is of unknown type.\n", full_path);
        }
    }
    closedir(p_directory);
}

void collect_every_directorys_files() { //STORE IN all_files
    //VALIDATE TOP LEVEL DIRECTORIES
    for(int d = 0; d < n_top_level_directories; ++d) {
        validate_top_level_directory(top_level_directories[d]);
    }
    //PROCESS TOP LEVEL DIRECTORIES
    for(int d = 0; d < n_top_level_directories; ++d) {
        process_directory(top_level_directories[d], top_level_directories[d]); //recursive function, one param is the directory to process, the other is its top_level_directory
    }
}

void collect_newest_unique_files() { //STORE IN unique_files
    printf("collecting newest unique files!\n"); //DEBUG
    sort_all_files_by_location_and_name();
    print_all_files(); //DEBUG
    //ITERATE OVER SORTED all_files AND STORE THE NEWEST VERSION OF EACH
    if(n_all_files != 0) {
        DIR_FILE file_to_store = all_files[0];
        for(int f = 0; f < n_all_files - 1; ++f) { //all files stored except the last (though it is compared against)
            char *file_a = all_files[f].location_and_name;
            char *file_b = all_files[f+1].location_and_name;
            if(strcmp(file_a, file_b) == 0) { //same names (files)
                if(all_files[f+1].modification_time > file_to_store.modification_time) {
                    file_to_store = all_files[f+1];
                }
            }
            else { //different names (files)
                store_file(&unique_files, &n_unique_files, file_to_store.top_level_directory, file_to_store.modification_time, file_to_store.location_and_name, file_to_store.permissions);
                file_to_store = all_files[f+1];
            }
        }
        store_file(&unique_files, &n_unique_files, file_to_store.top_level_directory, file_to_store.modification_time, file_to_store.location_and_name, file_to_store.permissions);
    }
    print_unique_files(); //DEBUG
}

void write_files_to_directories() {
    printf("writing files to directories:\n"); //DEBUG
    umask(0); //ensure that each file / directory created's permissions aren't modified by the system's umask value
    for(int f = 0; f < n_unique_files; ++f) {
        //FOR EACH UNIQUE FILE
        for(int d = 0; d < n_top_level_directories; ++d) {
            //FOR EACH TOP LEVEL DIRECTORY
            if(strcmp(unique_files[f].top_level_directory, top_level_directories[d]) != 0) {
                //...THAT IS NOT THE ONE THE FILE CAME FROM
                //printf("file: %s from directory: %s to write to directory: %s\n", unique_files[f].location_and_name, unique_files[f].top_level_directory, top_level_directories[d]); //DEBUG
                char full_path_new[MAXPATHLEN];
                sprintf(full_path_new, "%s%s", top_level_directories[d], unique_files[f].location_and_name);
                struct stat stat_buffer;
                if(stat(full_path_new, &stat_buffer) == -1) {
                    //FILE DOES NOT EXIST IN THIS DIRECTORY (to be written to)
                    if(switch_conditions.report_activity) {
                        printf("file: %s from directory: %s to be written to directory: %s\n", unique_files[f].location_and_name, unique_files[f].top_level_directory, top_level_directories[d]);
                    }
                    if(switch_conditions.synchronize) {
                        //printf("file: %s from directory: %s does not exist in directory: %s\n", unique_files[f].location_and_name, unique_files[f].top_level_directory, top_level_directories[d]); //DEBUG
                        char *file_directory = step_down_hierarchy(full_path_new);
                        ensure_directory_structure_exists(file_directory);
                        //CREATE THE FILE FROM THE STORED FILE
                        mode_t permissions_for_new_file;
                        if(switch_conditions.use_old_file_details) {
                            permissions_for_new_file = unique_files[f].permissions;
                        }
                        else {
                            permissions_for_new_file = 0666; //default for files
                        }
                        char full_path_stored[MAXPATHLEN];
                        sprintf(full_path_stored, "%s%s", unique_files[f].top_level_directory, unique_files[f].location_and_name);
                        bool create_not_update = true;
                        create_or_update_file_from_stored_file(create_not_update, full_path_new, full_path_stored, permissions_for_new_file, unique_files[f].modification_time);
                    }
                }
                else {
                    //FILES ALREADY EXISTS IN THIS DIRECTORY (to be updated)
                    if(switch_conditions.report_activity) {
                        printf("file: %s from directory: %s to be updated in directory: %s\n", unique_files[f].location_and_name, unique_files[f].top_level_directory, top_level_directories[d]);
                    }
                    if(switch_conditions.synchronize) {
                        //printf("file: %s from directory: %s exists in directory: %s\n", unique_files[f].location_and_name, unique_files[f].top_level_directory, top_level_directories[d]); //DEBUG
                        //UPDATE THE FILE FROM THE STORED FILE
                        mode_t permissions_for_new_file;
                        if(switch_conditions.use_old_file_details) {
                            permissions_for_new_file = unique_files[f].permissions;
                        }
                        else {
                            permissions_for_new_file = 0666; //default for files
                        }
                        char full_path_stored[MAXPATHLEN];
                        sprintf(full_path_stored, "%s%s", unique_files[f].top_level_directory, unique_files[f].location_and_name);
                        bool create_not_update = false;
                        create_or_update_file_from_stored_file(create_not_update, full_path_new, full_path_stored, permissions_for_new_file, unique_files[f].modification_time);
                    }
                }
            }
        }
    }
}