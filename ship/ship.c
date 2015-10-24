#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>

#define TRIPNUM 5
#define PNUM 100
#define SHIP_CAPACITY 3

int main(int argc, char** argv) {
    int sem_id = semget(IPC_PRIVATE, 2, IPC_CREAT | S_IRWXU);
    if (sem_id == -1) {
        perror("fail to create semaphore");
    } 
    int p_pid[PNUM];
    int ship_pid = fork();
    if (ship_pid == 0) {
        //ship
        struct sembuf sb;
        for (int i = 0; i < TRIPNUM; i++) {    
            sb.sem_num = 0;
            sb.sem_op = SHIP_CAPACITY;
            sb.sem_flg = 0;
            printf("SHIP: Desk has opened for boarding\n");
            if (semop(sem_id, &sb, 1) == -1) {
                perror("SHIP: Can't open desk for boarding");
                return 1;
            }
            sb.sem_op = 0;
            if (semop(sem_id, &sb, 1) == -1) {
                perror("SHIP: Wait fail");
                return 1;
            }
            sleep(1);
            printf("SHIP: Ship is full now! Sail away! \n");
            sleep(3);
            printf("SHIP: Arrived, good bye, sailors! \n");
            sb.sem_num = 1;
            sb.sem_op = SHIP_CAPACITY;
            if (semop(sem_id, &sb, 1) == -1) {
                perror("SHIP: Can't open desk for arriving");
                return 1;
            }
            sb.sem_op = 0;
            if (semop(sem_id, &sb, 1) == -1) {
                perror("SHIP: Can't wait for passengers leaving");
                return 1;
            }
            sleep(1);
        }
    }
    else {
        for (int i = 0; i < PNUM; i++) {
            int pid = fork();
            if (pid == 0) {
              //passenger
                struct sembuf sb;
                while (1) {
                    sb.sem_num = 0;
                    sb.sem_op = -1;
                    sb.sem_flg = 0;
                    if (semop(sem_id, &sb, 1) == -1) {
                        perror("Failed to board");
                        return 1;
                    } 
                    printf("PASSENGER %d: boarded \n", i);
                    sb.sem_num = 1;
                    if (semop(sem_id, &sb, 1) == -1) {
                        perror("Failed to leave");
                        return 1;
                    }
                    printf("PASSENGER %d: left the ship \n", i);
                }
            } else {
                p_pid[i] = pid;
            }   
        }
    }
    int status;
    waitpid(ship_pid, &status, 0);
    for (int i = 0; i < PNUM; i++) {
        kill(p_pid[i], SIGKILL);
        waitpid(p_pid[i], &status, 0);
    }
    return 0;
}
