// Israel Cohen
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
struct historyElemnt
{
char command[200];
char command_no_ref[200];
char flag; 
unsigned int pid;
};

//pid_t child_pids[200];

struct stacklike
{
char last_cwd[500];
};

struct historyElemnt history[200];
struct stacklike pathes[500];
int counter_hist = 0;
int spec_path = 0;
int k=0;

//general commamds: 
void do_what_was_asked(char* do_that[], bool background){
  int stat, waited, ret_code;
  pid_t pid;
  pid = fork();
  history[counter_hist-1].pid=pid;
  if (pid==0)
  {/*child*/
   /* printf("child gonna do his job!\n");
    if (background == true) {
      printf("child background job!\n");
      waitpid(pid,NULL,WNOHANG); }
    else {
      printf("child regular job!\n");
     waitpid(-1,NULL,WNOHANG); }//pid_t check = waitpid(-1,NULL,WNOHANG); //why needed again? XD
    */
    ret_code=execvp(do_that[0],&do_that[0]);
    //printf("child done his job!\n");
    if (ret_code==-1) {
      printf("exec failed\n");
      exit(-1); }
   /* else {
    printf("exec is good\n"); }*/
  }
  else {
    /*Parent*/
    if (!background)
    {
      waited=wait(&stat);
    }    
    //printf("Father: son proc completed, id is %d\n",waited);
}
  return;
}
//history commamds: 

//pid running or done state check
bool pid_state(int pid) {
    int waited, stat;
    waited = waitpid(pid, &stat, WNOHANG);
    if (waited == 0)
        return true;
    return false;
}


void history_func(){
  int i = 0;
  for (i ; i < 200; i++)
  {
    char curr_flag[200];
    if (history[i].command[0]!='\0') {
      bool state=false;
      if(history[i].pid!=0) {
        state = pid_state(history[i].pid);
        if (state == false)
        history[i].flag = 'd'; 
        else
          history[i].flag='r';
      }
      if (history[i].flag=='r')
        {
          strcpy(curr_flag,"RUNNING");
        }
      else
        {
          strcpy(curr_flag,"DONE");
        }
        /*if (history[i].command == "echo"){
          printf("%s %s\n",history[i].command_no_ref,curr_flag);    
        }
      else*/ printf("%s %s\n",history[i].command,curr_flag);
    }
    else
      return;  
  }
  return;
}
//jobs commamds: 
void jobs_func(){
  bool state = false;
  int i =0;
  for (i; i < 200; i++)
  {
    if (history[i].command[0]!='\0' && history[i].command[0]!='j'&& history[i].command[1]!='o'&& history[i].command[2]!='b') {
      state = pid_state(history[i].pid);
      if (history[i].flag=='r' && state)
      {
        printf("%s\n",history[i].command_no_ref);
      }
    }
    else
      if (history[i].command[0]=='\0')
      return;
      continue;
  }
  return;
}

void last_path(){
    memset(pathes[spec_path].last_cwd, 0, sizeof(pathes[spec_path].last_cwd)); 
    getcwd(pathes[spec_path].last_cwd, sizeof(pathes[spec_path].last_cwd));
    //printf("%s\n",pathes[spec_path].last_cwd);
    return;
}

//cd commamds: 
void cd_func(char* do_that[]){
  // If we write no path (only 'cd'), then go to the home directory
    if (do_that[1] == NULL) {
      spec_path+=1;
      last_path();
      chdir(getenv("HOME")); 
      return;
    }
    // Else we change the directory to the one specified by the 
    // argument, if possible
    else{ 
        if (*do_that[1]== '-'){
         spec_path-=1;
         chdir(pathes[spec_path].last_cwd);
         return;
        }
        else if (*do_that[1] == '~'){ //first letter is ~ than:
        int init_size = strlen(do_that[1]);
        char delim[] = "/";
        char actual_str[250];
        char *ptr;
          char * aftertilda = ++do_that[1];
          ptr = strtok(do_that[1], delim);
          int counter_letters = 0;
          while(ptr != NULL)
          {
            //printf("'%s'\n", ptr);
            if (counter_letters>=1)
            {
              int i=0;
              char *out;
              while (*ptr!='\0')
              {
                strcat(&actual_str[i],ptr);
                ptr++;
                i++;
              }
              memset(actual_str,0,250);
            }
            ptr = strtok(NULL, delim);
            counter_letters+=1;
          }
          if (counter_letters==0)
          {
            last_path(); 
            chdir(getenv("HOME"));
            return;
          }
          else{
            spec_path+=1;
            last_path();
            chdir(getenv("HOME"));
            char cdir[200]={};
            strcpy(cdir,aftertilda);
            chdir(aftertilda);            
            return;
          }
         }
        else if (chdir(do_that[1]) == -1) {
          printf("chdir failed\n");
          //printf("An error occurred\n");
         }
        else{
          spec_path+=1;
          last_path();  
          (do_that[1]);
            return;
         }
        }
  return;
}

