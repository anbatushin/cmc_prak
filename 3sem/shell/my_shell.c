#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

/*------------------------List of types------------------------*/

enum {
 NUM_CHAR = 10,
 SIZE = 256
};

typedef enum {nof, back, /* amper, */ ifp, ofp} flag_type;
typedef enum {neutral, begin, end} loc_flag_type;
typedef enum {no, yes} space_flag_type;

typedef struct cmd_info {
 char*** argv;
 char* infile;
 char* outfile;
 int backgrnd;
} cmd_info;

/*------------------------List of functions------------------------*/

void invitation(void);

void create_buf(void);

void create_block(void);

void prepare(void);

void termlist(void);

void add_word(void);

void add_buf(int symbol);

void add_ofp(void);

void add_ifp(void);

int new_str(void);

int check_word(void);

int check_special(char c);

int str_to_words(void);

void print_block(cmd_info* list);

void print_list(cmd_info* list);

void free_list(cmd_info* list);

void free_all(void);

void mv_func_do(char** argv);

void mv_func(char** argv);

void grep_func_do(char** argv);

void grep_func(char** argv);

void od_func_do(char** argv);

void od_func(char** argv);

void cmd_shell(char** argv);

void cmd_to_user_3(void);

/*------------------------List of parametres------------------------*/

cmd_info* list;

int c;
int sizebuf;
int curbuf;
int sizelist;
int curlist;
char str[SIZE];
int length;
char* buf;
flag_type flag;
loc_flag_type loc_flag;
space_flag_type space_flag;
int num_of_cmd = 0;
int back_num = 0;
int file_append = 0;
int input;
char debug;

/*------------------------Init buf, list------------------------*/

void invitation(void)
{
 printf("%s", "\x1b[33m");
 char s[100];
 gethostname(s, 100);
 printf("%s@%s", getenv("USER"), s);
 printf("%s", "\x1B[37m");
 getcwd(s, 100);
 printf(":%s$ ", s);
 fflush(stdout);
}

void create_buf(void)
{
 buf = NULL;
 sizebuf = 0;
 curbuf = 0;
 return;
}

void create_block(void)
{
 list = (cmd_info*)malloc(sizeof(cmd_info));
 sizelist = 0;
 curlist = 0;
 return;
}

void prepare(void/* int flag */)
{
// if (flag == 1) {
  for (int i = 0; i < SIZE; i++) {
   str[i] = 0;
  }
  length = 0;
// }
 create_buf();
 list->argv = NULL;
 list->infile = NULL;
 list->outfile = NULL;
 list->backgrnd = 0;
 num_of_cmd = 0;
 sizelist = 0;
 curlist = 0;
 file_append = 0;
}

/*------------------------List aligment------------------------*/

void termlist(void)
{
 if (list == NULL || list->argv == NULL || list->argv[num_of_cmd-1] == NULL) {
  return;
 }
 if (curlist > sizelist - 1) {
  list->argv[num_of_cmd-1] = realloc(list->argv[num_of_cmd-1], (sizelist + 1) * sizeof(list->argv[num_of_cmd-1]));
 }
 list->argv[num_of_cmd-1][curlist] = NULL; //
 list->argv[num_of_cmd-1] = realloc(list->argv[num_of_cmd-1], (sizelist = curlist + 1) * sizeof(list->argv[num_of_cmd-1]));
}

/*------------------------Adding to the list functions------------------------*/

void add_word(void)
{
 if (buf == NULL) {
  return;
 }
 if (curbuf > sizebuf - 1) {
  buf = realloc(buf, sizebuf += 1);
 }
 buf[curbuf++] = '\0';
 buf = realloc(buf, sizebuf = curbuf);

 if (num_of_cmd == 0) {
  list->argv = realloc(list->argv, (num_of_cmd += 1) * sizeof(list->argv));
 }

 if (curlist > sizelist - 1) {
  if (sizelist == 0) {
   list->argv[num_of_cmd-1] = (char**)malloc(sizeof(list->argv[num_of_cmd]) * NUM_CHAR);
   sizelist += NUM_CHAR;
  } else {
   list->argv[num_of_cmd-1] = realloc(list->argv[num_of_cmd-1], (sizelist += NUM_CHAR) * sizeof(list->argv[num_of_cmd-1]));
  }
 }

 list->argv[num_of_cmd-1][curlist++] = buf;
 create_buf();
}

