#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


pthread_mutex_t rng_mutex;

int thread_safe_rng(int min, int max) {
    pthread_mutex_lock(&rng_mutex);
    int r = rand();
    pthread_mutex_unlock(&rng_mutex);
    return min + r % max;
}

/* TODO : can add global vars, structs, functions etc */

int int1 = 0;
int int2 = 0;
int int3 = 0;
int int4 = 0;

// lock for the deadlock resolver thread
pthread_mutex_t dl;
// one lock for every intersection 1,2,3,4 resp
pthread_mutex_t l1;
pthread_mutex_t l2;
pthread_mutex_t l3;
pthread_mutex_t l4;
// pthread_mutex_t get_lock(int int_num){
//     if(int_num==1){
//         return l1;
//     }else if(int_num==2){
//         return l2;
//     }else if(int_num==3){
//         return l3;
//     }else{
//         return l4;
//     }   
//     return l1;
// }

pthread_cond_t c1;
pthread_cond_t c2;
pthread_cond_t c3;
pthread_cond_t c4;


void set_int_id(int int_num, int id){
    if(int_num==1){
        int1 = id;
    }else if(int_num==2){
        int2 = id;
    }else if(int_num==3){
        int3 = id;
    }else{
        int4 = id;
    }
    return;
}

void set_int_id2(int int_num, int id){
    if(int_num==1){
        int2 = id;
    }else if(int_num==2){
        int3 = id;
    }else if(int_num==3){
        int4 = id;
    }else{
        int1 = id;
    }
    return;
}

void sig_int(int int_num){
    if(int_num == 1){
        pthread_cond_signal(&c1);
    }else if(int_num==2){
        pthread_cond_signal(&c2);
    }else if(int_num==3){
        pthread_cond_signal(&c3);
    }else{
        pthread_cond_signal(&c4);
    }
}

void sig_int2(int int_num){
    if(int_num == 1){
        pthread_cond_signal(&c2);
    }else if(int_num==2){
        pthread_cond_signal(&c3);
    }else if(int_num==3){
        pthread_cond_signal(&c4);
    }else{
        pthread_cond_signal(&c1);
    }
}

void get_lock(int int_num){
    if(int_num == 1){
        pthread_mutex_lock(&l1);
    }else if(int_num==2){
        pthread_mutex_lock(&l2);
    }else if(int_num==3){
        pthread_mutex_lock(&l3);
    }else{
        pthread_mutex_lock(&l4);
    }
}
void get_lock2(int int_num){
    if(int_num == 1){
        pthread_mutex_lock(&l2);
    }else if(int_num==2){
        pthread_mutex_lock(&l3);
    }else if(int_num==3){
        pthread_mutex_lock(&l4);
    }else{
        pthread_mutex_lock(&l1);
    }
}
void give_lock(int int_num){
    if(int_num == 1){
        pthread_mutex_unlock(&l1);
    }else if(int_num==2){
        pthread_mutex_unlock(&l2);
    }else if(int_num==3){
        pthread_mutex_unlock(&l3);
    }else{
        pthread_mutex_unlock(&l4);
    }
}
void give_lock2(int int_num){
    if(int_num == 1){
        pthread_mutex_unlock(&l2);
    }else if(int_num==2){
        pthread_mutex_unlock(&l3);
    }else if(int_num==3){
        pthread_mutex_unlock(&l4);
    }else{
        pthread_mutex_unlock(&l1);
    }
}
void arriveLane(int id, char dir, int int_num){
    printf("Arriving \n");
    get_lock(int_num);
    // printf(int_num == 1 ? "&l1" : int_num==2 ? "&l2" : int_num==3 ? "&l3" : "&l4");
    while(int1>0){
        pthread_cond_wait(int_num == 1 ? &c1 : int_num==2 ? &c2 : int_num==3 ? &c3 : &c4, int_num == 1 ? &l1 : int_num==2 ? &l2 : int_num==3 ? &l3 : &l4);
    }
    set_int_id(int_num, id);
    give_lock(int_num);
    // pthread_mutex_unlock(int_num == 1 ? &l1 : int_num==2 ? &l2 : int_num==3 ? &l3 : &l4);
    /* TODO: add code here */
}

