#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>


pthread_mutex_t rng_mutex;

int thread_safe_rng(int min, int max) {
    pthread_mutex_lock(&rng_mutex);
    int r = rand();
    pthread_mutex_unlock(&rng_mutex);
    return min + r % max;
}

/* TODO : can add global vars, structs, functions etc */

volatile int locked1 = 0;
volatile int locked2 = 0;
volatile int locked3 = 0;
volatile int locked4 = 0;
pthread_mutex_t intersection1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t intersection2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t intersection3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t intersection4 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t int1con  = PTHREAD_COND_INITIALIZER;
pthread_cond_t int2con  = PTHREAD_COND_INITIALIZER;
pthread_cond_t int3con  = PTHREAD_COND_INITIALIZER;
pthread_cond_t int4con  = PTHREAD_COND_INITIALIZER;


// arriveLane() is called when the train's front end reaches the first intersection
void arriveLane(int firstIntersection) {
    if(firstIntersection==1){
        while(locked1==1){
            pthread_cond_wait(&int1con, &intersection1);
        }
        locked1 = 1;
    }else if(firstIntersection==2){
        while(locked2==1){
            pthread_cond_wait(&int2con, &intersection2);
        }
        locked2 = 1;
    }else if(firstIntersection==3){
        while(locked3==1){
            pthread_cond_wait(&int3con, &intersection3);
        }
        locked3 = 1;
    }else if(firstIntersection==4){
        while(locked4==1){
            pthread_cond_wait(&int4con, &intersection4);
        }
        locked4 = 1;
    }else{
        printf("Illegal first intersection identifier\n");
        return;
    }
    /* TODO: add code here */
}

// crossLane() is called when the train is crossing the lane, post arrivel/arriveLane()
void crossLane(int secondIntersection) {
    /* TODO: add code here */
    if(secondIntersection==1){
        while(locked1==1){
            pthread_cond_wait(&int1con, &intersection1);
        }
        locked1 = 1;
    }else if(secondIntersection==2){
        while(locked2==1){
            pthread_cond_wait(&int2con, &intersection2);
        }
        locked2 = 1;
    }else if(secondIntersection==3){
        while(locked3==1){
            pthread_cond_wait(&int3con, &intersection3);
        }
        locked3 = 1;
    }else if(secondIntersection==4){
        while(locked4==1){
            pthread_cond_wait(&int4con, &intersection4);
        }
        locked4 = 1;
    }else{
        printf("Illegal second intersection identifier\n");
        return;
    }
    // crossing second intersection (first too, since the train is long)
    usleep(1000 * thread_safe_rng(500, 1000)); // take 500-1000 ms to cross the lane
}

// exitLane() is called when the complete train exits the lane 
void exitLane(int firstIntersection, int secondIntersection) {
    /* TODO: add code here */
    if(firstIntersection==1){
        while(locked1==1){
            pthread_cond_wait(&int1con, &intersection1);
        }
        locked1 = 0;
    }else if(firstIntersection==2){
        while(locked2==1){
            pthread_cond_wait(&int2con, &intersection2);
        }
        locked2 = 0;
    }else if(firstIntersection==3){
        while(locked3==1){
            pthread_cond_wait(&int3con, &intersection3);
        }
        locked3 = 0;
    }else if(firstIntersection==4){
        while(locked4==1){
            pthread_cond_wait(&int4con, &intersection4);
        }
        locked4 = 0;
    }else{
        printf("Illegal first intersection identifier\n");
        return;
    }
    if(secondIntersection==1){
        while(locked1==1){
            pthread_cond_wait(&int1con, &intersection1);
        }
        locked1 = 0;
    }else if(secondIntersection==2){
        while(locked2==1){
            pthread_cond_wait(&int2con, &intersection2);
        }
        locked2 = 0;
    }else if(secondIntersection==3){
        while(locked3==1){
            pthread_cond_wait(&int3con, &intersection3);
        }
        locked3 = 0;
    }else if(secondIntersection==4){
        while(locked4==1){
            pthread_cond_wait(&int4con, &intersection4);
        }
        locked4 = 0;
    }else{
        printf("Illegal first intersection identifier\n");
        return;
    }
}

// points to note:
// 1. the train blocks the first intersection of the lane well in advance
// 2. to cross the lane it must block the second intersection of the lane as well
// 3. train is assumed to be very large, safe to unblock the two intersections only
//      after it has completed exited the complete lane i.e. once exitLane() has been called

// Safety: No crash can occur, and only one train can use a lane a time. 
// Liveness: The system is not stuck forever.
// Starvation: No train should be starved from entering the lane.

