#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>

#define MAX 100
#define THINKING 0
#define HUNGRY 1
#define EATING 2
#define N 5
#define clear() printf("\033[H\033[J")



static struct sembuf buf;
static struct sembuf philosophers_sem;
int semid;

void test(int*states,int*food_eaten,int index);

struct buf_elem {
    long mtype;
    int mvalue;
};


void sem_V(int semid, int semnum){
    buf.sem_num = semnum;
    buf.sem_op = 1;
    buf.sem_flg = 0;
    if (semop(semid, &buf, 1) == -1){
        perror("Podnoszenie semafora");
        exit(1);
    }
}

void sem_P(int semid, int semnum){
    buf.sem_num = semnum;
    buf.sem_op = -1;
    buf.sem_flg = 0;
    if (semop(semid, &buf, 1) == -1){
        perror("Opuszczenie semafora");
        exit(1);
    }
}



void pickup(int*states,int*food_eaten,int index,int msgid,struct buf_elem elem){
    states[index] = HUNGRY;
    test(states,food_eaten,index);

    while(states[index] != EATING){
        msgrcv(msgid, &elem, sizeof(elem.mvalue), index, 0);
        test(states,food_eaten,index);
    }


}

void putdown(int*states,int*food_eaten,int index,int msgid,struct buf_elem elem){
    states[index] = THINKING;
    elem.mtype = (index+1)%5;
    msgsnd(msgid, &elem, sizeof(elem.mvalue), 0);
    elem.mtype = (index+4)%5;
    msgsnd(msgid, &elem, sizeof(elem.mvalue), 0);
    sleep(2);
}


void test(int*states,int*food_eaten,int index){
        sem_P(semid,0);

        if((states[(index+1)%5] == THINKING || (states[(index+1) % 5] == HUNGRY && food_eaten[index] < food_eaten[(index+1)%5]) || (states[(index+1) % 5] == HUNGRY && food_eaten[index] >= food_eaten[(index+1)%5]) && states[(index+2) % 5] == EATING ) &&
                (states[(index+4)%5] == THINKING || (states[(index+4) % 5] == HUNGRY && food_eaten[index] <= food_eaten[(index+4)%5]) || ((states[(index+4) % 5] == HUNGRY && food_eaten[index] >= food_eaten[(index+4)%5]) && states[(index+3) % 5] == EATING) ))
                     states[index] = EATING;

        sem_V(semid,0);

    }





void philosopher(int*states,int*food_eaten,int id,int msgid,struct buf_elem signal){
    while(1) {
        //printf("%d\n",id);
        pickup(states,food_eaten,id,msgid,signal);
        sleep(2);
        food_eaten[id]+=2;
        putdown(states,food_eaten,id,msgid,signal);

    }
}

int main(){
    int *states,*food_eaten,shmid1,shmid2;
    char state_name[3][64];
    int msgid;
    struct buf_elem signal;
    msgid = msgget(IPC_PRIVATE,IPC_CREAT|IPC_EXCL|0600);
    signal.mvalue = 0;

    strcpy(state_name[THINKING],"THINKING");
    strcpy(state_name[HUNGRY],"HUNGRY  ");
    strcpy(state_name[EATING],"EATING  ");

    semid = semget(IPC_PRIVATE,6,IPC_CREAT|0600);
    semctl(semid, 0, SETVAL, 1);


    shmid1 = shmget(IPC_PRIVATE,N* sizeof(int),IPC_CREAT|0600); //statusy
    shmid2 = shmget(IPC_PRIVATE,N* sizeof(int),IPC_CREAT|0600);
    states = (int*)shmat(shmid1, NULL, 0);
    food_eaten = (int*)shmat(shmid2, NULL, 0);

    for (int i = 0 ; i < N;i++){
        states[i] = THINKING;
        food_eaten[i] = 0;
    }

    for(int i = 0 ; i < 5; i ++){
        if(fork() > 0)
            philosopher(states,food_eaten,i,msgid,signal);

    }

        while(1){
            clear();
            for(int i = 0; i < N;i++){
                printf("Filozof %d w stanie %s zjadl lacznie %d\n",i+1,state_name[states[i]],food_eaten[i]);
            }
            printf("\n");
            sleep(1);
        }



}




