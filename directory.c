#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "directory.h"

#define TRUE 1
#define FALSE 0
#define STRBLOCK 8

struct entryNode {
	char * name;
	struct entryNode * next;	/* sibling */
	int isDirectory;
	struct entryNode * parent;
	union {
		char * contents;	/* for a text file */
		struct entryNode * entryList;	/* for a directory */
	} entry;
};

struct entryNode * root;

/* Helper functions */
void pwdHelper (struct entryNode *);
struct entryNode * located (char *, struct entryNode *);

/* Helper functions not given */
struct entryNode * newNode (char *, struct entryNode *, int, struct entryNode *, char *, struct entryNode *);
void deleteNode(struct entryNode *);
void removeEntry(struct entryNode *, struct entryNode *);
void copyDir(struct entryNode *, struct entryNode *);
void copyList(struct entryNode *, struct entryNode *);
struct entryNode * getPrev(struct entryNode *);
void addAlphabetically(struct entryNode *, struct entryNode *);
void replaceEntry(struct entryNode *, struct entryNode *, struct entryNode *);
void placeNext (struct entryNode *, struct entryNode *, struct entryNode *);
char * getFileText();
char * allocateString(char *);
int alphabetical(char *, char *);

/* Return an initialized file system (an empty directory named "/") 
   after storing it in the root variable. */
struct entryNode * initialFileSystem ( ) {
	root = newNode("/", NULL, 1, root, NULL, NULL);
	return root;
}

/* implements the "create" command (one argument; not in standard UNIX) */
void createFile (struct entryNode * wd, char * fileName) {
	struct entryNode * newFile;
	if (located (fileName, wd->entry.entryList)) {
		printf ("create: %s: File exists\n", fileName);
	}
	else {
		char * fileText = getFileText();
		newFile = newNode(fileName, NULL, 0, wd, fileText, NULL);
		addAlphabetically(wd, newFile);
	}
}

/* implements the "mkdir" command (one argument; no options) */
void createDir(struct entryNode * wd, char * dirName) {
	struct entryNode * newDir;
	if (located(dirName, wd->entry.entryList)) {
		printf("mkdir: %s: File exists\n", dirName);
	}
	else {
		newDir = newNode(dirName, NULL, 1, wd, NULL, NULL);
		addAlphabetically(wd, newDir);
	}
}

/* implements the "rm" command (one argument, unlike standard UNIX; no options) */
/* fileName is the name of the text file to delete */
void removeFile (struct entryNode * wd, char * fileName) {
	struct entryNode * file;
	file = located (fileName, wd->entry.entryList);
	if (file == NULL) {
		printf ("rm: %s: No such file or directory.\n", fileName);
	} else if (file->isDirectory) {
		printf ("rm: %s: is a directory.\n", fileName);
	} else {
		removeEntry(wd, file);
	}
}

/* implements the "rmdir" command (one argument, unlike standard UNIX; no options) */
/* dirName is the name of the directory to delete */
void removeDir (struct entryNode * wd, char * dirName) {
	struct entryNode * dir;
	dir = located (dirName, wd->entry.entryList);
	if (dir == NULL) {
		printf ("rmdir: %s: No such file or directory.\n", dirName);
	} else if (!dir->isDirectory) {
		printf ("rmdir: %s: Not a directory.\n", dirName);
	} else if (dir->entry.entryList != NULL) {
		printf ("rmdir: %s: Directory not empty\n", dirName);
	} else {
		removeEntry(wd, dir);
	}
}

/* implements the "cp" command (two named arguments, no options) */
/* "from" must name an existing file or directory.
   It's copied--a deep copy is made in the case of a directory--and the
   copy is given the "to" argument as its name.
   It's an error if "to" names an existing entry in the working directory.
 */
