#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define LIFTS 2
#define GUYS 10
#define LIFT_TRIPS 3


int in[2];
int out;
int total;

void V(int sem, int i) {
    struct sembuf sb;
    sb.sem_num = i;
    sb.sem_flg = 0;
    sb.sem_op = 1;
    if(semop(sem, &sb, 1) == -1) {
        perror("can't use semaphor");
    }
}

void P(int sem, int i) {
    struct sembuf sb;
    sb.sem_num = i;
    sb.sem_flg = 0;
    sb.sem_op = -1;
    if(semop(sem, &sb, 1) == -1) {
        perror("can't use semaphor");
    }
}

int start_lift(int i) {
    int k = 1;
    V(in[k], i);
    printf("LIFT %d: Opened\n", i);
    int* ttl = (int*)shmat(total, 0, NULL);
    for (int j = 0; j < LIFT_TRIPS; j++) {
        P(in[k], i);
        k = k ^ 1;
        printf("LIFT %d: Closed, moved to %d floor\n", i, k);
        if (ttl[i] == 0) {
            printf("LIFT %d: Empty, opened for in\n", i);
            V(in[k], i);
        } 
        else {
            printf("LIFT %d: Not empty, opened for out\n", i);
            V(out, i); 
        }
        printf("LIFT %d: wait for close of floor %d\n", i, k);
    }
    shmdt(ttl);

    printf("Lift %d: finished\n", i);

    return 0;
}

int start_guy(int num) {
    int *ttl = (int*)shmat(total, 0, NULL);
    int k = 1;
    while(1) {
        srand(time(NULL)*(num+1));
        int i = rand() % LIFTS;
        P(in[k], i);
        ttl[i]++;
        printf("GUY %d: entered lift %d on floor %d, total - %d\n", num, i, k, ttl[i]);
        V(in[k], i);
        k = k ^ 1;
        P(out, i);
        if (--ttl[i] == 0) {
            printf("GUY %d: left lift %d on floor %d last, lift opened for in\n", num, i, k);
            V(in[k], i);
        } 
        else {
            printf("GUY %d: left lift %d on floor %d not last\n", num, i, k);
            V(out, i);
        } 
    }
    shmdt(ttl);
    return 0;
}

int main() {
    
    in[0] = semget(IPC_PRIVATE, LIFTS, IPC_CREAT | S_IRWXU);
    in[1] = semget(IPC_PRIVATE, LIFTS, IPC_CREAT | S_IRWXU);
    out = semget(IPC_PRIVATE, LIFTS, IPC_CREAT | S_IRWXU);
    total = shmget(IPC_PRIVATE, LIFTS*sizeof(int), IPC_CREAT | S_IRWXU);
    
    int guy[GUYS];
    int lift[LIFTS];

    for (int i = 0; i < GUYS; i++) {
        int pid = fork();
        if (pid == 0) {
            start_guy(i);
        } else {
            guy[i] = pid;
        }
    }

    int alive = LIFTS;

    for (int i = 0; i < LIFTS; i++) {
        int pid = fork();
        if (pid == 0) {
            start_lift(i);
        } else {
            lift[i] = pid;
        }
    }
    
    int status;
    while (alive > 0) {
        waitpid(-1, &status, 0);
        alive--; 
    }
    
    for (int i = 0; i < GUYS; i++) {
        kill(guy[i], SIGKILL);
    }

    semctl(in[0], 0, IPC_RMID);
    semctl(in[1], 0, IPC_RMID);
    semctl(out, 0, IPC_RMID);
    shmctl(total, IPC_RMID, NULL);
    
    return 0;
}
