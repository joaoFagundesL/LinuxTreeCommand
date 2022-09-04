#include <stdio.h> 

#include <string.h> // strlen, strcpy, strcmp

#include <stdbool.h> 

#include <stdlib.h> 

#define ERROR 0 
#define SUCCESS 1 

typedef struct TREE {
  struct TREE * child; // pointer to the child
  struct TREE * previous; // pointer to the previous
  struct TREE * next; // pointer to the next
  struct TREE * parent; // pointer to the parent

  char * name; // name of the node
  short quantity; // quantity of files in the folder
  short type; // type of the node
}
TREE; 

// Global variables: identify the type of the node
const short FOLDER = 2; 
const short FILE_ = 1; 

char * getString() { // get string from the user
  char * str;
  char a[100];

  str = (char * ) malloc(sizeof(a)); // allocate memory for the string
  if (!str) // if there is no memory
    return ERROR;

  else fgets(str, 100, stdin);
  return str;
}

TREE * getnodetype(TREE * currfolder, char * name, short type) { //if i should know the type of the node
  if (currfolder -> child) {
    TREE * currnode = currfolder -> child;

    while (currnode -> next) { // go through the list of the nodes
      if (strcmp(name, currnode -> name) == 0 && currnode -> type == type)
        return currnode; // if the node is found

      currnode = currnode -> next;
    }

    if (strcmp(name, currnode -> name) == 0 && currnode -> type == type) 
      return currnode;

    else return NULL;
  } else return NULL;
}

TREE * getnode(TREE * currfolder, char * name) { // if i shouldn't know the type of the node 
  if (currfolder -> child) {
    TREE * currnode = currfolder -> child;

    int res = strcmp(name, currnode -> name);

    while (currnode -> next) {
      if (!res) return currnode;
      currnode = currnode -> next;
    }

    res = strcmp(name, currnode -> name);
    if (!res) return currnode;
    else return NULL;
  } else return NULL;
}

TREE * set(TREE * newFolder, char * newFolderName) { // set new folder
  newFolder -> name = newFolderName;
  newFolder -> type = FOLDER;
  newFolder -> quantity = 0;
  newFolder -> next = NULL;
  newFolder -> child = NULL;

  return newFolder;
}

int mkdir(TREE * currfolder, char * new) {
  if (new) {
    if (!getnode(currfolder, new)) {
      currfolder -> quantity++;
      TREE * newfolder = (TREE * ) malloc(sizeof(TREE));
      if (!newfolder) return ERROR;

      if (!currfolder -> child) {
        currfolder -> child = newfolder;
        newfolder -> previous = NULL;
        newfolder -> parent = currfolder;
      } else {
        TREE * currnode = currfolder -> child;

        while (currnode -> next)
          currnode = currnode -> next;

        currnode -> next = newfolder;
        newfolder -> previous = currnode;
        newfolder -> parent = NULL;
      }

      int len = strlen(new);
      char * newFolderName = (char * ) malloc(sizeof(char) * len);
      if (!newFolderName) return ERROR;
      strcpy(newFolderName, new);
      newfolder = set(newfolder, newFolderName);

    } else
      printf("'%s' is already exist in current directory!\n", new);
  }
}

TREE * createFile(TREE * newFile, char * newFileName) { // create new file
  newFile -> name = newFileName;
  newFile -> type = FILE_;
  newFile -> quantity = 0; // quantity of files in the folder
  newFile -> next = NULL;
  newFile -> child = NULL;

  return newFile;
}

void setNode_touch(TREE * currnode, TREE * newFile) {
  currnode -> next = newFile; 
  newFile -> previous = currnode;
  newFile -> parent = NULL;
}

int touch(TREE * currfolder, char * name) {
  if (name) {
    TREE * currnode;
    if (!getnode(currfolder, name)) { // if file not exist

      currfolder -> quantity++;
      TREE * newFile = (TREE * ) malloc(sizeof(TREE));

      if (!newFile) // if memory allocation failed
        return ERROR;

      if (!currfolder -> child) { // if there is no child
        currfolder -> child = newFile;
        newFile -> previous = NULL;
        newFile -> parent = currfolder;
      } else {
        currnode = currfolder -> child;

        while (currnode -> next) // find the last node
          currnode = currnode -> next;

        setNode_touch(currnode, newFile); // it is gonna set the pointer of the currnode and newFile
      }

      char * newFileName = (char * ) malloc(sizeof(char));
      strcpy(newFileName, name);

      newFile = createFile(newFile, newFileName);

    } else
      printf("'%s' is already exist in current directory!\n", name);
  }
}

