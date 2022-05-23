#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define ERROR 0
#define SUCCESS 1

typedef struct TREE{
    char* name;
    int numberOfItems;
    short type;
    size_t size;
    time_t date;
    char* content;
    struct TREE* previous;
    struct TREE* parent;
    struct TREE* next;
    struct TREE* child;
}TREE;

const short FOLDER = 2;
const short FILE_ = 1;

char *getString(){
    char *str;
    char a[100];
 
    str = (char*) malloc(sizeof(a));
    if(!str) 
        return ERROR;
    else 
        fgets(str, 100, stdin);
    
    return str;
}

TREE* getNode(TREE *currentFolder, char* name, short type) {

    if (currentFolder->child != NULL) {

        TREE *currentNode = currentFolder->child;

        while (currentNode->next != NULL) {

            if (strcmp(name, currentNode->name) == 0 && currentNode->type == type) {
                return currentNode;
            }
            currentNode = currentNode->next;
        }

        if (strcmp(name, currentNode->name) == 0 && currentNode->type == type) {
            return currentNode;
        } else return NULL;

    } else return NULL;
}

TREE* getNodeTypeless(TREE *currentFolder, char* name) {
    if (currentFolder->child) {
        TREE *currentNode = currentFolder->child;

        int res = strcmp(name, currentNode->name);

        while (currentNode->next) {
            if (!res) 
                return currentNode;
            currentNode = currentNode->next;
        }

        res = strcmp(name, currentNode->name);
        if (!res)  return currentNode;
        else return NULL;
    } 
    else return NULL;
}

TREE* set(TREE* newFolder, char *newFolderName) {
    newFolder->name = newFolderName;
    newFolder->type = FOLDER;
    newFolder->numberOfItems = 0;
    newFolder->size = 0;
    newFolder->content = NULL;
    newFolder->next = NULL;
    newFolder->child = NULL;

    return newFolder;
}

int mkdir(TREE *currfolder, char *new) {
    if (new) {
        if (!getNodeTypeless(currfolder, new)) {
            currfolder->numberOfItems++;

            TREE *newfolder = (TREE*) malloc(sizeof(TREE));
            if (!newfolder)
                return ERROR;

            if (!currfolder->child) {
                currfolder->child = newfolder;
                newfolder->previous = NULL;
                newfolder->parent = currfolder;
            } else {
                TREE *currentNode = currfolder->child;

                while (currentNode->next) 
                    currentNode = currentNode->next;
        
                currentNode->next = newfolder;
                newfolder->previous = currentNode;
                newfolder->parent = NULL;
            }

            int len = strlen(new);
            char* newFolderName = (char*) malloc(sizeof(char) * len);

            if (!newFolderName)
                return ERROR;

            strcpy(newFolderName, new);
            newfolder = set(newfolder, newFolderName);

        } else 
            printf("'%s' is already exist in current directory!\n",  new);
    }
}

TREE* createFile(TREE* newFile, char* newFileName) {
    newFile->name = newFileName;
    newFile->type = 1;
    newFile->numberOfItems = 0;
    newFile->size = 0;
    newFile->content = NULL;
    newFile->next = NULL;
    newFile->child = NULL;

    return newFile;
}

void touch(TREE *currentFolder, char *name) {
    if (name) {
        if (!getNodeTypeless(currentFolder, name)) {

            currentFolder->numberOfItems++;

            TREE *newFile = (TREE *) malloc(sizeof(TREE));

            if (!currentFolder->child) {
                currentFolder->child = newFile;
                newFile->previous = NULL;
                newFile->parent = currentFolder;
            } else {

                TREE *currentNode = currentFolder->child;

                while (currentNode->next) {
                    currentNode = currentNode->next;
                }
                currentNode->next = newFile;
                newFile->previous = currentNode;
                newFile->parent = NULL;
            }

            char* newFileName = (char*) malloc(sizeof(char));
            strcpy(newFileName, name);

            newFile = createFile(newFile, newFileName);

        } else {
            printf("'%s' is already exist in current directory!\n", name);
        }
    }
}

void ls(TREE *currentFolder) {
    if (currentFolder->child == NULL) printf("empty\n");
    else {
        TREE *currentNode = currentFolder->child;

        while (currentNode->next) {
            if (currentNode->type == FOLDER) printf("(D) %s %d items\n", currentNode->name, currentNode->numberOfItems);
            else printf("(F) %s\n", currentNode->name);
            
            currentNode = currentNode->next;
        }
        
        if (currentNode->type == FOLDER) 
            printf("(D) %s %d items\n", currentNode->name, currentNode->numberOfItems);
        else 
            printf("(F) %s\n", currentNode->name);
    }
}

int lsrecursive(TREE *currentFolder, char *str ,int indentCount) {
    //todo    
}


void pwd(char *path) {
    char *pointer;
    pointer = path;

    while(*pointer != '\0') 
        printf("%c", *pointer++);
}