void crossLane(int id, char dir, int int_num) {
    /* TODO: add code here */
    get_lock2(int_num);
    while(int1>0){
        pthread_cond_wait(int_num == 1 ? &c2 : int_num==2 ? &c3 : int_num==3 ? &c4 : &c1, int_num == 1 ? &l2 : int_num==2 ? &l3 : int_num==3 ? &l4 : &l1);
    }
    set_int_id2(int_num, id);
    give_lock2(int_num);

    usleep(1000 * thread_safe_rng(500, 1000)); // take 500-1000 ms to cross the lane
}

void exitLane(int id, char dir, int int_num) {
    /* TODO: add code here */
    get_lock(int_num);
    get_lock2(int_num);
    set_int_id(int_num, 0);
    set_int_id2(int_num,0);
    sig_int(int_num);
    sig_int2(int_num);
    give_lock2(int_num);
    give_lock(int_num); 
}

struct train {
    int id;
    char dir;
};

int get_int(char dir){
    if(dir=='N') return 1;
    if(dir=='W') return 2;
    if(dir=='S') return 3;
    if(dir=='E') return 4;
    return 0;
}


void *trainThreadFunction(void* arg)
{
    struct train *t = (struct train *) arg;
    char* trainDir = NULL;
    char dir = t->dir; // TODO set the direction of the train: North/South/East/West.
    int id = t->id;
    // printf("%d, %c \n",id,dir);
    /* TODO extract arguments from the `void* arg` */

    if(dir=='N'){
        trainDir = "North";
    }
    if(dir=='E'){
        trainDir = "East";
    }
    if(dir=='W'){
        trainDir = "West";
    }
    if(dir=='S'){
        trainDir = "South";
    }
    // printf("Arriving \n");
    usleep(thread_safe_rng(0, 10000)); // start at random time
    int int_num = get_int(dir);
    arriveLane(id, dir, int_num);
    printf("Train Arrived at the lane from the %s direction\n", trainDir);

    crossLane(id, dir, int_num);

    printf("Train Exited the lane from the %s direction\n", trainDir);
    exitLane(id, dir, int_num);
    return NULL;
}

void *deadLockResolverThreadFunction(void * arg) {
    /* TODO extract arguments from the `void* arg` */
    while (1) {
        /* TODO add code to detect deadlock and resolve if any */
        // printf("Deadlock thread \n");
        int deadLockDetected = 0; // TODO set to 1 if deadlock is detected

        if (deadLockDetected) {
            printf("Deadlock detected. Resolving deadlock...\n");
            /* TODO add code to resolve deadlock */
        }

        usleep(1000 * 500); // sleep for 500 ms
    }
}

int main(int argc, char *argv[]) {


    srand(time(NULL));

    if (argc != 2) {
        printf("Usage: ./main <train dirs: [NSWE]+>\n");
        return 1;
    }

    pthread_mutex_init(&rng_mutex, NULL);
    pthread_mutex_init(&l1, NULL);
    pthread_mutex_init(&l2, NULL);
    pthread_mutex_init(&l3, NULL);
    pthread_mutex_init(&l4, NULL);
    pthread_cond_init(&c1, NULL);
    pthread_cond_init(&c2, NULL);
    pthread_cond_init(&c3, NULL);
    pthread_cond_init(&c4, NULL);


    /* TODO create a thread for deadLockResolverThreadFunction */


    char* train = argv[1];

    int num_trains = 0;
    int count = strlen(train);
    // first thread for deadlock resolver function
    // rest all for trains
    pthread_t threads[count+1];
    struct train thread_args[count+1];
    // threads = malloc((count+1) * sizeof(pthread_t));
    // thread_args = malloc((count+1) * sizeof(struct train));

    pthread_create(&threads[0], NULL, deadLockResolverThreadFunction, NULL);

    while (train[num_trains] != '\0') {
        char trainDir = train[num_trains];

        if (trainDir != 'N' && trainDir != 'S' && trainDir != 'E' && trainDir != 'W') {
            printf("Invalid train direction: %c\n", trainDir);
            printf("Usage: ./main <train dirs: [NSEW]+>\n");
            return 1;
        }

        /* TODO create a thread for the train using trainThreadFunction */
        num_trains++;
        thread_args[num_trains].id = num_trains;
        thread_args[num_trains].dir = trainDir;
        pthread_create(&threads[num_trains], NULL, trainThreadFunction, &thread_args[num_trains]);

    }

    /* TODO: join with all train threads*/

    while(num_trains > 0){
        // printf("JOINING THREADS\n");
        pthread_join(&threads[num_trains], NULL);
        num_trains = num_trains - 1;
    }



    return 0;
}