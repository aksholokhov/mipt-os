#define _POSIX_C_SOURCE 199309L
#define _GNU_SOURCE

#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>



// Empty signal handler for SIGINT and SIGCHLD interception
void signal_handler(int signo) {
    //do nothing
}

//Runs the sequence of the programs
int runpiped(char*** programs, size_t n) {
    
    sigset_t mask;
    siginfo_t info;
    // Block all SIGINT and SIGCHLD events to maintain them manually
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGCHLD);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    

    struct sigaction action;
    struct sigaction oldaction, oldaction2;

    action.sa_handler = signal_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_SIGINFO;
    
    // Save previous actions and deploy new
    sigaction(SIGINT, NULL, &oldaction);
    sigaction(SIGCHLD, NULL, &oldaction2);

    sigaction(SIGINT, &action, NULL);
    sigaction(SIGCHLD, &action, NULL);

    // Create a pipeline
    int pipefd[2];
    int* children = (int*)malloc(sizeof(int)*n);
    int* pipes = (int*)malloc(sizeof(int)*2*(n-1));

    for (int i = 0; i < n-1; i+=1) {
        if (pipe2(pipefd, O_CLOEXEC) == -1) {
            for (int j = i-1; j>= 0; j--) {
                close(pipes[2*j]);
                close(pipes[2*j + 1]);
            }
            return 1;
        }
        pipes[2*i] = pipefd[0];
        pipes[2*i+1] = pipefd[1];
    }

    // Invoke all programs
    size_t alive = 0;
    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            
        }
        if (pid == 0) {
            // Establish connections between threads
            if (i != 0) {
                dup2(pipes[2*i - 2], STDIN_FILENO);
            }
            if (i != n - 1) {
                dup2(pipes[2*i + 1], STDOUT_FILENO);
            }
            //sigprocmask(SIG_UNBLOCK, &mask, NULL);
            // exec the program
            return execvp(programs[i][0], programs[i]);          
        } else {
            children[i] = pid;
            alive++;
        }
    }   
    // close all pipes after program starting
    for (int i = 0; i < 2*(n-1); i++) {
        close(pipes[i]);
    }


    // wait passively children finishing or SIGINT appearance
    while (alive) {
        if (sigwaitinfo(&mask, &info) == -1) {
            continue;
        }
        int pid;
        switch(info.si_signo) {
            // if one of the children changed his state - terminate everything and exit
            case SIGCHLD:
                if (children[n-1] == info.si_pid)
                while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
                    for (int i = 0; i < n; i++) {
                        if (children[i] == pid) {
                            children[i] = 0;
                            alive--;
                            break;
                        }
                    }
                }
                // return old actions to it's place and finish
                // the same for sigint
            case SIGINT:
                alive = 0;
        }
    }

    int status;
    int retcode;
    for (int i = 0; i < n; i++) {
        if (children[i] > 0) {
            kill(children[i], SIGKILL);
            waitpid(children[i], &status, 0);
            if (i == n-1) {
                retcode = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
            }
        }
    } 

    sigaction(SIGINT, &oldaction, NULL);
    sigaction(SIGCHLD, &oldaction2, NULL);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);

    return retcode;
}


int main() {

	char buf[2048];
    
    char dollar[5] = "\n$ ";
    int count = 0;
    char space[1] = " ";

    //Custon sigaction for SIGINT (CTRL+C) interrupting 
    struct sigaction action;
                       
    char stick[1] = "|";
    action.sa_handler = signal_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGINT, &action, NULL);     
    
	while(1) {
        write(STDOUT_FILENO, &dollar, 3);
        char*** programs = (char***)malloc(sizeof(char**)*20);
	    fgets(buf, 2048, stdin); 
        count = strlen(buf);
        
	    if (count == -1) {
            if (errno != EINTR) {
                break;
            } else {
                errno = 0;
                continue;
            }
        }
        //if it's just a '\n' - try again
        if (count == 1) {
            buf[0] = 0;
            continue;
        } 

        //if it's a 0 symbol (CTRL-D) - exit
        if (count == 0) {
            char exit_text[9] = " exit...\n";
            int write_count = write(STDOUT_FILENO, exit_text, sizeof(exit_text)); 
            if (write_count == -1) {
                exit(1);
            }
            return 0;
        }

        //delete last \n
        buf[count-1]=0;
        //Split line by '|' and parse programs
        char** programs_lines = (char**)malloc(sizeof(char*)*20);
        char* program = strtok(buf, stick);
        int pNum = 0;
        while (program != NULL) {
            programs_lines[pNum++] = program;
            program = strtok(NULL, stick);
        }

        for (int i = 0; i < pNum; i++) {
            char* name = strtok(programs_lines[i], space);
            char** args = (char**)malloc(sizeof(char*)*10);
            args[0] = name;
            int arg_count = 1;
            char* arg = strtok(NULL, space);
            while(arg != NULL) {
                args[arg_count++] = arg;
                arg = strtok(NULL, space);
            }
            args[arg_count] = 0;
            programs[i] = args;
        }
        free(programs_lines);

        //run the sequence of programs and fetch return code
        int retcode = runpiped(programs, pNum);
        // do whatever you want with it

        buf[0] = 0;
    }
}
