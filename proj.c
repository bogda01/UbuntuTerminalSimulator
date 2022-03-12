#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <fcntl.h>

//when you run you need to type: gcc proj.c -o proj -lreadline

#define MAXLETTERS 1000000 
#define MAXCOMMANDS 10000 

#define clear() printf("\033[H\033[J")

int pipes,redirects;
char* cmds[6]={"help","wc","expand","env","exit","hello"};
int outputFile;
  
//read input si history
int readInput(char* input)
{
    char* text;
    text = readline("$> ");
    if (strlen(text) != 0) {
        add_history(text);
        strcpy(input, text);
        return 0;
    } else {
        return 1;
    }
}
  
//directory 
void printDir()
{
    char currentDirectory[MAXLETTERS];
    getcwd(currentDirectory, sizeof(currentDirectory));
    printf("\n~%s\n", currentDirectory);
}

//Ctrl+C nu inchide programul
void ctrlC(int signal)
{
    printf("\nCtrl+C doesn't work, type exit if you want to close the program\n");
    printDir();
}

//vedem cate | avem in comanda
int countPipe(char* str)
{
    int counter=0,length;
    length=strlen(str);
    for(int i=0;i<length;i++)
        if(str[i]=='|')
            counter++;
    return counter;
}

//vedem daca avem redirect in comanda
int countRedirect(char *str)
{
    int counter=0,length;
    length=strlen(str);
    for(int i=0;i<length;i++)
        if(str[i]=='>')
            counter++;
    return counter;
}

//functia help
void helpExec()
{
    puts(
        "\nList of Commands supported:"
        "\n>help"
        "\n>wc -c, -w, -l, -L"
        "\n>expand -t, -i"
        "\n>env -u"
        "\n>exit");
}

void testare()
{
    puts("Hello");
}

void wcExecPipe(int byt,int l,int L,int w)
{
    char buffer[MAXLETTERS];
    int bytes=0, words=0, newlines=0,ok=0,count=0,longest=0;
    FILE *f;
    char c[1],ch;
    f=fopen("stdout.txt","r");
    if(f==0){
        printf("\nCouldn't open file\n");
        exit(0);
    }
    else{
        //ok=0 when it meets whitespace,newline or tab, ok=1 when it meets a word 
        while(read(fileno(f),c,1)==1)
        {
            count++;
            if(c[0]==' ' || c[0]=='\t')
                ok=0;
            else if(c[0]=='\n')
            {
                ok=0;
                newlines++;
                if(count>longest)
                    longest=count;
                count=0;
            }
            else{
                if(ok==0)
                    words++;
                ok=1;
            }

            bytes++;
            
        }
        if(byt==1)
            printf("\n%d\n",bytes);
        else if(l==1)
            printf("\n%d\n",newlines);
        else if(L==1)
            printf("\n%d\n",longest);
        else if(w==1)
            printf("\n%d\n",words);
        else {
            //printf("\nSUNT In ultimul if\n");
            printf("\n%d %d %d\n",newlines,words,bytes);
        }
    }

}

void wcPipe(char **parsed)
{
//-c, -l, -L, -w
    //printf("\nSUNT FUNCTIA WCPIPE\n");
    int c=0,l=0,L=0,w=0;

    if(parsed[1]!=NULL)
    {
        if(strcmp(parsed[1],"-c")==0){
            c=1;
        }
        else if(strcmp(parsed[1],"-l")==0){
            l=1;

        }
        else if(strcmp(parsed[1],"-L")==0){
            L=1;
        }
        else if(strcmp(parsed[1],"-w")==0){
            w=1;
        }
    }   
    wcExecPipe(c,l,L,w);
}

void wcExec(char **parsed,int byt,int l,int L,int w)
{
    char *filename,buffer[MAXLETTERS];
    filename=parsed[1];
    int bytes=0, words=0, newlines=0,ok=0,count=0,longest=0;
    FILE *f;
    char c[1],ch;
    if(byt==0 && l==0 && L==0 && w==0)
        f=fopen(parsed[1],"r");
    else 
        f=fopen(parsed[2],"r");
    if(f==0){
        printf("\nCouldn't open file\n");
        exit(0);
    }
    else{
        //ok=0 when it meets whitespace,newline or tab, ok=1 when it meets a word 
        while(read(fileno(f),c,1)==1)
        {
            count++;
            if(c[0]==' ' || c[0]=='\t')
                ok=0;
            else if(c[0]=='\n')
            {
                ok=0;
                newlines++;
                if(count>longest)
                    longest=count;
                count=0;
            }
            else{
                if(ok==0)
                    words++;
                ok=1;
            }

            bytes++;
            
        }
        
        fclose(f);
        if(byt==1)
            printf("\n%d %s\n",bytes,parsed[2]);
        else if(l==1)
            printf("\n%d %s\n",newlines,parsed[2]);
        else if(L==1)
            printf("\n%d %s\n",longest,parsed[2]);
        else if(w==1)
            printf("\n%d %s\n",words,parsed[2]);
        else 
            printf("\n%d %d %d %s\n",newlines,words,bytes,parsed[1]);
    }

}

