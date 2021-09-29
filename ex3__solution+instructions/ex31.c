// Israel Cohen
#include <ctype.h>
#include <sys/fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define SIZE 10

//func check similarity (state 2)
int similar_check (char *argv[]) {

//int fdin1 = open("./check2.txt", O_RDONLY);
//int fdin2 = open("./check1.txt", O_RDONLY);
int fdin1 = open(argv[1], O_RDONLY);
int fdin2 = open(argv[2], O_RDONLY);

//how many letter was read
int charsr1=0; 
int charsr2=0;

char letters_holder1[300];
char letters_holder2[300];
int index1=0, index2=0;

char temp1[1];    // input (output) buffer 
char temp2[1];    // input (output) buffer 

char a;
char b;

do{
	charsr1 = read(fdin1,temp1,1);
	a=*temp1;
	if(a==*" " ||a==*"\n")
	continue;
	else{
		if (charsr1!=0)
		{
			letters_holder1[index1]=tolower(temp1[0]);
			index1++;
		}
	}
} while(charsr1!=0);

do{
	charsr2 = read(fdin2,temp2,1);
	b=*temp2;
	if(b==*" " ||b==*"\n")
	continue;
	else{
		if (charsr2!=0)
		{
			letters_holder2[index2]=tolower(temp2[0]);
			index2++;
		}
	}
} while(charsr2!=0);

int check = strcmp(letters_holder1,letters_holder2);
close(fdin1);
close(fdin2);
if (check == 0)
{
	return 3;
}
else
return 2;
}

int main(int argc , char *argv[]) 
{
	int fdin1;            // input file descriptor  
	int fdin2;            // out file descriptor  
	char buf1[1];    	  // input (output) buffer 
	char buf2[1]; 		  // input (output) buffer 
	int charsr1;          // how many chars were actually red  
	int charsr2;          // how many chars were actually red 
	int state = 1;
	char a,b;

	//fdin1 = open("./check1.txt", O_RDONLY);
	//fdin2 = open("./check2.txt", O_RDONLY);
	
	fdin1 = open(argv[1],O_RDONLY);
	if (fdin1 < 0) { //Means file open did not take place 
		exit(-1); 
	}
	fdin2 = open(argv[2], O_RDONLY);
	if (fdin2 < 0) {
		exit(-1); 
	}
	int faster_way_out=0;
	do{
	charsr1 = read(fdin1,buf1,1);
	charsr2 = read(fdin2,buf2,1);
	a=*buf1;
	b=*buf2;
	if (charsr2!=0)
	{
		int check=(a==b);
		if ((check!=1)||(charsr2!=charsr1))
		{
			faster_way_out = 1;
			state = -1;
		}
	}
	else if (charsr1!=0)
	{
		faster_way_out = 1;
		state = -1;
	}
  else 
	{
		faster_way_out = 1;
	}
	} while(charsr2!=0,faster_way_out==0);

	close(fdin1);		// free allocated structures 
	close(fdin2);		// free allocated structures 

	if (state!=1) 
	
	state = similar_check(argv);
	if (state != 3 && state !=1)
	state = 2;
	return state;
}


//for my own use, not directly relevant to exercise
/* //see cwd to debbug/terminal usage...
#include <stdio.h>
#include <limits.h>
int main() {
   char cwd[150];
   if (getcwd(cwd, sizeof(cwd)) != NULL) {
       printf("Current working dir: %s\n", cwd);
   } else {
       perror("getcwd() error");
       return 1;
   }
   return 0;
}
*/
