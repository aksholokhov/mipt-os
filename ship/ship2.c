#include <unistd.h>
#include <strings.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdio.h>


int PNUM = 0;
int CAPACITY = 0;

//semaphores for boarding and leaving [2x2] - four binary semaphores
int in = 0;
int out = 0;


// just a pattern of semaphore operation
void semopr(int sem_id, int sem_no, int sem_op) {
    struct sembuf sb;
    sb.sem_num = sem_no;
    sb.sem_op = sem_op;
    sb.sem_flg = 0;
    if (semop(sem_id, &sb, 1) == -1) {
        perror("can't use semaphores\n");
    }
}

void P (int sem_id, int sem_no) {
    semopr(sem_id, sem_no, -1);
}

void V (int sem_id, int sem_no) {
    semopr(sem_id, sem_no, 1);
}

void start_ship() {
    int wanna_go = PNUM;
    while (wanna_go > 0) {
        int passengers_on_board = wanna_go < CAPACITY ? wanna_go : CAPACITY;
        printf("SHIP: welcome, passengers!\n");
        for (int i = 0; i < passengers_on_board; i++) {
            //open trap for a passenger
            V(in, 0);
            //"awake me when you finish boarding"
            P(in, 1);
        }
        printf("SHIP: boarding finished, sail away!\n");
        printf("~~~~~~~~~~\n");
        printf("SHIP: arrived, good bye, sailors!\n");
        for (int i = 0; i < passengers_on_board; i++) {
            //open trap for out
            V(out, 0);
            //"awake me when you finish leaving"
            P(out, 1);
        }
        wanna_go -= passengers_on_board;
    }
    printf("SHIP: we finished!\n");
}

void start_passenger(int num) {
    //try to earn the trap for boarding
    P(in, 0);
    printf("PASSENGER %d: onboard\n", num);
    //awake ship to allow to board to a next passenger
    V(in, 1);
    //try to leave the ship
    P(out, 0);
    printf("PASSENGER %d: left the ship\n", num);
    //awake ship to allow to leave to a next passenger
    V(out, 1);
}

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Wrong args. Should be: PNUM, CAPACITY\n");
        return 1;
    }

    PNUM = atoi(argv[1]);
    CAPACITY = atoi(argv[2]);
    
    //two semaohores for in (trap and "ship alarm clock")
    in = semget(IPC_PRIVATE, 2, IPC_CREAT | S_IRWXU);
    //two the same for out
    out = semget(IPC_PRIVATE, 2, IPC_CREAT | S_IRWXU);
    
    if (in == -1 || out == -1) {
        perror("can't init semaphores");
        return 1;
    }

    int passengers_pid[PNUM];

    for (int i = 0; i < PNUM; i++) {
        passengers_pid[i] = fork(); 
        if (passengers_pid[i] == 0) {
            start_passenger(i);
            return 0;
        }
    }

    int ship_pid = fork();
    if (ship_pid == 0) {
        start_ship();
        return 0;
    }

    int status;
    waitpid(ship_pid, &status, 0);
    
    //we assume, that if ship finished, then all passengers are "dead", so
    //kill() isn't necessary
    for (int i = 0; i < PNUM; i++) {
        waitpid(passengers_pid[i], &status, 0);
    }

    semctl(in, 0, IPC_RMID);
    semctl(out, 0, IPC_RMID);

    return 0;
}