void wc(char** parsed)
{
//-c, -l, -L, -w
    //-c=nr caractere, -l=nr linii, -L=lungimea celei mai lungi linii, -w=nr de cuvinte
    int c=0,l=0,L=0,w=0;

    if(strcmp(parsed[1],"-c")==0){
        c=1;
    }
    else if(strcmp(parsed[1],"-l")==0){
        l=1;

    }
    else if(strcmp(parsed[1],"-L")==0){
        L=1;
    }
    else if(strcmp(parsed[1],"-w")==0){
        w=1;
    }
    wcExec(parsed,c,l,L,w);
}

//-a nu exista
//un tab= 8 spatii, cred
void expandExec(char **parsed)
{
    char numb[1];
    numb[0]=parsed[1][2];
    int t=0,i=0,a=0,count=0;
    char *filename;
    if(parsed[1]!=NULL)
    {
        if(strstr(parsed[1],"-t")!=NULL)
        {
            t=1;
            if(numb!=NULL){
                count=atoi(numb);
            }
            else
            {
                printf("\nTab size wasn't introduced\n");
                exit(0);
            }
            filename=parsed[2];
        }
        if(strcmp(parsed[1],"-i")==0)
        {
            i=1;
            filename=parsed[2];
        }
        if(strcmp(parsed[1],"-a")==0)
        {
            a=1;
            filename=parsed[2];
        }
        if(strstr(parsed[1],"-t")==NULL && strcmp(parsed[1],"-i")!=0 && strcmp(parsed[1],"-a")!=0)
            filename=parsed[1];
    }
    FILE *f;
    char c;
    f=fopen(filename,"r");
    if(f==0){
        printf("\nCouldn't open file\n");
        exit(0);
    }
    else
    {
        //in loc de tab avem x spatii
        if(t==1)
        {
            c=fgetc(f);
            while(c!=EOF)
            {
                if(c!='\t')
                    printf("%c",c);
                else
                    for(int j=0;j<count;j++)
                        printf(" ");
                c=fgetc(f);
            }
        }
        else if(i==1)
        {
            //-i sterge taburi care sunt la inceput de linie, au inainte newline
            //ok=0 when it meets newline, ok=1 when it meets a word,space,tab
            //printf("\nAM ITNRAT IN IF LA -I\n");
            int ok=0;
            c=fgetc(f);
            while(c!=EOF)
            {
                if(c=='\t')
                {
                    if(ok==1)
                        printf("%c",c);
                    ok=1;
                }
                else if(c=='\n')
                {
                    ok=0;
                    printf("%c",c);
                }
                else
                {
                    ok=1;  
                    printf("%c",c);
                }
                c=fgetc(f);
            }
        }
        else if(a==1)
        {
            
                printf("\nSunt in -a, nu exista -a pt expand\n");
        }
        else
        {
            //la mine in terminal un tab e 8 spatii, dar poate este 4 spatii?
            c=fgetc(f);
            while(c!=EOF)
            {
                if(c!='\t')
                    printf("%c",c);
                else
                {
                    printf(" ");
                    printf(" ");
                    printf(" ");
                    printf(" ");
                    printf(" ");
                    printf(" ");
                    printf(" ");
                    printf(" ");
                }
                c=fgetc(f);
            }
        }
    }
}

void envExec(char **parsed,char **envp)
{
    char varName[MAXLETTERS];
    //printf("\nSUNT AICI\n");
    int ok=0;
    if(parsed[1]!=NULL)
    {
        //printf("\nSUNT AICI ZZZZZZZZz\n");
        if(strcmp(parsed[1],"-u")==0)
        {
            //printf("\nSUNT AICI ASKLFDHASLDKASJ\n");
            if(parsed[2]!=NULL)
            {
                //printf("\nSUNT AICI LLLLLLLLLL\n");
                strcpy(varName,parsed[2]);
                ok=1;
            }
            if(parsed[2]==NULL)
            {
                printf("\nNo variable name for env -u\n");
                return;
            }
        }
    }
    if(ok==1)
    {
        //printf("\n\n\nSUNT AICI PPPPPPPPP\n\n\n");
        for(char** i=envp;*i!=NULL;i++)
        {
            if(strstr(*i,varName)==NULL)
                printf("%s\n",*i);
        }
        //printf("\n\n======SUNT AICI3========\n\n");
    }
    else
    {
        //printf("\nSUNT AICI22222222\n");
        for(char** i=envp;*i!=NULL;i++)
        {
            char *j=*i;
            printf("%s\n",j);
        }
    }
    printf("\n\n%s\n\n",*envp);
    //printf("\n\n======SUNT AICI4444========\n\n");
}

