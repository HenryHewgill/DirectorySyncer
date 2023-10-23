//SHARED FUNCTIONS
extern void collect_every_directorys_files(); //collects all top level directories files (as outlined by switches)
extern void collect_newest_unique_files(); //sorts all_files by location_and_name, collects the newest version of each file in all_files, stores them in unique_files
extern void write_files_to_directories(); //writes files to directories, effectively performing the sync