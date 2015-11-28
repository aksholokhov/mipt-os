#include <unistd.h>
#include <sys/msg.h>

int* amoeba_factory(int N, char* DNA) {
    int* amoebas = (int*)malloc(N*sizeof(int));

    for (int i = 0; i < N; i++) {
        
        int msgqueue = msgget(IPC_PRIVATE, IPC_CREAT | S_IRWXU);
        int pid = fork();
        if (pid == 0) {
            amoeba(
        }

        int pipefd[2] = pipe();
        pid = fork();
        if (pid == 0) {
            close(pipefd[0]);
            kernel(pipefd[1]);
            close(pipefd[1]);
            exit(0);
        } 
        pid = fork();
        if (pid == 0) {
            close(pipefd[1]);
            body(pipefd[0]);
            close(pipefd[0]);
            exit(0);
        }
        close(pipefd[0]);
        close(pipefd[1]);
        
    }
}

void run_kernel_and_body() {

}

void replicate() {
    
}

void amoeba() {
    run body & kernel
    while (feed < 6) {
        growth
    }    
    replicate()
}

int main(int argc, char** argv) {
    
}