//read from user:
void strip(char *s) {
    char *p2 = s;
    while(*s != '\0') {
        if(*s != '\t' && *s != '\n') {
            *p2++ = *s++;
        } else {
            ++s;
        }
    }
    *p2 = '\0';
}
void parse (char *buf, char **args)  
{
  while (*buf != (long) NULL)
    {
      while ((*buf == ' ') || (*buf == '\t')) *buf++ = (long) NULL;
      // Save the argument.
      *args++ = buf;
      // Skip over the argument.
      while ((*buf != (long) NULL) && (*buf != ' ') && (*buf != '\t')) buf++;
    }
  *args = NULL;
}

int main(void) {
  bool background = false; 
  char str[1024];
  //int waited0, stat0; 
  char *args[64]={};
  last_path();
  
      while(k==0) {
      background = false;
      printf("$ "); 
      fflush(stdout);
      fgets(str, 200, stdin);
      strip(str);
      parse (str, args);
      char text[200];
      sprintf(text,"/bin/%s",args[0]);
      fflush(stdout);//~~~~~~check at end that its ok~~~~~
      int i=0;
      char *check[200]={};
      int num_words=0;
      while (args[i]!=NULL)
      {
        if (*args[i]=='&')
        {
          background=true;
          i++;
          continue; 
        }
        check[i]=args[i];
        num_words++;
        i++;
      }
      char temp[200];
      int indd=0;
      for (indd; indd < num_words; indd++)
      {
        strcat(temp, args[indd]);
        if(indd+1 != num_words)
          strcat(temp, " ");
      }
      strcpy(history[counter_hist].command_no_ref, temp);
      if (background==true)
      {
        strcat(temp, " ");
        strcat(temp, "&");
      }
      
      strcpy(history[counter_hist].command, temp);
      history[counter_hist].flag = 'r';
      counter_hist++;
      memset(temp, 0, 200);
      if (!strcmp(args[0],"exit")) //if its compare = return 0, which is false, so that why '!' before
      {
        k=1;
        /*bool state = false;
      for (int i = 0; i < 200; i++)
      {
        if (history[i].command[0]!='\0' && history[i].pid!='0') {
          state = pid_state(history[i].pid);
          if (history[i].flag=='r' && state)
          {
           kill (history[i].pid, SIGKILL);
          }
        }
      }
      */
        //~~~~~~~~~~~~~~~~~~~~Todo delete all procceses (meybe through jobs) than exit.~~~~~~~~~~~~~~~~~~~~
        //waited0=wait(&stat0);
       exit(0);
       return 0;
      }
      else if (!strcmp(check[0],"jobs")) 
      {
        jobs_func();
        history[counter_hist-1].flag='d';
        history[counter_hist-1].pid=0;
        continue;
      }
      else if (!strcmp(check[0],"history")) 
      {
        history_func();
        history[counter_hist-1].flag='d';
        history[counter_hist-1].pid=0;
        continue;
      }//add_to_history
      else if (!strcmp(check[0],"cd")) 
      {
        if (num_words>2) { 
          printf("Too many argument\n");
          history[counter_hist-1].flag='d';
          history[counter_hist-1].pid=0;
          continue;
        }
        else {
          cd_func(check);
          history[counter_hist-1].flag='d';
          history[counter_hist-1].pid=0;
          continue;
        }
      }
      else if(!strcmp(check[0],"echo")){
        
        char str_holder[100] = "";
        int indx=0;
        char *r , *w;
        for (w = r = check[1]; *r; r++) {
            if (*r != '"') {
                str_holder[indx]=*r;
                indx++;
                *w++ = *r;
            }
        }
        *w = '\0';
        strcpy(check[1], str_holder);


        do_what_was_asked(check, background);
        history[counter_hist-1].flag='d';
        history[counter_hist-1].pid=0;
        continue;
      }
      else 
      do_what_was_asked(check, background);
      //history[counter_hist-1].flag='d';
      }
}