// Function to execute builtin commands
int ownCmdHandler(char** parsed,char **envp)
{
    //commands: help,wc,expand,env,exit||||||pipes,redirection,pipes and redirection in the same command line
    int ok=0;
    //printf("\nAM INTRAT IN OWN\n");
    for(int i=0;i<6;i++)
        if(strcmp(cmds[i],parsed[0])==0)
            ok=1;
    if(ok==0)
        return 0;
    else if(ok==1){
        if(strcmp(parsed[0],"help")==0)
            helpExec();
        if(strcmp(parsed[0],"wc")==0)
            wc(parsed);
        if(strcmp(parsed[0],"env")==0)
            envExec(parsed,envp);
        if(strcmp(parsed[0],"expand")==0)
            expandExec(parsed);
        if(strcmp(parsed[0],"exit")==0)
            exit(0);
        if(strcmp(parsed[0],"hello")==0)
            testare();
        return 1;
    }
  
    return 0;
}
  
// exec pt o singura comanda
void execSimple(char** parsed,char **envp)
{
    //printf("\nSUNT IN EXEC SIMPLE\n");
    pid_t p=fork(); 
  
    if (p==-1) 
    {
        printf("\nCouldn't fork\n");
        exit(0);
    } 
    else if(p==0) 
    {
        if(ownCmdHandler(parsed,envp)==0)
            if(execvp(parsed[0],parsed)<0) 
                printf("\nCan't execute a simple command\n");
        exit(0);
    } 
    else 
    {
        wait(NULL); 
        return;
    }
}

int changeChar(char** command1, char** parsed,int commandCounter)
{
    int x=0,y;
    while(x<4)
    {
        if(parsed[commandCounter]==NULL && parsed[commandCounter+1]!=NULL)
            break;
        command1[x]=parsed[commandCounter];
        x++;
        commandCounter+=1;
    }
    y=commandCounter;
    while(y%5!=0)
    {
        command1[x]=NULL;
        x++;
        y++;
    }
    while(commandCounter%4!=0)
        commandCounter+=1;
    x=commandCounter;
    return x;
}

void redirect(char** parsed,char **filename,char **envp)
{
    //printf("\nSUNT AICI\n");
    char finalOutput[MAXLETTERS];
    int fd[2],lengthOutput;
    //printf("\n%s \n",filename[0]);
    if(filename!=NULL)
        {
            int file=open(filename[0],O_CREAT | O_RDWR | O_TRUNC, S_IRWXU);
            //printf("/n %s /n",filename[0]);
            if(file>0)
            {
                pid_t p;
                if(pipe(fd)<0){
                    printf("\nCouldn't pipe\n");
                    exit(0);
                }
                p=fork();
                if(p<0){
                    printf("\nCouldn't fork\n");
                    return;
                }
                else if(p==0)
                {
                    close(fd[0]);
                    dup2(fd[1],STDOUT_FILENO);
                    close(fd[1]);
                    execSimple(parsed,envp);
                    exit(0);
                }
                else{
                    wait(NULL);
                    close(fd[1]);
                    lengthOutput=0;
                    while(read(fd[0],&finalOutput[lengthOutput],1)!=0)
                        lengthOutput++;
                    close(fd[0]);
                    write(file,finalOutput,lengthOutput); 
                }
            }
            else
            printf("\nCouldn't open the file\n");   
        }
        
}


void execPipes(char** parsed,char **filename, int ok,int fileCheck,char **envp)
{
    char *command1[5], finalOutput[MAXLETTERS];
    //printf("\nAM INTRAT IN FUNCTIE\n");
    int fd[2],commandCounter=0,lengthOutput;
    pid_t pid;
    if(ok==0)
        outputFile=open("stdout.txt", O_CREAT | O_RDWR | O_TRUNC, S_IRWXU);
    else outputFile=open(filename[0],O_CREAT | O_RDWR | O_TRUNC, S_IRWXU);
    for(int i=0;i<=pipes;i++)
    {
        commandCounter=changeChar(command1,parsed,commandCounter);
        //printf("\n %s \n",command1[0]);
        if(pipe(fd)<0){
            printf("\nCouldn't pipe\n");
            exit(0);
        }
        pid=fork();
        if(pid<0){
            printf("\nCouldn't fork\n");
            exit(0);
        }
        if(pid==0){
            //printf("\nSUNT IN COPIL\n");
            close(fd[0]);
            dup2(fd[1],STDOUT_FILENO);
            close(fd[1]);
            if(i!=0)
            {
                dup2(outputFile,STDIN_FILENO);
            }
            if(strcmp(command1[0],"wc")!=0)
                execSimple(command1,envp);
            if(strcmp(command1[0],"wc")==0 && fileCheck==1)
                execSimple(command1,envp);
            exit(0);
        }
        else
        {
            wait(NULL);
            close(fd[1]);
            lengthOutput=0;
            while(read(fd[0],&finalOutput[lengthOutput],1)!=0)
                lengthOutput++;
            close(fd[0]);
            write(outputFile,finalOutput,lengthOutput); 

            
        }

    }
    
    
}