void ls(TREE * currfolder) { // list all files and folders
  if (currfolder -> child == NULL) printf("empty\n");
  else {
    TREE * currnode = currfolder -> child;

    while (currnode -> next) { // print all files
      if (currnode -> type == FOLDER) printf("(D) %s (%d items)\n", currnode -> name, currnode -> quantity);
      else printf("(F) %s\n", currnode -> name);
      currnode = currnode -> next; // go to the next
    }
    currnode -> type == FOLDER ? printf("(D) %s (%d items)\n", currnode -> name, currnode -> quantity) : printf("(F) %s\n", currnode -> name);
  }
}

void pwd(char * path) { // print current path
  char * pointer; // pointer to the path
  pointer = path; // set pointer to the path

  while ( * pointer != '\0') // print path
    printf("%c", * pointer++);
}

TREE * cd(TREE * currfolder, char * folder, char ** path) {
  char * slash;
  slash = (char * ) malloc(sizeof(char));
  strcpy(slash, "/"); 

  if (!slash) 
    return ERROR;

  if (folder) { // if there is a folder name
    TREE * destinationFolder = getnodetype(currfolder, folder, FOLDER); // get the folder

    if (destinationFolder) { // if folder exist
      strcat( * path, destinationFolder -> name);
      strcat( * path, slash);
      return destinationFolder;
    } else { // if folder doesn't exist
      printf("'%s' doesn't exist\n", folder);
      return currfolder;
    }
  } else
    return currfolder; // if there is no folder name

  return currfolder; 

}

TREE * cdprevious(TREE * currfolder, char ** path) {
  int pathlen = strlen( * path); 
  int namelen = strlen(currfolder -> name);
  int len = pathlen - namelen; // length of the path

  while (currfolder -> previous)
    currfolder = currfolder -> previous;

  if (currfolder -> parent) { // if there is a parent
    ( * path)[--len] = '\0';
    return currfolder -> parent;
  } else return currfolder; // if there is no parent
}

void freeNode(TREE * freeingNode) {
  TREE * currnode;
  TREE * nextNode;

  if (freeingNode -> child) {
    currnode = freeingNode -> child;
    while (currnode -> next) {
      nextNode = currnode -> next;
      freeNode(currnode);
      currnode = nextNode;
    }
    freeNode(currnode);
  }
  free(freeingNode -> name);
  free(freeingNode);
}

void removeNode(TREE * removingNode) {
  if (removingNode -> parent) {
    if (removingNode -> next) {
      removingNode -> next -> parent = removingNode -> parent;
      removingNode -> parent -> child = removingNode -> next;
      removingNode -> next -> previous = NULL;
    } else
      removingNode -> parent -> child = NULL;
  } else {
    if (removingNode -> next) {
      removingNode -> previous -> next = removingNode -> next;
      removingNode -> next -> previous = removingNode -> previous;
    } else
      removingNode -> previous -> next = NULL;

  }
}

char * splitString(char * string) { // split string by ' '
  char delim[] = " ";

  string = strtok(string, delim); 

  while (!string) string = strtok(NULL, delim); 

  return string;
}

char * removeLineBreak(char * string) { 
  int len = strlen(string);

  string[len - 1] == '\n' ? string[len - 1] = '\0' : string[len - 1];

  return string; // return the string without '\n'
}

int rmdir(TREE * curr, char * str) {  // remove directory
  if (str) {
    TREE * removingNode = getnode(curr, str);
    if (removingNode -> type == FOLDER) { // if removingNode is folder
      if (removingNode) {
        curr -> quantity--;
        removeNode(removingNode);
        freeNode(removingNode);
      }
    } else printf("'%s' is not exist!\n", str);
  } else printf("it's not a folder!\n");  // if removingNode is file
}

int rm(TREE * curr, char * str) { //remove file
  if (str) {
    TREE * removingNode = getnode(curr, str); 
    if (removingNode -> type == FILE_) {  //if file
      if (removingNode) {
        removeNode(removingNode);
        freeNode(removingNode);
      } else printf("'%s' is not exist\n", str);
    } else printf("it's not a file!\n"); //if folder
  }
}

int exitProgram(char * command, TREE * root, char * path) {
  free(command);
  free(root);
  free(path);
}

TREE * rootCreate() { 
  TREE * root = (TREE * ) malloc(sizeof(TREE));
  char * name = (char * ) malloc(sizeof(char));

  if (!root)
    return ERROR;

  if (!name)
    return ERROR;

  root -> type = FOLDER; // root is always folder
  root -> name = name;
  root -> quantity = 0;
  root -> previous = NULL; 
  root -> parent = NULL; 
  root -> next = NULL; 
  root -> child = NULL;
  strcpy(name, "/"); // root name is always '/'

  return root;
}