void copyFile (struct entryNode * wd, char * from, char * to) {
	struct entryNode * file;
	file = located (from, wd->entry.entryList);
	if (file == NULL) {
		printf ("cp: %s: No such file or directory.\n", from);
	} else {
		if (located(to, wd->entry.entryList)) { /* error if "to" in wd */
			printf("cp: %s: File already exists in directory.\n", to);
		}
		else if (file->isDirectory) { /* file is a directory */
			struct entryNode * head = file->entry.entryList;
			struct entryNode * newHead = newNode(head->name, head->next, head->isDirectory, file, head->entry.contents, head->entry.entryList);
			struct entryNode * newDir = newNode(to, NULL, 1, wd, NULL, newHead);
			addAlphabetically(wd, newDir);
			copyList(file->entry.entryList, newDir);
		}
		else { /* add file to wd */
			struct entryNode * newFile = newNode(to, NULL, 0, wd, file->entry.contents, NULL);
			addAlphabetically(wd, newFile);
		}
	}
}

/* implements the "mv" command (two named arguments, no options) */
/* "from" must name an existing file or directory.
   If "to" doesn't name an existing file or directory, the "from" entry is renamed.
   If "to" names an existing directory, that directory can't already contain 
   an entry named "from"; the "from" entry is removed from the working directory
   and added to the "to" directory.
   It's an error if "to" names an existing file.
 */
	/*
		removes the named file from the working directory's entries list, renames it, and
		inserts it either back into the working directory's entries list or into the specified
		subdirectory's entries list. Checks to ensure that the move does not replace an
		existing file.
	*/
void moveFile (struct entryNode * wd, char * from, char * to) {
	struct entryNode * file;
	file = located (from, wd->entry.entryList);
	if (file == NULL) {
		printf ("mv: %s: No such file or directory.\n", from);
	} else {
		struct entryNode * check = located(to, wd->entry.entryList);
		if (check) { /* file with name "to" exists */
			if (!check->isDirectory) { /* if it's a file then give error */
				printf("mv: %s: file with name already exists.\n", to);
				return;
			}
			else { /* else check if dir has "from" in it */
				struct entryNode * inDir = located(from, check->entry.entryList);
				if (inDir) {
					printf("mv: %s: directory already exists in ", from);
					printf("%s\n", to);
					return;
				}
				else {
					if (file->isDirectory) {
						struct entryNode * newDir = newNode(file->name, NULL, 1, check, NULL, file->entry.entryList);
						addAlphabetically(check, newDir);
						file->entry.entryList = NULL;
						removeDir(wd, file->name);
					}
					else {
						char * filecontents = allocateString(file->entry.contents);
						struct entryNode * newFile = newNode(file->name, NULL, 0, check, filecontents, NULL);
						addAlphabetically(check, newFile);
						removeFile(wd, file->name);
					}
				}
			}
		}
		else { /* rename "from" */
			if (file->isDirectory) {
				struct entryNode * newDir = newNode(to, NULL, 1, wd, NULL, file->entry.entryList);
				file->entry.entryList = NULL;
				addAlphabetically(wd, newDir);
				removeDir(wd, file->name);
			}
			else {
				char * filecontents = allocateString(file->entry.contents);
				struct entryNode * newFile = newNode(to, NULL, 0, wd, filecontents, NULL);
				addAlphabetically(wd, newFile);
				removeFile(wd, file->name);
			}
		}
	}
}

/* my helper functions */

/* allocates memory for each of a new nodes variables */
struct entryNode * newNode(char * name, struct entryNode * next, int isDirectory, struct entryNode * parent, char * contents, struct entryNode * entryList) {
	struct entryNode * node = malloc(sizeof(struct entryNode));
	node->name = allocateString(name);
	node->isDirectory = isDirectory;
	node->next = next;
	node->parent = parent;
	if (isDirectory) {
		node->entry.entryList = entryList;
	}
	else {
		node->entry.contents = contents;
	}
	return node;
}

/* deallocates memory for parts of node that require allocation beyond struct memory */
void deleteNode(struct entryNode * node) {
	free(node->entry.contents);
	free(node->name);
	free(node);
	return;
}