void add_buf(int symbol)
{
 char* tmp;
 if (symbol == ' ' || symbol == '<' || symbol == '>') {
  return;
 }
 if (curbuf > sizebuf - 1) {
  tmp = realloc(buf, sizebuf += NUM_CHAR);
  if (tmp != NULL) {
   buf = tmp;
  } else {
   printf("\n!!!!!!!!!!!!!!!!!!!!error!!!!!!!!!!!!!!!!!!!\n");
  }
 }
 buf[curbuf++] = symbol;
}

void add_ofp(void)
{
 if (buf == NULL) {
  return;
 }
 if (curbuf > sizebuf - 1) {
  buf = realloc(buf, sizebuf += 1);
 }
 buf[curbuf++] = '\0';
 buf = realloc(buf, sizebuf = curbuf);

 if (list->outfile == NULL) {
  list->outfile = (char*)malloc(sizeof(list->outfile));
  list->outfile = buf;
  create_buf();
 }
}

void add_ifp(void)
{
 if (buf == NULL) {
  return;
 }
 if (curbuf > sizebuf - 1) {
  buf = realloc(buf, sizebuf += 1);
 }
 buf[curbuf++] = '\0';
 buf = realloc(buf, sizebuf = curbuf);

 if (list->infile == NULL) {
  list->infile = (char*)malloc(sizeof(list->infile));
  list->infile = buf;
  create_buf();
 }
}

/*------------------------Syntax check------------------------*/

int new_str(void)
{
 for(;;) {
  if (read(1, &c, 1) == 0) {
   str[length] = '\0';
   return EOF;
  }

  if (c == '\n') {
   str[length] = '\0';
   return '\n';
  }

  if (length == 0 && (c == ' ' || c == '\t' || c == '>' || c == '<' || c == '|' || c == '&')) {
   ;
  } else {
   str[length++] = c;
  }
 }
 return 0;
}

int check_word(void)
{
 if (space_flag == no) {
  switch(flag) {
   case ofp:
		add_ofp();
		flag = nof;
		break;
  case ifp:
		add_ifp();
		flag = nof;
		break;
  case back:
		add_word();
		flag = nof;
		break;
/*  case amper:
		printf("return 2\n");
		return 2;
*/
  case nof:
		add_word();
		break;
  }
  space_flag = yes;
 }
 return 0;
}

int check_special(char c)
{
 return (c == '>' || c == '<' || c == '&' || c == '(' || c == ';' ||
	 c == '|' || c == ' ' || c == '\t' || c == ')');
}

int str_to_words(void)
{
 int check;
 int i;
 flag = nof;
 space_flag = no;
 for (i = 0; i < length; i++) {
  if (check_special(str[i])) {
   check = check_word();
/*
   if (check == 2) {
    length -= i;
    length--;
    return 2;
   }
*/
  }

  switch (str[i]) {
   case '>':
		if (flag == ofp && file_append == 0 && list->outfile == NULL) {
		 file_append++;
		}
		if (flag == nof) {
		 flag = ofp;
		}
		break;
   case '<':
		flag = ifp;
		break;
   case '&':
		if (flag == back) {
		 flag = nof;
		} else {
		 flag = back;
		}

/*		if (flag == amper) {
		 flag = nof;
		} else {
		 if (flag == back) {
		  flag = amper;
		 } else {
		  if (flag == nof) {
		   flag = back;
		  }
		 }
		}
*/
		break;
/*   case ';'
		return 2;
*/
   case '|':
		if (list->argv[num_of_cmd - 1] == NULL) {
		 printf("Null argument(s) between pipes\n");
		 //termlist();
		 return -1;
		}
		termlist();
		list->argv = realloc(list->argv, (num_of_cmd += 1) * sizeof(list->argv));
		curlist = 0;
		sizelist = 0;
		break;
   case ' ':
		break;
   case '\t':
		break;
   case '(':
		break;

   case ')':
		break;
   default:
		space_flag = no;
		add_buf(str[i]);
  }
 }
 if (space_flag == yes && flag == back) {
  list->backgrnd = 1;
 }
  check = check_word();
  termlist();
  if (num_of_cmd != 0) {
   if (list->argv[num_of_cmd - 1] == NULL) {
    printf("Too much '|' in the cmd\n");
    return -1;
   }
  }
/*
  if (check == 2) {
   length -= i;
   length--;
  }
*/
 return check;
}