TREE* cd(TREE *currentFolder, char *folder, char **path) {
    char *slash;
    slash = (char *)malloc(sizeof(char));
    strcpy(slash, "/");

    if(!slash)
        return ERROR;

    if (folder) {
        TREE *destinationFolder = getNode(currentFolder, folder, FOLDER);

        if (destinationFolder) {
            strcat(*path, destinationFolder->name);
            strcat(*path, slash);
            return destinationFolder;
        } else {
            printf("'%s' doesn't exist\n", folder);
            return currentFolder;
        }
    } else 
        return currentFolder;
        
    return currentFolder;
    
}

TREE* cdup(TREE *currentFolder, char **path) {
    size_t newPathLength = strlen(*path) - strlen(currentFolder->name);

    while (currentFolder->previous != NULL) {
        currentFolder = currentFolder->previous;
    }
    if (currentFolder->parent != NULL ) {

        *path = (char *) realloc(*path, sizeof(char)* newPathLength);
        (*path)[newPathLength-1] = '\0';

        currentFolder = currentFolder->parent;
        return currentFolder;
    } else {
        return currentFolder;
    }
}

void freeNode(TREE *freeingNode) {
    if (freeingNode->child) {

        TREE* currentNode = freeingNode->child;

        while (currentNode->next) {
            TREE* nextNode = currentNode->next;
            freeNode(currentNode);
            currentNode = nextNode;
        }
        freeNode(currentNode);
    }
    free(freeingNode->name);
    free(freeingNode->content);
    free(freeingNode);

}

void removeNode(TREE *removingNode) {
    if (removingNode->parent != NULL){
        if (removingNode->next != NULL) {
            removingNode->next->parent = removingNode->parent;
            removingNode->parent->child = removingNode->next;
            removingNode->next->previous = NULL;
        } else {
            removingNode->parent->child = NULL;
        }
    } else {
        if (removingNode->next != NULL) {
            removingNode->previous->next = removingNode->next;
            removingNode->next->previous  = removingNode->previous;
        } else {
            removingNode->previous->next = NULL;
        }
    }
}


void moveNode(TREE *movingNode, TREE *destinationFolder) {

    if (destinationFolder->child == NULL) {
        destinationFolder->child = movingNode;
        movingNode->previous = NULL;
        movingNode->parent = destinationFolder;
        movingNode->next = NULL;
    } else {
        TREE *currentNode = destinationFolder->child;
        while (currentNode->next != NULL) {
            currentNode = currentNode->next;
        }

        currentNode->next = movingNode;
        movingNode->previous = currentNode;
        movingNode->parent = NULL;
        movingNode->next = NULL;
    }
    destinationFolder->numberOfItems++;
}

void mov(TREE *currentFolder, char *command) {

    char* nodeName;
    char* destinationName;

    if (strtok(command, " ") != NULL) {
        nodeName = strtok(NULL, " ");
        if (nodeName != NULL) {
            destinationName = strtok(NULL, " ");
            if (destinationName != NULL) {
                if (strtok(NULL, " ")) {
                    return;
                } else {

                    TREE* movingNode = getNodeTypeless(currentFolder, nodeName);
                    TREE* destinationFolder = getNode(currentFolder, destinationName, FOLDER);

                    if (destinationFolder != NULL && movingNode != NULL && destinationFolder != movingNode) {

                        removeNode(movingNode);
                        moveNode(movingNode, destinationFolder);
                    } else {
                        fprintf(stderr, "Something you made wrong!\n");
                    }
                }
            }
        }
    }
}

char *splitString(char *string) {
    char delim[] = " ";
    
    string = strtok(string, delim);

    while(!string) string = strtok(NULL, delim);

    return string;
}

char *removeLineBreak(char *string) {
    int len = strlen(string);

    string[len - 1] == '\n' ? string[len - 1] = '\0' : string[len - 1];

    return string;
}

int rmdir(TREE *curr, char *str) {
    if (str) {
        TREE *removingNode = getNodeTypeless(curr, str);
        if(removingNode->type == FOLDER) {
            if (removingNode) {
                curr->numberOfItems--;
                removeNode(removingNode);
                freeNode(removingNode);
            }   
        } else 
            printf("'%s' is not exist in current directory!\n",  str);
        } else 
            printf("it's not a folder!\n");
}   

int rm(TREE *curr, char *str) {
     if (str) {
        TREE *removingNode = getNodeTypeless(curr, str);
        if(removingNode->type == FILE_) {
            if (removingNode) {
                removeNode(removingNode);
                freeNode(removingNode);
            } else 
                printf("'%s' is not exist in current directory!\n",  str);
        } else 
            printf("it's not a file!\n");
    }   
}

int exitProgram(char *command, TREE *root, char *path) {
    free(command);
    free(root);
    free(path);
}