/* removes node from wd by checking node placement in relation to wd */
/* sets adjacent node pointers correctly based off placement */
void removeEntry(struct entryNode * wd, struct entryNode * toRemove) {
	if (toRemove == wd->entry.entryList) {
		if (toRemove->next == NULL) { /* only entry in entryList */
			wd->entry.entryList = NULL;
		}
		else { /* change head to toRemove's next */
			wd->entry.entryList = toRemove->next;
		}
	}
	else {
		struct entryNode * prev = getPrev(toRemove);
		if (toRemove->next == NULL) { /* end of list */
			prev->next = NULL;
		}
		else { /* delete from within list */
			prev->next = toRemove->next;
		}
	}
	deleteNode(toRemove);
}

/* copies directory into given destination */
void copyDir(struct entryNode * destination, struct entryNode * directory) {
	if (located(directory->name, destination->entry.entryList)) { /* directory is already in destination */
		return;
	}
	else {
		struct entryNode * newDir = newNode(directory->name, directory->next, 1, destination, NULL, directory->entry.entryList);
		addAlphabetically(destination, newDir);
		if (newDir->entry.entryList != NULL) { /* if directory has files, need to copy those as well */
			copyList(newDir->entry.entryList, newDir);
		}
	}
}

/* given the head of a directory's entryList, copies all entries in list into directory */
void copyList(struct entryNode * current, struct entryNode * directory) {
	if (current == NULL) {
		return;
	}
	else if (current->isDirectory) {
		copyDir(directory, current);
		if (current->next != NULL) {
			copyList(current->next, directory);
		}
	}
	else {
		while (current->next) {
			struct entryNode * node = newNode(current->name, current->next, current->isDirectory, directory, current->entry.contents, NULL);
			addAlphabetically(directory, node);
			copyList(current->next, directory);
		}
	}
}

/* returns node that precedes input node in node parent's entryList */
struct entryNode * getPrev(struct entryNode * node) {
	struct entryNode * previous = node->parent->entry.entryList;
	while (previous->next != node) {
		previous = previous->next;
	}
	return previous;
}

/* places node in alphabetical order inside a wd's entryList */
/* assumed that the newNode has already been checked to not replace the wd's entryList head */
/* list is the current node within the list, next is the current list's next, newNode is node to add */
void placeNext(struct entryNode * list, struct entryNode * next, struct entryNode * newNode) {
	if (next == NULL) { /* newNode is next */
		list->next = newNode;
	}
	else if (alphabetical(next->name, newNode->name)) { /* newNode comes before next */
		list->next = newNode;
		newNode->next = next;
	}
	else { /* newNode comes after next */
		placeNext(next, next->next, newNode);
	}
}

/* adds node alphabetically */
void addAlphabetically(struct entryNode * wd, struct entryNode * toAdd) {
	if (wd->entry.entryList == NULL) {
		wd->entry.entryList = toAdd;
	}
	else if (alphabetical(wd->entry.entryList->name, toAdd->name)) {
		replaceEntry(wd, wd->entry.entryList, toAdd);
	}
	else {
		placeNext(wd->entry.entryList, wd->entry.entryList->next, toAdd);
	}
}

/* replaces head of wd entryList with replacement node */
void replaceEntry(struct entryNode * wd, struct entryNode * wdEntry, struct entryNode * replacementNode) {
	wd->entry.entryList = replacementNode;
	replacementNode->next = wdEntry;
}

/* takes an input string for file text and dynamically allocates it in memory */
char * getFileText() {
	printf("Enter file text:\n");
	size_t stringAllocSize = STRBLOCK;
	char * tempInputString = malloc(stringAllocSize);
	int c;
	unsigned int i = 0;
	while ((c = getchar()) != '\0') {
		if (c == '\n' && i > 0) { /* check for second null char */
			if (tempInputString[i - 1] == '\n') {
				break;
			}
		}
		tempInputString[i++] = c;
		if (i == stringAllocSize) { /* resize when current alloc amount is reached */
			stringAllocSize += STRBLOCK;
			char * newStr = realloc(tempInputString, stringAllocSize);
			tempInputString = newStr;
		}
	}
	if (i == stringAllocSize) { /* allocate space for null char if needed */
		char * newStr = realloc(tempInputString, ++stringAllocSize);
		tempInputString = newStr;
	}
	tempInputString[i] = '\0';
	char * returnString;
	returnString = malloc(sizeof(char) * i); /* realloc with final size */
	strcpy(returnString, tempInputString);
	free(tempInputString);
	return returnString;
}

