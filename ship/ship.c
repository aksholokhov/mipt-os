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
#include <sys/shm.h>
#include <stdlib.h>
#include <time.h>


//initial number of passengers
#define PNUM 10

#define SHIP_CAPACITY 3

int main(int argc, char** argv) {
    //We'll use 2 semaphores: the first for "tickets for in" and
    //the second for "tickets for out"
    int sem_id = semget(IPC_PRIVATE, 2, IPC_CREAT | S_IRWXU);
    if (sem_id == -1) {
        perror("fail to create semaphore");
    } 
    int p_pid[PNUM];
    //We'll use shared memory for current number of people on the beach
    int key = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | S_IRWXU);
    int* p_num = (int*)shmat(key, NULL, 0);
    *p_num = PNUM;

    int ship_pid = fork();
    if (ship_pid == 0) {
        //ship
        p_num = (int*)shmat(key, NULL, 0);
        struct sembuf sb;
        while(*p_num > 0) {    
            sb.sem_num = 0;
            sb.sem_op = (SHIP_CAPACITY > *p_num) ? *p_num : SHIP_CAPACITY;
            sb.sem_flg = 0;
            printf("CAPTAIN: Desk has opened for boarding\n");
            if (semop(sem_id, &sb, 1) == -1) {
                perror("CAPTAIN: Can't open desk for boarding");
                return 1;
            }
            sb.sem_op = 0;
            if (semop(sem_id, &sb, 1) == -1) {
                perror("CAPTAIN: Wait fail");
                return 1;
            }
            //these sieeps need only for better log reading (without them log
            //sometimes renders in a wrong order) 
            sleep(1);
            printf("CAPTAIN: Ship is full now! Sail away! \n ~~~~~~~~~~~~~ \n");
            sleep(3);
            printf("CAPTAIN: Arrived, good bye, sailors! \n");
            sb.sem_num = 1;
            sb.sem_op = (SHIP_CAPACITY > *p_num) ? *p_num : SHIP_CAPACITY;
            if (semop(sem_id, &sb, 1) == -1) {
                perror("CAPTAIN: Can't open desk for arriving");
                return 1;
            }
            sb.sem_op = 0;
            if (semop(sem_id, &sb, 1) == -1) {
                perror("CAPTAIN: Can't wait for passengers leaving");
                return 1;
            }
            sleep(1);
        }
        printf("CAPTAIN: We finished!\n");
        shmdt(p_num);
        return 0;
    }
    else {
        for (int i = 0; i < PNUM; i++) {
            int pid = fork();
            if (pid == 0) {
              //passenger
                p_num = (int*)shmat(key, NULL, 0);
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
                    printf("PASSENGER %d: left the ship and ", i);
                    srand(time(NULL)*(i+1));
                    int stay = rand() % 2;
                    if (stay) {
                        printf("wants again! \n");
                    } else {
                        printf("was tired: went home.\n");
                        (*p_num)--;
                        shmdt(p_num);
                        return 0;
                    }
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
    shmdt(p_num);
    shmctl(key, IPC_RMID, NULL);
    semctl(sem_id, 0, IPC_RMID);
    return 0;
}