void menu() { // print menu
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

int renamedir(TREE * curr, char * name, char * newname) { //rename a directory
  TREE * node = getnode(curr, name);
  int len = strlen(name);
  if (!len) return ERROR;

  if (!node) {
    puts("something went wrong");
    return ERROR;
  }

  if (node -> type == FOLDER) strcpy(node -> name, newname);
  else puts("it's not a folder");
}

int renamefile(TREE * curr, char * name, char * newname) { //rename a file
  TREE * node = getnode(curr, name);
  int len = strlen(name);
  if (!len) return ERROR; // if name is empty

  if (!node) { // if node doesn't exist
    puts("something went wrong");
    return ERROR;
  }

  if (node -> type == FILE_) strcpy(node -> name, newname);
  else puts("it's not a file");
}

void green() { //green color
  printf("\033[0;32m");
}

void blue() { //blue color
  printf("\033[0;34m");
}

void reset() { //reset color
  printf("\033[0m");
}

int verifyString(TREE * currfolder, TREE * root, char * path) { 
  char a[100]; 
  char * newFolder = (char * ) malloc(sizeof(a)); 
  char * newname = (char * ) malloc(sizeof(a));
  char * newnameAUX = (char * ) malloc(sizeof(a));
  char * command = (char * ) malloc(sizeof(a));
  int index;
  int auxname;
  bool aux;

  if (!path || !newFolder || !newFolder || !command) 
    return ERROR; 

  while (true) {
    green();
    printf("joao@joao:~");
    blue();
    pwd(path); // print current directory
    green();
    printf("$ ");
    reset();

    command = getString();
    index = strlen(command);

    aux = false;
    auxname = 0;

    for (int i = 0; i < index; ++i)
      if (command[i] == ' ') {
        aux = true;
        auxname++;
      }

    strcpy(newnameAUX, command);

    if (auxname >= 2) { 
      newFolder = strtok(newnameAUX, " ");
      newFolder = strtok(NULL, " ");
      newname = strtok(command, " ");
      newname = strtok(NULL, " ");
      newname = strtok(NULL, " "); //newname is the name of the file or folder
      newname = removeLineBreak(newname);
    }

    if (aux && auxname < 2) {
      newFolder = strtok(command, " ");
      newFolder = strtok(NULL, " "); 
      newFolder = removeLineBreak(newFolder);
    }

    command = splitString(command); //remove spaces
    command = removeLineBreak(command); 

    if (!strcmp(command, "mkdir"))
      mkdir(currfolder, newFolder);

    else if (!strcmp(command, "touch"))
      touch(currfolder, newFolder);

    else if (!strcmp(command, "ls"))
      ls(currfolder);

    else if (!strcmp(command, "pwd")) {
      pwd(path);
      printf("\n");
    } else if (!strcmp(command, "cd.."))
      currfolder = cdprevious(currfolder, & path);

    else if (!strcmp(command, "renamedir"))
      renamedir(currfolder, newFolder, newname);

    else if (!strcmp(command, "renamefile"))
      renamefile(currfolder, newFolder, newname);

    else if (!strcmp(command, "cd"))
      currfolder = cd(currfolder, newFolder, & path);

    else if (!strcmp(command, "rmdir"))
      rmdir(currfolder, newFolder);

    else if (!strcmp(command, "rm"))
      rm(currfolder, newFolder);

    else if (!strcmp(command, "clear"))
      clearScreen();

    else if (!strcmp(command, "help"))
      menu();

    else if (!strcmp(command, "exit")) 
      exitProgram(command, root, path);

  }
  free(command);
  free(newnameAUX);
  free(newname);
}

int main() { 
  TREE * root = rootCreate(); //create root
  char a[100];
  TREE * curr = root;
  char * path = (char * ) malloc(sizeof(a));
  strcpy(path, "/");
  mkdir(curr, "Arquivos-e-Programas"); //create folder 'Arquivos-e-Programas'
  curr = cd(curr, "Arquivos-e-Programas", & path); //change directory to 'Arquivos-e-Programas'
  touch(curr, "Firefox"); //create file 'Firefox'
  touch(curr, "Chrome"); //create file 'Chrome'
  touch(curr, "Opera"); //create file 'Opera'
  curr = cdprevious(curr, & path); //return to root
  mkdir(curr, "Meus-Documentos"); //create folder 'Meus-Documentos'
  curr = cd(curr, "Meus-Documentos", & path); //change directory to 'Meus-Documentos'
  touch(curr, "apresentacao.ppt"); //create file 'apresentacao.ppt'
  touch(curr, "relatorio.doc"); //create file 'relatorio.doc'
  mkdir(curr, "fontes"); //create folder 'fontes'
  curr = cd(curr, "fontes", & path); //change directory to 'fontes'
  touch(curr, "main.c"); //create file 'main.c'
  touch(curr, "main.h"); //create file 'main.h'
  curr = cdprevious(curr, & path); //return to root
  mkdir(curr, "imagens"); //create folder 'imagens'
  touch(curr, "7zip.exe"); //create file '7zip.exe'
  touch(curr, "t2.rar"); //create file 't2.rar'
  curr = cdprevious(curr, & path); //return to root
  verifyString(curr, root, path); //verify and execute commands
  return SUCCESS;
}