/*------------------------Print | Free------------------------*/

void print_block(cmd_info* list)
{
 int i = 0;
 if (list == NULL /*|| list->argv == NULL*/) {
  return;
 }
 printf("\nCMD_STRUCT:\n");
 if (list->argv == NULL) {
  printf("list->argv is empty\n");
  return;
 }
 for (int j = 0; j < num_of_cmd; j++){
  i = 0;
  while (list->argv[j][i] != NULL) {
   printf("l->a[%d][%d]: %s\n", j, i, list->argv[j][i]);
   i++;
  }
 }
 printf("list->infile: %s\n", list->infile);
 printf("list->outfile: %s\n", list->outfile);
 printf("list->backgrnd: %d\n", list->backgrnd);
 return;
}

void print_list(cmd_info* list)
{
 print_block(list);
 printf("CMDs: %d\n", num_of_cmd);
 printf("FILE_APPEND: %d\n", file_append);
}


void free_list(cmd_info* list)
{
 int i = 0;
 if (list->argv != NULL) {
  for (int j = 0; j < num_of_cmd; j++) {
   i = 0;
   if (list->argv[j] == NULL) {
    break;
   }
   while (list->argv[j][i] != NULL) {
    free(list->argv[j][i]);
    i++;
   }
   free(list->argv[j]);
  }
  free(list->argv);
 }

 if (list->infile != NULL) {
  free(list->infile);
 }

 if (list->outfile != NULL) {
  free(list->outfile);
 }
}

void free_all(void)
{
 free(list);
}

/*------------------------Mini-tasks------------------------*/
/*----------------------------MV----------------------------*/

void mv_func_do(char** argv)
{
 char* error1 = "The source file failed to open\n";
 char* error2 = "The destination file failed to open or create\n";
 char* mv_buf;
 struct stat size;
 int file = open(argv[1], O_RDONLY);

 if (file == -1) {
  write(STDOUT_FILENO, error1, strlen(error1));
  return;
 } else {
  fstat(file, &size);
  buf = (char*)malloc(size.st_size);
  read(file, buf, size.st_size);
  close(file);
 }

 if (file != -1) {
  file = open(argv[2], O_CREAT | O_WRONLY, 0666);
  if (file == -1) {
   if (chdir(argv[2])) {
    write(STDOUT_FILENO, error2, strlen(error2));
    return;
   } else {
    file = open(argv[1], O_CREAT | O_WRONLY, 0666);
    write(file, buf, size.st_size);
    close(file);
   }
  } else {
   write(file, buf, size.st_size);
   close(file);
  }
  free(buf);
 }
}

void mv_func(char** argv)
{
 char* error;
 mv_func_do(argv); // mv: old -> new
 if (unlink(argv[1])) { // delete old file
  write(STDOUT_FILENO, error, strlen(error));
  return;
 }
}

/*------------------------GREP------------------------*/

