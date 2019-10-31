# simple-unix-file-system
Simplified version of the UNIX file system

# Commands implemented and their differences from standard UNIX if any:
+ pwd
+ cd - working dir can be changed only to the root dir, the working dir's parent, or one of the child dirs of the working dir
+ ls - takes 0 or 1 as arg; directories not sorted before files
+ cat
+ mkdir - takes only 1 arg
+ rmdir - takes only 1 arg
+ create - no UNIX counterpart; creates a text file with given name and reads contents of file from standard input
+ rm - takes only 1 arg
+ cp - takes only 2 args; doesn't allow overwriting exisitng file; does recursive copying of dirs (i.e. same as enabling -r for real UNIX cp)
+ mv - takes only 2 args, doesn't allow overwriting existing file