/* dynamically allocates memory for string found at given pointer */
char * allocateString(char * name) {
	int i = 0;
	while (name[i] != '\0') {
		i++;
	}
	char * string = malloc(sizeof(char) * i);
	strcpy(string, name);
	return string;
}

/* returns alphabetical order of given input strings */
/* 1 if word1 goes after word 2, 0 otherwise */
/* originally implemented just returning first char comparison;
   current "messy" impl is due to adjusting after rest of code was made */
int alphabetical(char * word1, char * word2) {
	int comp = strcmp(word1, word2);
	if (comp < 0) {comp = 0;}
	return comp;
}

/* ------------------- */

/* implements the "cd" command (one argument, which may be ".." or "/"; no options) */
struct entryNode * newWorkingDir (struct entryNode * wd, char * dirName) {
	struct entryNode * newWd;
	if (strcmp (dirName, "/") == 0) {
		return root;
	} else if (strcmp (dirName, "..") == 0) {
		return wd->parent;
	} else {
		newWd = located (dirName, wd->entry.entryList);
		if (newWd == NULL) {
			printf ("cd: %s: No such file or directory.\n", dirName);
			return wd;
		} else if (!newWd->isDirectory) {
			printf ("cd: %s: Not a directory.\n", dirName);
			return wd;
		} else {
			return newWd;
		}
	}
}

/* implements the "pwd" command (no arguments; no options) */
void printWorkingDir (struct entryNode * wd) {
	if (strcmp (wd->name, "/") == 0) {
		printf ("/\n");
	} else {
		pwdHelper (wd);
		printf ("\n");
	}
}

void pwdHelper (struct entryNode * wd) {
	if (strcmp (wd->name, "/") != 0) {
		pwdHelper (wd->parent);
		printf ("/%s", wd->name);
	}
}

/* implements the "ls" command (0 or 1 argument, unlike standard UNIX; no options) */
/* Behavior is as follows:
	if no arguments, list the names of the files in wd;
	if one argument that names a text file in wd, echo the argument;
	if one argument that names a directory d in wd, list the names of the files in d;
	if one argument that names nothing in wd, print
		ls: ___: No such file or directory
 */
void listWorkingDir (struct entryNode * wd) {
	struct entryNode * p  = wd->entry.entryList;
	while (p != NULL) {
		printf ("%s\n", p->name);
		p = p->next;
	}
}

void listWithinWorkingDir (struct entryNode * wd, char * name) {
	struct entryNode * entryPtr;
	entryPtr = located (name, wd->entry.entryList);
	if (entryPtr == NULL) {
		printf ("ls: %s: No such file or directory\n", name);
	} else if (entryPtr->isDirectory) {
		listWorkingDir (entryPtr);
	} else {
		printf ("%s\n", name);
	}
}

/* implements the "cat" command (arbitrary number of arguments, which all must
   name text files; no options) */
/* This function prints the contents of a single file with the given name. */
void listFileContents (struct entryNode * wd, char * name) {
	struct entryNode * entryPtr;
	entryPtr = located (name, wd->entry.entryList);
	if (entryPtr == NULL) {
		printf ("cat: %s: No such file or directory\n", name);
	} else if (entryPtr->isDirectory) {
		printf ("cat: %s: Operation not permitted\n", name);
	} else {
		printf ("%s", entryPtr->entry.contents);
	}
}

/* Return a pointer to the entry with the given name in the given list,
   or NULL if no such entry exists. */
struct entryNode * located (char * name, struct entryNode * list) {
	if (list == NULL) {
		return NULL;
	} else if (strcmp (list->name, name) == 0) {
		return list;
	} else {
		return located (name, list->next);
	}
}