void trainThreadFunction(void* arg)
{
    printf("TTF Function running .. \n");
    printf("%s\n",arg);
    /* TODO extract arguments from the `void* arg` */
    int firstIntersection = 0;
    char direction = '_';
    int secondIntersection = 0;

    if(firstIntersection==1){
        secondIntersection = 2;
    }else if(firstIntersection==2){
        secondIntersection = 3;
    }else if(firstIntersection==3){
        secondIntersection = 4;
    }else if(firstIntersection==4){
        secondIntersection = 1;
    }

    usleep(thread_safe_rng(0, 10000)); // start at random time

    char* trainDir = NULL; // TODO set the direction of the train: North/South/East/West.
    
    if(direction=='N'){
        trainDir = "North";
    }else if(direction=='S'){
        trainDir = "South";
    }else if(direction=='E'){
        trainDir = "East";
    }else if(direction=='W'){
        trainDir = "West";
    }else{
        printf("Illegal direction\n");
        return;
    }

    if(firstIntersection==1){
        pthread_mutex_lock(&intersection1);
    }else if(firstIntersection==2){
        pthread_mutex_lock(&intersection2);
    }else if(firstIntersection==3){
        pthread_mutex_lock(&intersection3);
    }else if(firstIntersection==4){
        pthread_mutex_lock(&intersection4);
    }else{
        printf("Illegal first intersection identifier\n");
        return;
    }

    arriveLane(firstIntersection);
    printf("Train Arrived at the lane from the %s direction\n", trainDir);

    if(secondIntersection==1){
        pthread_mutex_lock(&intersection1);
    }else if(secondIntersection==2){
        pthread_mutex_lock(&intersection2);
    }else if(secondIntersection==3){
        pthread_mutex_lock(&intersection3);
    }else if(secondIntersection==4){
        pthread_mutex_lock(&intersection4);
    }else{
        printf("Illegal second intersection identifier\n");
        return;
    }
    crossLane(secondIntersection);

    if(direction=='N'){
        trainDir = "South";
    }else if(direction=='S'){
        trainDir = "North";
    }else if(direction=='E'){
        trainDir = "West";
    }else if(direction=='W'){
        trainDir = "East";
    }else{
        printf("Illegal direction\n");
        return;
    }

    printf("Train Exited the lane from the %s direction\n", trainDir);
    exitLane(firstIntersection, secondIntersection);

    if(firstIntersection==1){
        pthread_mutex_unlock(&intersection1);
    }else if(firstIntersection==2){
        pthread_mutex_unlock(&intersection2);
    }else if(firstIntersection==3){
        pthread_mutex_unlock(&intersection3);
    }else if(firstIntersection==4){
        pthread_mutex_unlock(&intersection4);
    }else{
        printf("Illegal first intersection identifier\n");
        return;
    }

    if(secondIntersection==1){
        pthread_mutex_unlock(&intersection1);
    }else if(secondIntersection==2){
        pthread_mutex_unlock(&intersection2);
    }else if(secondIntersection==3){
        pthread_mutex_unlock(&intersection3);
    }else if(secondIntersection==4){
        pthread_mutex_unlock(&intersection4);
    }else{
        printf("Illegal second intersection identifier\n");
        return;
    }
}

void deadLockResolverThreadFunction(void * arg) {
    /* TODO extract arguments from the `void* arg` */
    printf("Background deadlock detector is running...\n");
    while (1) {
        /* TODO add code to detect deadlock and resolve if any */

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

    /* TODO create a thread for deadLockResolverThreadFunction */
    pthread_t dLRTFThread;
    pthread_create(&dLRTFThread, NULL, deadLockResolverThreadFunction, NULL);

    char* train = argv[1];
    // printf("%d\n",strlen(train));
    // printf("%s\n", train);

    int num_trains = 0;
    int total_num_trains = strlen(train);
    pthread_t train_threads[total_num_trains];
    while (train[num_trains] != '\0') {
        char trainDir = train[num_trains];

        if (trainDir != 'N' && trainDir != 'S' && trainDir != 'E' && trainDir != 'W') {
            printf("Invalid train direction: %c\n", trainDir);
            printf("Usage: ./main <train dirs: [NSEW]+>\n");
            return 1;
        }
        char* args = trainDir;
        printf("trainDir %c\n", trainDir);
        /* TODO create a thread for the train using trainThreadFunction */
        pthread_create(&train_threads[num_trains], NULL, trainThreadFunction, args);

        num_trains++;
    }

    /* TODO: join with all train threads*/
    for(int i=0; i<num_trains; i++){
        pthread_join(train_threads[i], NULL);
    }

    return 0;
}