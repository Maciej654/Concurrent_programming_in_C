#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define WAITING 0
#define OUTSIDE 1
#define INSIDE 2
#define ENTERING 3
#define LEAVING 4

#define clear() printf("\033[H\033[J")

void * ship_route(void * s);

pthread_mutex_t mut =PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond=PTHREAD_COND_INITIALIZER;

struct Ship{
    struct Dock* dock;
    int mass;
    int id;
    int flag;

};

struct Ship * new_Ship(struct Dock* dock, int mass,int id){
    struct Ship* ship = malloc(sizeof(struct Ship));
    ship->dock = dock;
    ship->mass = mass;
    ship->id = id;
    ship->flag = OUTSIDE;
    return ship;
}


struct Dock{
    int dock_space;
    int tug_counter;
    int ship_counter;
    struct Ship ** ships;

};


struct Dock* new_Dock(int dc,int tc,int sc,int*mass){
    struct Dock* dock = malloc(sizeof(struct Dock));
    dock->dock_space = dc;
    dock->tug_counter = tc;
    dock->ship_counter = sc;

    dock->ships  = malloc(sizeof(struct Ship)*dock->ship_counter);
    for(int i = 0 ; i < 5;i++){
        dock->ships[i] = new_Ship(dock,mass[i],i+1);
    }
    return dock;
}
void * show_stuff(void * d1) {
    char state_name[5][64];
    strcpy(state_name[WAITING], "CZEKA");
    strcpy(state_name[INSIDE], "W PORCIE");
    strcpy(state_name[OUTSIDE], "NA MORZU");
    strcpy(state_name[ENTERING], "DOPLYWAJACY");
    strcpy(state_name[LEAVING], "ODPLYWAJACY");
    struct Dock *dock = (struct Dock *) d1;

    while (1) {
        clear();
        printf("WOLNE MIEJSCA: %d\n",dock->dock_space);
        printf("WOLNE HOLOWNIKI: %d\n",dock->tug_counter);
        for (int i = 0; i < dock->ship_counter; i++) {
            printf("Statek nr %d w stanie: %s\n", dock->ships[i]->id, state_name[dock->ships[i]->flag]);
        }
        sleep(1);

    }
    return NULL;
}


void run_simulation(struct Dock* dock){
    pthread_t thread[dock->ship_counter];
    pthread_t thread_show;
    for(int i = 0 ; i < dock->ship_counter ; i++)
        pthread_create(&thread[i],NULL,&ship_route,(void*)(dock->ships[i]));

    pthread_create(&thread_show,NULL,&show_stuff,(void*)(dock));

    for(int i = 0 ; i < dock->ship_counter;i++)
        pthread_join(thread[i],NULL);
    pthread_join(thread_show,NULL);
}

void enter(struct Ship* ship){
    ship->flag = WAITING;
    pthread_mutex_lock(&mut);

    while(ship->dock->dock_space == 0 || ship->dock->tug_counter < ship->mass){
        pthread_cond_wait(&cond,&mut);
        pthread_cond_signal(&cond);

    }


    ship->dock->dock_space--;
    ship->dock->tug_counter -= ship->mass;
    pthread_mutex_unlock(&mut);
    ship->flag = ENTERING;
    sleep(2);

    pthread_mutex_lock(&mut);
    ship->dock->tug_counter += ship->mass;
    pthread_mutex_unlock(&mut);


    ship->flag = INSIDE;

}
void leave(struct Ship* ship){
    ship->flag = WAITING;

    pthread_mutex_lock(&mut);
    while(ship->dock->tug_counter < ship->mass){
        pthread_cond_wait(&cond,&mut);
        pthread_cond_signal(&cond);

    }
    ship->dock->tug_counter -= ship->mass;
    pthread_mutex_unlock(&mut);

    ship->flag = LEAVING;
    sleep(2);
    pthread_mutex_lock(&mut);
    ship->dock->dock_space++;
    ship->dock->tug_counter += ship->mass;
    pthread_mutex_unlock(&mut);


    pthread_cond_signal(&cond);



    ship->flag = OUTSIDE;

}

void * ship_route(void *s1){
    struct Ship*s = (struct Ship*)s1;
    while(1){
        enter(s1);
        sleep(5+rand()%5);
        leave(s1);
        sleep(5+rand()%5);
    }
    return NULL;
}

int main(){
    int dock_space = 3,tug_counter = 12,ship_counter = 5;
    int  mass_of_ships[5] = {1,2,3,4,5};
    struct Dock*dock = new_Dock(dock_space,tug_counter,ship_counter,mass_of_ships);

    run_simulation(dock);



}