Background
We increasingly use multiple computers, such as those at university, at places of employment, personal desktop computers, and laptops. Unfortunately, because cloud-based services such as Dropbox and OneDrive are not always available, we may have only a subset of our files on the computer we're using. We (should also) employ portable storage devices, such as USB thumb-drives to backup and transport copies of our files between computers but, over time, we need to manually identify the most up-to-date version of a file before we overwrite another file of the same name.
We'd like to synchronise files between our computers and our USB thumb-drive (so that both hold the most up-to-date copy of the same files), travel home, and then synchronise our USB thumb-drive with our home computer or laptop. Eventually, all three locations will hold identical copies of the same files.

The activity of ensuring that two sets of files are identical, is termed file synchronization, or just syncing.

For this project, two sets of files are deemed identical if each file exists in the same relative location below a 'top-level' directory, with the same filename, and with identical contents.

The goal of this project is to design and develop a command-line utility program, named mysync, to synchronise the contents of two or more directories.
 
Successful completion of the project will develop and enhance your understanding of advanced features of the C11 programming language, POSIX function calls, and the use of the make utility.

Program invocation
Your program will be invoked from the command-line with zero-or-more options (switches) and two-or-more directory names:
prompt> ./mysync  [options]  directory1  directory2  [directory3  ...]

The program's options are:

-a	By default, 'hidden' and configuration files (those beginning with a '.' character) are not synchronised because they do not change very frequently. The -a option requests that all files, regardless of whether they begin with a '.' or not, should be considered for synchronisation.
-i pattern	Filenames matching the indicated pattern should be ignored; all other (non-matching) filenames should be considered for synchronisation. The -i option may be provided multiple times.
-n	By default, mysync determines what files need to be synchronised and then silently performs the necessary copying. The -n option requests that any files to be copied are identified, but they are not actually synchronised. Setting the -n option also sets the -v option.
-o pattern	Only filenames matching the indicated pattern should be considered for synchronisation; all other (non-matching) filenames should be ignored. The -o option may be provided multiple times.
-p	By default, after a file is copied from one directory to another, the new file will have the current modification time and default file permissions. The -p option requests that the new copy of the file have the same modification time and the same file permissions as the old file.
-r	By default, only files found immediately within the named directories are synchronised. The -r option requests that any directories found within the named directories are recursively processed.
-v	By default, mysync performs its actions silently. No output appears on the stdout stream, although some errors may be reported on the stderr stream. The -v option requests that mysync be more verbose in its output, reporting its actions to stdout. There is no required format for the (your) debug printing (it will be ignored during marking).
File patterns
Globbing, also known as wildcard expansion, is a feature in Unix-like operating systems (including Linux and macOS) that allows you to use special characters, termed wildcards, to match and specify multiple files and directories in a concise way. The wildcard characters *, ?, [ ], and { } are widely supported. Normally, globbing is performed by the shell before it passes the expanded patterns as arguments to a new child process.
Within our own programs, such as mysync with its -i and -o options, we need to perform globbing ourselves. A simple way to determine if a glob-pattern matches a given filename, is to first convert the glob-pattern to a regular-expression, and to then see if the regular expression matches the filename. The provided function glob2regex performs the first part of this process, and the library functions regcomp and regexec, used in sequence, perform the second.

Note that, because the shell expands wildcards, that you'll need to enclose your file patterns within single-quotation characters. For example, the following command will (only) synchronise your C11 files:

prompt> ./mysync  -o  '*.[ch]'  ....

and the following command will synchronise all files except your laboratory exercises:

prompt> ./mysync  -i  'lab?-Q*.*'  ....

Examples
Assume that you have a USB thumb-drive, imaginatively named "myUSB", and that you plug it in:
if using a CSSE lab Linux computer, your thumb-drive should be mounted at "/media/UNIWA  2xxxxxxx/myUSB" (yes, there's an annoying space in that pathname).
if using your own laptop running macOS, your thumb-drive should be mounted at "/Volumes/myUSB".
if using your own laptop running Linux, you can use the df command to find the name of your thumb-drive's mount point.
Consider the following typical invocations of mysync (which employ the simpler macOS pathname):

prompt> ./mysync  ~  /Volumes/myUSB
this command will synchronise files between your home directory and your USB thumb-drive.

prompt> ./mysync  -a  /Volumes/myUSB  ~
this command will synchronise all files (including configuration files) between your home directory and your USB thumb-drive.

prompt> ./mysync  -n  -r  /Volumes/myUSB  cits2002
this command will recursively determine which files require synchronization, it will report the actions that would be taken in synchronizing them, but no actual copying will be performed.

prompt> ./mysync  -p  -v  /Volumes/myUSB  cits2002
this command will synchronise Systems Programming directory and your USB thumb-drive, ensuring that all files eventually have the same modification time and protection modes. At the same time, all actions taken will be reported.

prompt> ./mysync  -r  /Volumes/myUSB  cits2002  ~/backup/cits2002
this command will recursively synchronise files between 3 directories - your Systems Programming (working) directory, your USB thumb-drive, and a local copy of all your cits2002 work.

WARNING: until you are very confident that your mysync program is working, and is copying files correctly, you are strongly advised NOT to use your mysync program to backup your project!

Getting started
There is no required sequence of steps to undertake the project, and no sequence of steps will guarantee success. However, the following sequence is strongly recommended (and this is how the sample solution was built). It is assumed (considered essential for success!) that each step:
is extensively tested before you proceed to the next step, and
if any errors are found, the first error should be reported to stderr, and the program should immediately terminate.
The recommended initial steps (some will take 2 minutes, others make take a day):

find a project partner.
determine what activities are to be performed by the program. Determine what data needs to be stored during the execution of the program. Design one of more data types and data structures to store this long-term data, and create a new mysync.h header file to define these constructs.
break up the activities into fairly independent sets. Write a number of "empty" C files, each of which includes your mysync.h header file, to implement the activities. Ensure that the main() function is in a file named mysync.c
write a Makefile to compile and link your C files, each of which will be dependent on the header file.
write the main() function, whose initial task is simply to receive and validate the command-line options and arguments. Write a usage() function to report the program's synopsis, should command-line processing detect any errors.
ensure that the actual command-line arguments are the names of directories that you can read.
open and read each directory, locating all regular files in the directory. At this stage, limit your project to support just two directories, and to just print each filename as it is found.
add support for the -a command-line option when finding files.
store the name (location) and modification time of each file found in each directory.
having finished reading the directories, identify all files that exist in one directory but not in the other, or the newer of two files if they exist in both directories.
if the -n command-line option is not provided, copy the unique (or newer) file from one directory to the other.
add support for three or more directories.
add support for the -r command-line option when finding files.
add support for the -p command-line option when copying files.
 armed with a fully tested program, and overflowing with confidence, support the -i and -o options.

It is anticipated that a successful project will use (most of) the following Linux system-calls, and standard C11 & POSIX functions: 

perror, getopt, malloc, realloc, strdup, free, opendir, readdir, closedir, stat, mkdir, open, read, write, close, chmod, utime, and  glob2regex(provided), regcomp and regexec.
