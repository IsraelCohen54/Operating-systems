// Israel Cohen
#include <string.h>
#include <sys/wait.h>
#include <time.h> //hope its ok to check time running...
#include <ctype.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <sys/fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#define SIZE 150

int main(int argc , char *argv[]) 
{   
    int fdin1;            // conf file descriptor
    int fdin2;            // input file descriptor
    int fdin3;            // correct_ouput file descriptor

    //~~~ open error file, redirect errors to this file
    int error_file;
    error_file = open("errors.txt", O_WRONLY | O_APPEND |O_TRUNC| O_CREAT, 0666);
	if (error_file < 0) //means file open did not take place
	{      
        perror("");
		exit(-1);
	}
    int sys_error = 0;
    sys_error = dup(2); //erorr value to change it back at prog end, just to be on the safe side
    dup2(error_file,2); //error goes to error file
    //~~~

    //~~~ get "root" = students folder, directory path:
    fdin1 = open(argv[1],O_RDONLY,0777); //meaning read only, with all restriction needed...
	if (fdin1 < 0) // means file open did not take place 
	{                
        //perror("");
		exit(-1); 
	}

    DIR* dip, *dip2; //dir pointer
	struct dirent* dit, *dit2; //dir data holder, probably :)
	int i = 0;

    int EFO_CHECK=0;
    int ind=0;
    int dir_path=0, input=0;
    char path_data_dir[SIZE];    // store sub dir& dir name to get full path
    char path_input_data[SIZE];  // store input path
    char path_output_data[SIZE]; // store output path
    
    do //read conf.txt:
	{
        char path_data[SIZE]; // input (output) buffer of conf.txt
        memset(path_data,0,150); //deleate path_data after copying the path
        ind=0; //index to fill path data char char

        //as we where told the path can be like file.txt or ./file.txt, not using the 4 next lines:
        //path_data[ind]='.'; 
        //ind++;
        //path_data[ind]='/';
        //ind++;

        do {    
            if (strcmp(&path_data[ind-1],"\n")) //while its not end of line
            {
                //1 mean rading char char, from fdin1 into path_data index:
                read(fdin1, &path_data[ind], 1); 
            }
        ind++;
        } while (strcmp(&path_data[ind-1],"\n"));
        path_data[ind-1]=*"\000"; //give EFO (end of file) symbol to path_data
        
        //For first line (*only*, that's why 'if' at start) in path_data do:
        //open studens dir:
        if (dir_path==0) 
        {
            if((dip = opendir(path_data)) == NULL)
	        {
		    printf("Not a valid directory\n");
		    exit (-1);
            }
            dir_path++;
            strcpy(path_data_dir,path_data); //save path to later use
        } //sec line:
        else if (input==0) //sec line: get text input pointer
        {
            strcpy(path_input_data,path_data);
            input++;
        } 
        else //third line: get correct_output file pointer:
        {
            strcpy(path_output_data,path_data);
            fdin3 = open(path_data,O_RDONLY|O_CREAT,0666); 
            if (fdin3 < 0) // means file open did not take place 
	        {                
                printf("Output file not exist\n");
		        exit(-1); 
	        }
        }
        EFO_CHECK++; //after 3 lines, finish with conf.txt
	} while (EFO_CHECK != 3);

    char path_data_dir_first[SIZE]; // store dir name
    strcpy(path_data_dir_first,path_data_dir); //hold students as dir name to path usage

    //write result of comparison between 'compare_temp' to 'correct_output' to csv file:
    //creating the csv:
    int fpt_csv;
    fpt_csv = open("results.csv", O_RDWR | O_APPEND |O_TRUNC| O_CREAT, 0666);
    if (fpt_csv < 0)	// means file open did not take place
	{ 
		perror("");
		exit(-1); 
	}
    int sys_input=0, sys_output=0; //save dup original values...
    sys_input=dup(0);              //...input value (like from terminal & keyboard etc)
    sys_output=dup(1);             //...output value    

    while((dit = readdir(dip)) != NULL) //for every folder in students:
	{
        int fpt_temp; //ptr to write file_name.C results with the input file to TEMP file:
        //O_RDWR =Read & write type, O_APPEND = every line goes to end of file
        //O_TRUNC = start from "zero" every run, O_CREAT = if its not exist, create one.
        //The file hold output from c files + input.txt and compilatiaon & run, after I compare it to
        //correct output
        fpt_temp = open("compare_temp.txt", O_RDWR | O_APPEND|O_TRUNC| O_CREAT, 0777);
        if (fpt_temp < 0)	// means file open did not take place
        { 
        //    perror("");
            exit(-1); 
        }
        if(strcmp((dit->d_name),"..")==0||strcmp((dit->d_name),".")==0) //jump over linux folders
        continue;

        memset(path_data_dir,0,SIZE);
        strcpy(path_data_dir,path_data_dir_first);
        strcat(path_data_dir, "/");
        strcat(path_data_dir, dit->d_name); //dit->d_name = sec gen folders name
        //after we have full path to files in dir within dir:
        if((dip2 = opendir(path_data_dir)) == NULL) //here look for .c file type
        {
           // perror("");
            continue;
            //return 0;
        }
        int check_if_c_file_exist = 0; //should be only one!
        char one_line[SIZE]; //used ahead in writing to csv file.
        while((dit2 = readdir(dip2)) != NULL) //for every sub-folder in students
	    {
            if(strcmp((dit2->d_name),"..")==0||strcmp((dit2->d_name),".")==0) //jump over linux folders
            continue;
            
            //Logic: check if 'c' at end of file name, than use full name as path.
            //first thing - file name is: dit2->d_name
            char type_file[SIZE];
            //here goes magic:
            char * token, * last; 
            strcpy(type_file,dit2->d_name);
            last = token = strtok(dit2->d_name, "."); //delimiter = "." with file name given
            while(token != NULL) { //get last writed letters after . from token
                last = token;
                token = strtok(NULL, ".");
            }
            if (!strcmp(last,"c")) //if file end with .c: 
            {
            check_if_c_file_exist = 1;
            //check .c file with its path and input after excecuting, save output to csv file:
            int c_prog_ptr; //c_prog_ptr = pointer to file_name.c
            char full_path_holder[SIZE];
            memset(full_path_holder,0,SIZE); //clear
            strcat(full_path_holder,path_data_dir_first);
            strcat(full_path_holder,"/");
            strcat(full_path_holder,dit->d_name);
            strcat(full_path_holder,"/");
            strcat(full_path_holder,type_file);
            
            if ((c_prog_ptr = open(full_path_holder, O_CREAT|O_RDONLY, 0644)) < 0) 
            { 
             //   perror(""); // open failed
                continue;
            }
            char * c_path[]={"gcc","-o","b.out", full_path_holder,NULL}; //compilation data
            //Two forks, one to compile, second to execute/run it.
            pid_t child1, child2;
            int child_status1,child_status2;
            child1 = fork();
            if(child1 == 0) {
                // This is done by the child process.
                execvp("gcc",c_path); //gcc goes twice...
                //If and only if ;) execvp failed: (else its return 0 if I am not mistaken)
                return(-1);
            }
            else{
                //In father process:
                pid_t tpid_1 = wait(&child_status1); //wait son to finish running
            }
            //Somehow child_status1 give as wrong return value 256 instead of 1
            if(child_status1/256==1){ //compilation error! => write result to csv file
                memset(one_line,0,SIZE);
                strcat(one_line,dit->d_name);
                strcat(one_line,",");
                strcat(one_line,"10");
                strcat(one_line,",");
                strcat(one_line,"COMPILATION_ERROR\n");
                write(fpt_csv,one_line,SIZE); 
                close(fpt_temp);
                continue;
            }
            //check running time, if it's greater than 5 sec, not good!
            //Store the execution time of the code:
            time_t begin = time(NULL); //its begin here, as its unknown who'l start first, child or father

            child2 = fork();
            if(child2 == 0) { // This is done by the child process.
                //Open input file:
                fdin2 = open(path_input_data,O_RDONLY);
                if (fdin2 < 0) // Means file open did not take place 
                {                
               //     printf("Input file not exist\n"); // open failed
                    exit(-1); 
                }
                //writing data of input file as input, like its currently input from a keyboard/terminal
                dup2(fdin2,0);
                //output to temp file to compare with ex1 if its alike/identical etc with correct_output
                dup2(fpt_temp,1);
                //run:
                execl("./b.out","./b.out",NULL);
                //if execvp failed:
                return(-1);
            }
            else{
                pid_t tpid_2 = wait(&child_status2); //wait for son to finish
                time_t end = time(NULL);           
                // calculate elapsed time by finding difference (end - begin)
                // printf("The elapsed time is %d seconds", (int)(end - begin));
                int time_ok = 5;
                if (time_ok<=(int)(end - begin))
                {   //took longer than 5 sec:
                    memset(one_line,0,SIZE);
                    strcat(one_line,dit->d_name);
                    strcat(one_line,",");
                    strcat(one_line,"20");
                    strcat(one_line,",");
                    strcat(one_line,"TIMEOUT\n");
                    write(fpt_csv,one_line,SIZE); 
                    close(fpt_temp);
                    continue;
                }
            }
            memset(full_path_holder,0,SIZE);
            close(c_prog_ptr);

            //compare output of c file+its input, with correct output file:
            pid_t child_compare;
            int child_compare_status, child_pid=-1;

            dup2(sys_input,0);  //Regular input again. Just in case :)
            dup2(sys_output,1); //Regular output again. Just in case :)

            //Here we assume comp.out currenctly does exist (got from compilation of ex31)
            char * ex31_files_check2[]={"./comp.out","./compare_temp.txt", path_output_data,NULL};
            
            //Here goes checking output equal to correct_output file:
            child_compare = fork();
            if(child_compare == 0) {
                // This is done by the child process.
                execve ("./comp.out",ex31_files_check2,NULL);
                //if execvp failed:
                return(-1);
            }
            else{
                pid_t tpid_compare = wait(&child_compare_status);
                //printf("return value: %d",child_compare_status/256); //to see return value from ex31
            }
            int grade = child_compare_status/256; //res from ex31
            memset(one_line,0,SIZE);
            strcat(one_line,dit->d_name);
            strcat(one_line,",");
            if (grade == 1) {
            strcat(one_line,"100");
            strcat(one_line,",");
            strcat(one_line,"EXCELLENT\n");
            }
            if (grade == 3) {
            strcat(one_line,"75");
            strcat(one_line,",");
            strcat(one_line,"SIMILAR\n");
            }
            if (grade == 2) {
            strcat(one_line,"50");
            strcat(one_line,",");
            strcat(one_line,"WRONG\n");
            }
            close(fpt_temp);
            write(fpt_csv,one_line,SIZE); 
            }
        }
        //If c file wasnt found in dir (as it should):
        if (check_if_c_file_exist==0)
        {
            memset(one_line,0,SIZE);
            strcat(one_line,dit->d_name);
            strcat(one_line,",");
            strcat(one_line,"0");
            strcat(one_line,",");
            strcat(one_line,"NO_C_FILE\n");
            write(fpt_csv,one_line,SIZE); 
            close(fpt_temp);
        }
	}
    //Close all allocated memory:
    if(closedir(dip) == -1)
	{
		//perror("");
		//return 0;
	}
    if(closedir(dip2) == -1)
	{
		//perror("");
		//return 0;
	}
    //Making sure that stdin/stdout/stderrors are as it was...
    dup2(sys_input,0);
    dup2(sys_output,1);
    dup2(sys_error,2);

    close(fdin1); // free allocated structures 
    close(fdin2); // free allocated structures 
    close(fdin3); // free allocated structures 
    //deleate temp files:
    int del = remove("compare_temp.txt");
    //if (del)
      //perror("");
    int del2 = remove("b.out");
    //if (del2)
      //perror("");
    close(fpt_csv);
    close(error_file);
}