void grep_func_do(char** argv)
{
 char* error1 = "File not exist or failed to open\n";
 int fl_v = 0; // "-v" mode is OFF
 int fl_n = 0; // "-n" mode is OFF
 int fl_null = 0;
 int k = 3;
 while (k != 5 && argv[k] != NULL) {
  if (!strcmp(argv[k], "-v")) { // "-v" mode check
   fl_v = 1; // "-v" mode is ON
   k++;
  } else {
   if (!strcmp(argv[k], "-n")) { // "-n" mode check
    fl_n = 1; // "-n" mode is ON
    k++;
   }
  }
 }
 char* substring = argv[1];
 FILE* fd = fopen(argv[2], "r");

 if (fd == NULL) {
  write(STDOUT_FILENO, error1, strlen(error1));
  return;
 }

 int num_str = 1;
 size_t size = SIZE;
 ssize_t size_of_str = 1;
 char* str = (char*)malloc(sizeof(char) * SIZE);
 char* bufstr = (char*)malloc(sizeof(char) * strlen(substring));
 while (size_of_str > 0) {
  size_of_str = getline(&str, &size, fd);

  int offset = 0;
  int flag = 0;
  while (str[offset] != '\0') {
   if (str[offset] == substring[0]) {
    /*flag = 1;
    for (int length = 0; length < strlen(substring); length++) {
     if (str[offset + length] != substring[length]) {
      flag = 0; break;
     }
    }
    */
    for (int i = 0; i < strlen(substring); i++) {
     bufstr[i] = str[offset + i];
    }
    if (!strcmp(bufstr, substring)) {
     flag = 1;
     break;
    }
   }
   offset++;
  }
  if (flag == 1 && fl_v == 0) {
   if (fl_n == 1) {
    printf("%d: %s", num_str, str);
   } else {
    printf("%s", str);
   }
  } else {
   if (flag == 0 && fl_v == 1) {
    if (fl_n == 1) {
     printf("%d: %s", num_str, str);
    } else {
     printf("%s", str);
    }
   }
  }
  num_str++;
 }
 free(str);
 fclose(fd);
}

void grep_func(char** argv)
{
 char* error = "No file-argument\n";
 if (argv[1] != NULL) {
  if (argv[2] == NULL) {
   write(STDOUT_FILENO, error, strlen(error));
   return;
  } else {
   grep_func_do(argv);
  }
 }
}

/*------------------------OD------------------------*/

void od_func_do(char** argv)
{
 char* error1 = "File not exist or failed to open\n";
 int fl_b = 0;
 if (argv[2] != NULL) {
  if (!strcmp(argv[2], "-b")) {
   char str[10];
   fl_b = 1;
  }
 }
 char* str_n = "\\n";
 int symbol;
 FILE* fd = fopen(argv[1], "r");
 if (fd == NULL) {
  write(STDOUT_FILENO, error1, strlen(error1));
  return;
 }
 int count = 0;
 size_t size = 1;
 while ((symbol = fgetc(fd)) != EOF) {

  if (fl_b == 1) {
   str[count] = symbol;
  }

  if (count == 0) {
   printf("%06ld ", size);
   size += 10;
  }

  switch (symbol) {
   case '\n':
		printf("%6s ", "\\n");
		break;
   case '\t':
		printf("%6s ", "\\t");
		break;
   default:
		printf("%6c ", symbol);
  }
  count++;
  if (count == 10) {
   printf("\n");
   if (fl_b == 1) {
    printf("%6c ", ' ');
    for (int i = 0; i < count; i++) {
     printf("%6o ", str[i]);
    }
    printf("\n");
   }
   count = 0;
  }
 }
 fclose(fd);
}

void od_func(char** argv)
{
 char* error = "No file-argument\n";
 if (argv[1] != NULL) {
  od_func_do(argv);
 } else {
  write(STDOUT_FILENO, error, strlen(error));
  return;
 }
}

/*------------------------CONVEYOR AND CALLS------------------------*/

void cmd_shell(char** argv)
{
 if (argv != NULL) {
  if (!strcmp(argv[0], "mv")) {
   mv_func(argv);
  } else {
   if (!strcmp(argv[0], "grep")) {
    grep_func(argv);
   } else {
    if (!strcmp(argv[0], "od")) {
     od_func(argv);
    } else {
     execvp(argv[0], argv);
    }
   }
  }
 }
}

