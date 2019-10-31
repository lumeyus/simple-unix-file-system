#include <stdio.h>
#include <string.h>
#include "directory.h"

#define MAXLINESIZE 50
#define MAXARGSIZE 50
#define MAXARGSTRING "50"

#define TRUE 1
#define FALSE 0

void handle_create (struct entryNode * workingDir, char * args);
void handle_mkdir (struct entryNode * workingDir, char * args);
void handle_rm (struct entryNode * workingDir, char * args);
void handle_rmdir (struct entryNode * workingDir, char * args);
void handle_cp (struct entryNode * workingDir, char * args);
void handle_mv (struct entryNode * workingDir, char * args);
struct entryNode * handle_cd (struct entryNode * workingDir, char * args);
void handle_pwd (struct entryNode * workingDir, char * args);
void handle_ls (struct entryNode * workingDir, char * args);
void handle_cat (struct entryNode * workingDir, char * args);

int main ( ) {
	char cmd[7];
	char line[MAXLINESIZE];
	int tokenCount;
	int numCharsRead;
	struct entryNode * workingDir;
	
	workingDir = initialFileSystem ( );
	printf ("> ");
	while (fgets (line, MAXLINESIZE, stdin) != NULL) {
		tokenCount = sscanf (line, "%6s%n", cmd, &numCharsRead);
		if ((tokenCount == 0) || (cmd[0] == '\0')) {
			// blank line; do nothing
		} else if (strcmp (cmd, "create") == 0) {
			handle_create (workingDir, line+numCharsRead);
		} else if (strcmp (cmd, "mkdir") == 0) {
			handle_mkdir (workingDir, line+numCharsRead);
		} else if (strcmp (cmd, "rm") == 0) {
			handle_rm (workingDir, line+numCharsRead);
		} else if (strcmp (cmd, "rmdir") == 0) {
			handle_rmdir (workingDir, line+numCharsRead);
		} else if (strcmp (cmd, "cp") == 0) {
			handle_cp (workingDir, line+numCharsRead);
		} else if (strcmp (cmd, "mv") == 0) {
			handle_mv (workingDir, line+numCharsRead);
		} else if (strcmp (cmd, "cd") == 0) {
			workingDir = handle_cd (workingDir, line+numCharsRead);
		} else if (strcmp (cmd, "pwd") == 0) {
			handle_pwd (workingDir, line+numCharsRead);
		} else if (strcmp (cmd, "ls") == 0) {
			handle_ls (workingDir, line+numCharsRead);
		} else if (strcmp (cmd, "cat") == 0) {
			handle_cat (workingDir, line+numCharsRead);
		} else {
			printf ("%s: Command not found.\n", cmd);
		}
		printf ("> ");
		cmd[0] = '\0';
	}
	return 0;
}

void handle_create (struct entryNode * workingDir, char * args) {
	int argCount;
	char junk[2];
	char arg[MAXARGSIZE+1];
	argCount = sscanf (args, "%" MAXARGSTRING "s %1s", arg, junk);
	if (argCount > 1) {
		printf ("create: too many arguments\n");
	} else if (argCount == 0 || argCount == EOF) {
		printf ("usage: create fileName\n");
	} else {
		createFile (workingDir, arg);
	}
}

void handle_mkdir (struct entryNode * workingDir, char * args) {
	int argCount;
	char junk[2];
	char arg[MAXARGSIZE+1];
	argCount = sscanf (args, "%" MAXARGSTRING "s %1s", arg, junk);
	if (argCount > 1) {
		printf ("mkdir: too many arguments\n");
	} else if (argCount == 0 || argCount == EOF) {
		printf ("usage: mkdir directoryName\n");
	} else {
		createDir (workingDir, arg);
	}
}

void handle_rm (struct entryNode * workingDir, char * args) {
	int argCount;
	char junk[2];
	char arg[MAXARGSIZE+1];
	argCount = sscanf (args, "%" MAXARGSTRING "s %1s", arg, junk);
	if (argCount > 1) {
		printf ("rm: too many arguments\n");
	} else if (argCount == 0 || argCount == EOF) {
		printf ("usage: rm fileName\n");
	} else {
		removeFile (workingDir, arg);
	}
}

void handle_rmdir (struct entryNode * workingDir, char * args) {
	int argCount;
	char junk[2];
	char arg[MAXARGSIZE+1];
	argCount = sscanf (args, "%" MAXARGSTRING "s %1s", arg, junk);
	if (argCount > 1) {
		printf ("rmdir: too many arguments\n");
	} else if (argCount == 0 || argCount == EOF) {
		printf ("usage: rmdir directoryName\n");
	} else {
		removeDir (workingDir, arg);
	}
}

void handle_cp (struct entryNode * workingDir, char * args) {
	int argCount;
	char junk[2];
	char fromArg[MAXARGSIZE+1];
	char toArg[MAXARGSIZE+1];
	argCount = sscanf (args, "%" MAXARGSTRING "s %" MAXARGSTRING "s %1s", fromArg, toArg, junk);
	if (argCount > 2) {
		printf ("cp: too many arguments\n");
	} else if (argCount < 2) {
		printf ("usage: cp file1 file2\n");
	} else {
		copyFile (workingDir, fromArg, toArg);
	}
}

void handle_mv (struct entryNode * workingDir, char * args) {
	int argCount;
	char junk[2];
	char fromArg[MAXARGSIZE+1];
	char toArg[MAXARGSIZE+1];
	argCount = sscanf (args, "%" MAXARGSTRING "s %" MAXARGSTRING "s %1s", fromArg, toArg, junk);
	if (argCount > 2) {
		printf ("mv: too many arguments\n");
	} else if (argCount < 2) {
		printf ("usage: mv file1 file2\n");
	} else {
		moveFile (workingDir, fromArg, toArg);
	}
}

struct entryNode * handle_cd (struct entryNode * workingDir, char * args) {
	int argCount;
	char junk[2];
	char arg[MAXARGSIZE+1];
	argCount = sscanf (args, "%" MAXARGSTRING "s %1s", arg, junk);
	if (argCount > 1) {
		printf ("cd: too many arguments\n");
		return workingDir;
	} else if (argCount == 0 || argCount == EOF) {
		printf ("usage: cd directoryName\n");
		return workingDir;
	} else {
		return newWorkingDir (workingDir, arg);
	}
}

void handle_pwd (struct entryNode * workingDir, char * args) {
	int argCount;
	char junk[2];
	argCount = sscanf (args, "%1s", junk);
	if (argCount > 0) {
		printf ("usage: pwd\n");
	} else {
		printWorkingDir (workingDir);
	}
}

void handle_ls (struct entryNode * workingDir, char * args) {
	int argCount;
	char junk[2];
	char arg[MAXARGSIZE+1];
	argCount = sscanf (args, "%" MAXARGSTRING "s %1s", arg, junk);
	if (argCount > 1) {
		printf ("ls: too many arguments\n");
	} else if (argCount == 1) {
		listWithinWorkingDir (workingDir, arg);
	} else {
		listWorkingDir (workingDir);
	}
}

void handle_cat (struct entryNode * workingDir, char * args) {
	int argCount;
	int argSeen = FALSE;
	int numCharsRead;
	char arg[MAXARGSIZE+1];
	while (TRUE) {
		argCount = sscanf (args, "%" MAXARGSTRING "s%n", arg, &numCharsRead);
		if (argCount == 0 || argCount == EOF) {
			if (!argSeen) {
				printf ("usage: cat fileName ...\n");
			}
			return;
		}
		argSeen = TRUE;
		listFileContents (workingDir, arg);
		args = args + numCharsRead;
	}
}

