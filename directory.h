struct entryNode;

struct entryNode * initialFileSystem ( );

/* implements the "create" command (one argument; not in standard UNIX) */
void createFile (struct entryNode * wd, char *);

/* implements the "mkdir" command (one argument; no options) */
void createDir (struct entryNode * wd, char *);

/* implements the "rm" command (one argument, no options) */
void removeFile (struct entryNode * wd, char *);

/* implements the "rmdir" command (one argument, no options) */
void removeDir (struct entryNode * wd, char *);

/* implements the "cp" command (two arguments, no options) */
void copyFile (struct entryNode * wd, char * from, char * to);	

/* implements the "mv" command (two arguments, no options) */
void moveFile (struct entryNode * wd, char * from, char * to);	

/* implements the "cd" command (one argument, which may be ".."; no options) */
struct entryNode * newWorkingDir (struct entryNode * wd, char *);

/* implements the "pwd" command (no arguments; no options) */
void printWorkingDir (struct entryNode * wd);

/* implements the "ls" command (0 or 1 argument; no options) */
void listWorkingDir (struct entryNode *);
void listWithinWorkingDir (struct entryNode *, char *);

/* implements the "cat" command (arbitrary number of arguments; no options) */
void listFileContents (struct entryNode * wd, char * args);