void cmd_to_user_3(void)
{
 if (list->backgrnd == 0) {		// not background mode
  int prevfd = -1;
  for (int i = 0; i < num_of_cmd; i++) {
   int fd[2];
   if (i < num_of_cmd - 1) {
    pipe(fd);
   }
   if (!fork()) {
    if (i > 0) {
     dup2(prevfd, 0);
     close(prevfd);
    }
    if (i < num_of_cmd - 1) {
     dup2(fd[1],1);
     close(fd[1]);
     close(fd[0]);
    }
    if (list->infile != NULL) {
     fd[0] = open(list->infile, O_RDONLY, 0666);
     dup2(fd[0],0);
     close(fd[0]);
    }
    if (list->outfile != NULL) {
     if (file_append != 0) {
      fd[1] = open(list->outfile, O_CREAT | O_APPEND | O_WRONLY, ALLPERMS);
     } else {
      fd[1] = creat(list->outfile, ALLPERMS);
     }
     dup2(fd[1],1);
     close(fd[1]);
    }
    cmd_shell(list->argv[i]);
    exit(1);
   }
   if (i < num_of_cmd - 1) {
    close(fd[1]);
   }
   if (i > 0) {
    close(prevfd);
   }
   prevfd = fd[0];
  }
  while (wait(NULL) > 0) {
   ;
  }
 } else {				// background mode
  back_num++;
  pid_t pid;
  pid = fork();
  if (pid == 0) { // son
   signal(SIGINT, SIG_IGN);
   pid = fork();
   if (pid == 0) {
    signal(SIGINT, SIG_IGN);
    int prevfd = -1;
    for (int i = 0; i < num_of_cmd; i++) {
     int fd[2];
     if (i < num_of_cmd - 1) {
      pipe(fd);
     }
     if (!fork()) {
      if (i > 0) {
       dup2(prevfd, 0);
       close(prevfd);
      }
      if (i < num_of_cmd - 1) {
       dup2(fd[1],1);
       close(fd[1]);
       close(fd[0]);
      }

      if (list->infile != NULL) {
       fd[0] = open(list->infile, O_RDONLY, 0666);
      } else {
       fd[0] = open("/dev/null", O_RDONLY, 0666);
      }
       dup2(fd[0],0);
       close(fd[0]);

      if (list->outfile != NULL) {
       if (file_append != 0) {
        fd[1] = open(list->outfile, O_CREAT | O_APPEND | O_WRONLY, ALLPERMS);
       } else {
        fd[1] = creat(list->outfile, ALLPERMS);
       }
      } else {
       fd[1] = open("/dev/null", O_WRONLY | O_CREAT, 0666);
      }
      dup2(fd[1],1);
      close(fd[1]);

      cmd_shell(list->argv[i]);
//    execvp(list->argv[i][0], list->argv[i]);
      exit(1);
     }
     if (i < num_of_cmd - 1) {
      close(fd[1]);
     }
     if (i > 0) {
      close(prevfd);
     }
     prevfd = fd[0];
    }
    while (wait(NULL) > 0) {
     ;
    }
   }
   exit(0);
  }
  printf("[%d] background process was started by a process with PID = %d \n", back_num, pid);
  wait(NULL);
 }
}

/*------------------------SHELL------------------------*/

int main(void)
{
 signal(SIGINT, SIG_IGN);
 int eof_fl = 0;
 int str_fl = 0;
 int str_er = 0;
 create_block();
 for(;;) {
/*
   if (str_fl == 2 && str_er == 0) {
   prepare(0);
   str_fl = 0;
   printf("there\n");
  } else {
   prepare(1);
   str_fl = 0;
   str_er = 0;
*/
  invitation();
  //printf("$ ");
  //fflush(stdout);
  prepare();
  str_fl = 0;
  if (new_str() == EOF) {
   eof_fl++;
  }

  if (eof_fl == 1) {
   break;
  }
// }

  str_fl = str_to_words();

  if (str_fl == -1) {
   free_list(list);
   printf("Please, write the next command:\n");
   continue;
  }

  cmd_to_user_3();
  print_list(list);
  free_list(list);
 }

 free_all();
 printf("\n");
 return 0;
}