TREE *rootCreate() {
    TREE *root = (TREE*)malloc(sizeof(TREE));
    char *name = (char *) malloc(sizeof(char));

    if(!root) 
        return ERROR;
    
    if(!name)
        return ERROR;
    
    root->type = FOLDER;
    root->name = name;
    root->numberOfItems = 0;
    root->size = 0;
    root->content =NULL;
    root->previous = NULL;
    root->parent = NULL;
    root->next = NULL;
    root->child = NULL;
    strcpy(name, "/");

    return root;
}

void menu() {
    puts("mkdir -arg\tcreate directory");
    puts("rmdir -arg\tdelete directory");
    puts("touch -arg\tcreate a file");
    puts("ls\t\tshow all contents of a directory");
    puts("rm -arg\t\tdelete file");
    puts("cd -arg\t\treturn to a previous directory");
    puts("renamedir -arg\trename a directory");
    puts("renamefile -arg\trename a file");
    puts("cd..\t\treturn to a previous directory");
    puts("pwd\t\tcurrent directory");
    puts("clear\t\tclear screen");
    puts("exit\t\texit program");

}

void clearScreen() {
    printf("\e[1;1H\e[2J");
}

int renamedir(TREE* curr, char* name, char* newname) {
    TREE *node = getNodeTypeless(curr, name);

    int len = strlen(name);

    if(!len)
        return ERROR;
    
    if(!node) {
        puts("something went wrong");
        return ERROR;
    }

    if(node->type == FOLDER) 
        strcpy(node->name, newname);
    else 
        puts("it's not a folder");
}

int renamefile(TREE* curr, char* name, char* newname) {
    TREE *node = getNodeTypeless(curr, name);

    int len = strlen(name);

    if(!len)
        return ERROR;
    
    if(!node) {
        puts("something went wrong");
        return ERROR;
    }

    if(node->type == FILE_) 
        strcpy(node->name, newname);
    else 
        puts("it's not a file");
}

void green() {
    printf("\033[0;32m");
}

void blue() {
    printf("\033[0;34m");
}

void reset() {
    printf("\033[0m");
}

int verifyString() {
    TREE *root = rootCreate(); 
    char a[100];
    TREE *currentFolder = root;
    char *path = (char *) malloc(sizeof(a));
    char *newFolder = (char *) malloc(sizeof(a));
    char *newname = (char *) malloc(sizeof(a));
    char *newnameAUX = (char *) malloc(sizeof(a));
    char *command = (char *) malloc(sizeof(a));
    int index;
    int auxname;
    bool aux;

    if(!path || !newFolder || !newFolder || !command)
        return ERROR;

    strcpy(path, "/");

    while (true) {
        green();
        printf("joao@joao:~");
        blue();
        pwd(path);
        green();
        printf("$ ");
        reset();

        command = getString();
        index = strlen(command);

        aux = false;
        auxname = 0;

        for(int i = 0; i < index; ++i) 
            if(command[i] == ' ') {
                aux = true;
                auxname++;
            }
        
        strcpy(newnameAUX, command);

        if(auxname >= 2) {
            newFolder = strtok(newnameAUX, " ");
            newFolder = strtok(NULL, " ");
            newname = strtok(command, " ");
            newname = strtok(NULL, " ");
            newname = strtok(NULL, " ");
            newname = removeLineBreak(newname);
        }
        
        if(aux && auxname < 2) {
            newFolder = strtok(command, " ");
            newFolder = strtok(NULL, " ");
            newFolder = removeLineBreak(newFolder);
        }
        
        command = splitString(command);
        command = removeLineBreak(command);

        if (!strcmp(command, "mkdir")) 
            mkdir(currentFolder, newFolder);

        else if (!strcmp(command, "touch")) 
            touch(currentFolder, newFolder);

        else if (!strcmp(command, "ls")) 
            ls(currentFolder);

        else if (!strcmp(command, "search")) 
            lsrecursive(currentFolder, newFolder ,0);

        else if (!strcmp(command, "pwd")) {
            pwd(path);
            printf("\n");
        }

        else if (!strcmp(command, "cd..")) 
            currentFolder = cdup(currentFolder, &path);
        
        else if (!strcmp(command, "renamedir")) 
            renamedir(currentFolder, newFolder, newname);

        else if (!strcmp(command, "renamefile")) 
            renamefile(currentFolder, newFolder, newname);

        else if (!strcmp(command, "cd"))
            currentFolder = cd(currentFolder, newFolder, &path);

        else if (!strcmp(command, "rmdir")) 
            rmdir(currentFolder, newFolder);

        else if (!strcmp(command, "rm")) 
            rm(currentFolder, newFolder);

        else if (!strcmp(command, "clear")) 
            clearScreen();

        else if (!strcmp(command, "mov")) 
            mov(currentFolder, command);

        else if (!strcmp(command, "help")) 
            menu();

        else if (!strcmp(command, "exit")) {
            exitProgram(command, root, path);
        }     
    }
    free(command);
    free(newnameAUX);
    free(newname);
}

int main() {
    verifyString();

    return 0;
}