// separam text de |
void eliminatePipe(char* str, char** strpiped)
{
    int i;
    for (i = 0; i < (pipes+1)*4; i++) 
    {
        strpiped[i] = strsep(&str, "|");
        if (strpiped[i] == NULL)
            break;
    }
}

  
// separam cuvinte de spatiu
void eliminateSpace(char* str, char** parsed)
{
    int i=0;
    while((parsed[i]=strsep(&str," "))!=NULL)
    {
        if (parsed[i] == NULL)
            break;
        if (strlen(parsed[i]) == 0)
            i--;
        i++;
    
    }
}

void processInput(char* str, char** parsed,char **envp){
    char *pipesArgs[(pipes+1)*4],*filename[5],*str2[5];
    int ok=0;
    //printf("\nCOUNT REDIRECT==== %d\n",countRedirect(str));
    if(countRedirect(str)!=0){
        str2[0]=strtok(str,">");
        filename[0]=strtok(NULL,"> ");
        ok=1;
    }
    //printf("\nSTR+============ %s\n",str);
    if(countPipe(str)==0)
    {
        //printf("\nPRIMUL IF\n");
        if(ok==1)
        {
            //printf("\nAL DOILEA IF\n");
            eliminateSpace(str,parsed);
            redirect(parsed,filename,envp);
        }
        else 
        {
            eliminateSpace(str,parsed);
            if(ownCmdHandler(parsed,envp)==0)
                execSimple(parsed,envp);
        }
    }
    else{
        int wcCheck=0,fileCheck=0,optionCheck=0,countWc=0,countFile=0,countOption=0;
        ftruncate(outputFile,0);
        char *strpiped[pipes+1], *wcCmd[3];
        eliminatePipe(str,strpiped);
        for(int i=0;i<=pipes;i++)
        {
            eliminateSpace(strpiped[i],parsed);
            for(int j=0;j<4;j++)
                pipesArgs[i*4+j]=parsed[j];
        }
        for(int i=0;i<(pipes+1)*4;i++)
        {
            //printf("%s ",pipesArgs[i]);
            if(pipesArgs[i]!=NULL)
            {
                if(strcmp(pipesArgs[i],"wc")==0){
                    wcCheck=1;
                    countWc=i;
                }
                if(strcmp(pipesArgs[i],"-c")==0 || strcmp(pipesArgs[i],"-l")==0 || strcmp(pipesArgs[i],"-L")==0 ||strcmp(pipesArgs[i],"-w")==0){
                    optionCheck=1;
                    countOption=i;
                }
                if(strstr(pipesArgs[i],".txt")!=0 && countWc<i && countWc!=0)
                {
                    fileCheck=1;
                    countFile=i;
                }
                //if(fileCheck==1)
                    //printf("\nAICI AM GASIT FILECHECK ==== %d\n",i);
            }
            
        }
        printf("\n");
        execPipes(pipesArgs,filename,ok,fileCheck,envp);
        if((ok==0 && wcCheck==0) || (ok==0 && wcCheck==1 && fileCheck==1)){
            FILE *f;
            char c;
            f=fopen("stdout.txt","r");
            while((c=fgetc(f))!=EOF)
                printf("%c",c);
            fclose(f);
        }
        if(wcCheck!=0){
            if(fileCheck==0){
                wcCmd[0]=pipesArgs[countWc];
                wcCmd[1]=pipesArgs[countWc+1];
                wcPipe(wcCmd);
            }
        }
    }
}   


  
int main(int argc,char *argv[],char **envp)
{
    char inputString[MAXLETTERS], *parsedArgs[MAXCOMMANDS];
    char* parsedArgsPiped[MAXCOMMANDS];
    signal(SIGINT,ctrlC);
    // DUPA COMENZI, MAI SUNT 2 NULLURI EX: ls -l null null
    
    while (1) {
        printDir();
        
        if (readInput(inputString))
            continue;

        pipes=countPipe(inputString);
        redirects=countRedirect(inputString);
        processInput(inputString,parsedArgs,envp);
        
    }
    return 0;
}