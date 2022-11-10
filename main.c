#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
// final submission - 10/11/2022

int debug = 0;
pthread_mutex_t rng_mutex;

typedef struct {
    char trainDir;
    int trainNo;
} myarg_t;

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
volatile int signalled = 0;
volatile int deadlockTrainNo = 0;
volatile int deadlockIntersectionNo = 2;
pthread_mutex_t deadlockResolve = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t intersection1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t intersection2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t intersection3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t intersection4 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t int1open  = PTHREAD_COND_INITIALIZER;
pthread_cond_t int2open  = PTHREAD_COND_INITIALIZER;
pthread_cond_t int3open  = PTHREAD_COND_INITIALIZER;
pthread_cond_t int4open  = PTHREAD_COND_INITIALIZER;
pthread_cond_t int1open2  = PTHREAD_COND_INITIALIZER;
pthread_cond_t int2open2  = PTHREAD_COND_INITIALIZER;
pthread_cond_t int3open2  = PTHREAD_COND_INITIALIZER;
pthread_cond_t int4open2  = PTHREAD_COND_INITIALIZER;


// arriveLane() is called when the train's front end reaches the first intersection
void arriveLane(int firstIntersection, int trainNo) {

    if(firstIntersection==1){
        pthread_mutex_lock(&intersection1);
        while(locked1!=0){
            pthread_cond_wait(&int1open, &intersection1);
        }
        locked1 = trainNo;
        // pthread_cond_signal(&int1closed);
        pthread_mutex_unlock(&intersection1);
    }else if(firstIntersection==2){
        pthread_mutex_lock(&intersection2);
        while(locked2!=0){
            pthread_cond_wait(&int2open, &intersection2);
        }
        locked2 = trainNo;
        // pthread_cond_signal(&int2closed);
        pthread_mutex_unlock(&intersection2);
    }else if(firstIntersection==3){
        pthread_mutex_lock(&intersection3);
        while(locked3!=0){
            pthread_cond_wait(&int3open, &intersection3);
        }
        locked3 = trainNo;
        // pthread_cond_signal(&int3closed);
        pthread_mutex_unlock(&intersection3);
    }else if(firstIntersection==4){
        pthread_mutex_lock(&intersection4);
        while(locked4!=0){
            pthread_cond_wait(&int4open, &intersection4);
        }
        locked4 = trainNo;
        // pthread_cond_signal(&int4closed);
        pthread_mutex_unlock(&intersection4);
    }else{
        if(debug==1) printf("Illegal first intersection identifier\n");
        return;
    }
}

// crossLane() is called when the train is crossing the lane, post arrivel/arriveLane()
void crossLane(int firstIntersection, int secondIntersection, int trainNo) {
    if(firstIntersection==1){
        pthread_mutex_lock(&intersection2);
        while(locked2!=0 && deadlockTrainNo!=trainNo){
            pthread_cond_wait(&int2open2, &intersection2);
        }
        if(deadlockTrainNo==trainNo && deadlockIntersectionNo==1){
            pthread_mutex_unlock(&intersection2);
            return;
        }
        locked2 = trainNo;
        pthread_mutex_unlock(&intersection2);
    }else if(firstIntersection==2){
        pthread_mutex_lock(&intersection3);
        while(locked3!=0 && deadlockTrainNo!=trainNo){
            pthread_cond_wait(&int3open2, &intersection3);
        }
        if(deadlockTrainNo==trainNo && deadlockIntersectionNo==2){
            pthread_mutex_unlock(&intersection3);
            return;
        }
        locked3 = trainNo;
        pthread_mutex_unlock(&intersection3);
    }else if(firstIntersection==3){
        pthread_mutex_lock(&intersection4);
        while(locked4!=0 && deadlockTrainNo!=trainNo){
            pthread_cond_wait(&int4open2, &intersection4);
        }
        if(deadlockTrainNo==trainNo && deadlockIntersectionNo==3){
            pthread_mutex_unlock(&intersection4);
            return;
        }
        locked4 = trainNo;
        pthread_mutex_unlock(&intersection4);
    }else if(firstIntersection==4){
        pthread_mutex_lock(&intersection1);
        while(locked1!=0 && deadlockTrainNo!=trainNo){
            pthread_cond_wait(&int1open2, &intersection1);
        }
        if(deadlockTrainNo==trainNo && deadlockIntersectionNo==4){
            pthread_mutex_unlock(&intersection1);
            return;
        }
        locked1 = trainNo;
        pthread_mutex_unlock(&intersection1);
    }else{
        if(debug==1) printf("Illegal first intersection identifier\n");
        return;
    }
    // crossing second intersection (first too, since the train is long)
    usleep(1000 * thread_safe_rng(500, 1000)); // take 500-1000 ms to cross the lane
}

// exitLane() is called when the complete train exits the lane 
void exitLane(int firstIntersection, int secondIntersection, int trainNo) {
    if(firstIntersection==1){
        pthread_mutex_lock(&intersection1);
        pthread_mutex_lock(&intersection2);
        if(deadlockTrainNo==trainNo && deadlockIntersectionNo==1){
            deadlockTrainNo = 0;
            locked1 = 0;
            pthread_cond_signal(&int1open2);
            // pthread_cond_signal(&int2open2);
            pthread_cond_signal(&int1open);
            // pthread_cond_signal(&int2open);
            signalled = 0;
            pthread_mutex_unlock(&intersection2);
            pthread_mutex_unlock(&intersection1);
            return;
        }
        locked1 = 0;
        locked2 = 0;
        pthread_cond_signal(&int1open2);
        pthread_cond_signal(&int2open2);
        pthread_cond_signal(&int1open);
        pthread_cond_signal(&int2open);
        pthread_mutex_unlock(&intersection2);
        pthread_mutex_unlock(&intersection1);
    }else if(firstIntersection==2){
        pthread_mutex_lock(&intersection2);
        pthread_mutex_lock(&intersection3);
        if(deadlockTrainNo==trainNo && deadlockIntersectionNo==2){
            deadlockTrainNo = 0;
            locked2 = 0;
            // pthread_cond_signal(&int2open2);
            // pthread_cond_signal(&int3open2);
            pthread_cond_signal(&int2open2);
            pthread_cond_signal(&int2open);
            // pthread_cond_signal(&int3open);
            signalled = 0;
            pthread_mutex_unlock(&intersection3);
            pthread_mutex_unlock(&intersection2);
            return;
        }
        locked2 = 0;
        locked3 = 0;
        pthread_cond_signal(&int2open2);
        pthread_cond_signal(&int3open2);
        pthread_cond_signal(&int2open);
        pthread_cond_signal(&int3open);
        pthread_mutex_unlock(&intersection3);
        pthread_mutex_unlock(&intersection2);
    }else if(firstIntersection==3){
        pthread_mutex_lock(&intersection3);
        pthread_mutex_lock(&intersection4);
        if(deadlockTrainNo==trainNo && deadlockIntersectionNo==3){
            deadlockTrainNo = 0;
            locked3 = 0;
            pthread_cond_signal(&int3open2);
            // pthread_cond_signal(&int4open2);
            pthread_cond_signal(&int3open);
            // pthread_cond_signal(&int4open);
            signalled = 0;
            pthread_mutex_unlock(&intersection4);
            pthread_mutex_unlock(&intersection3);
            return;
        }
        locked3 = 0;
        locked4 = 0;
        pthread_cond_signal(&int3open2);
        pthread_cond_signal(&int4open2);
        pthread_cond_signal(&int3open);
        pthread_cond_signal(&int4open);
        pthread_mutex_unlock(&intersection4);
        pthread_mutex_unlock(&intersection3);
    }else if(firstIntersection==4){
        pthread_mutex_lock(&intersection4);
        pthread_mutex_lock(&intersection1);
        if(deadlockTrainNo==trainNo && deadlockIntersectionNo==4){
            deadlockTrainNo = 0;
            locked4 = 0;
            pthread_cond_signal(&int4open2);
            // pthread_cond_signal(&int1open2);
            pthread_cond_signal(&int4open);
            // pthread_cond_signal(&int1open);
            signalled = 0;
            pthread_mutex_unlock(&intersection1);
            pthread_mutex_unlock(&intersection4);
            return;
        }
        locked4 = 0;
        locked1 = 0;
        pthread_cond_signal(&int4open2);
        pthread_cond_signal(&int1open2);
        pthread_cond_signal(&int4open);
        pthread_cond_signal(&int1open);
        pthread_mutex_unlock(&intersection1);
        pthread_mutex_unlock(&intersection4);
    }else{
        if(debug==1) printf("Illegal first intersection identifier\n");
        return;
    }
}

// Points to note:
// 1. the train blocks the first intersection of the lane well in advance
// 2. to cross the lane it must block the second intersection of the lane as well
// 3. train is assumed to be very large, safe to unblock the two intersections only
//      after it has completed exited the complete lane i.e. once exitLane() has been called

// Safety: No crash can occur, and only one train can use a lane a time. 
// Liveness: The system is not stuck forever.
// Starvation: No train should be starved from entering the lane.

void *trainThreadFunction(void* arg)
{   
    int firstIntersection = 0;

    myarg_t *args = (myarg_t *) arg;
    char direction = args->trainDir;
    int trainNo = args->trainNo;
    if(debug==1) printf("TTF Function running .. %d %c\n", trainNo, direction);

    int secondIntersection = 0;

    char* trainDir = NULL; // TODO set the direction of the train: North/South/East/West.
    // char* trainDirExit = NULL; // TODO set the direction of the train: North/South/East/West.

    if(direction=='N'){
        firstIntersection = 1;
        secondIntersection = 2;
        trainDir = "North";
        // trainDirExit = "South";
    }else if(direction=='E'){
        firstIntersection = 4;
        secondIntersection = 1; 
        trainDir = "East";
        // trainDirExit = "West";       
    }else if(direction=='S'){
        firstIntersection = 3;
        secondIntersection = 4;
        trainDir = "South";
        // trainDirExit = "North";
    }else if(direction=='W'){
        firstIntersection = 2;
        secondIntersection = 3;
        trainDir = "West";
        // trainDirExit = "East";
    }else{
        printf("Illegal direction\n");
        return NULL;
    }

    // printf("First intersection init'd to %d\n", firstIntersection);
    // printf("Second intersection init'd to %d\n", secondIntersection);

    usleep(thread_safe_rng(0, 10000)); // start at random time


    arriveLane(firstIntersection, trainNo);
    printf("Train Arrived at the lane from the %s direction\n", trainDir);
    if(debug==1) printf("Train %d arrived %s\n", trainNo, trainDir);


    crossLane(firstIntersection, secondIntersection, trainNo);
    printf("Train Exited the lane from the %s direction\n", trainDir);
    if(debug==1) printf("Train %d exitd %s\n", trainNo, trainDir);
    exitLane(firstIntersection, secondIntersection, trainNo);


    return NULL;
}

void *deadLockResolverThreadFunction(void * arg) {
    /* TODO extract arguments from the `void* arg` */
    if(debug==1) printf("Background deadlock detector is running...\n");
    while (1) {
        int deadLockDetected = 0; 
        if(locked1!=locked2 && locked2!=locked3 && locked3!=locked4 && locked4!=locked1 && locked1!=locked3 && locked2!=locked4 && locked1!=0 && locked2!=0 && locked3!=0 && locked4!=0) {
            if(debug==1) printf("Deadlock detected.\n");
            deadLockDetected = 1;
        }
        if (deadLockDetected) {
            pthread_mutex_lock(&deadlockResolve);
            if(signalled==0) printf("Deadlock detected. Resolving deadlock...\n");
            if(debug==1) printf("I1 %d I2 %d I3 %d I4 %d\n", locked1, locked2, locked3, locked4);
            // Train T1 coming from North, waiting at intersection 1
            // Train T2 coming from West, waiting at intersection 2
            // Train T3 coming from South, waiting at intersection 3
            // Train T4 coming from East, waiting at intersection 4
            // To clear deadlock, let Train T2 pass through intersection 3
            if(signalled==0){
                deadlockIntersectionNo = (deadlockIntersectionNo+1)%4 + 1;
                if(deadlockIntersectionNo==1){
                    deadlockTrainNo = locked1;
                }else if(deadlockIntersectionNo==2){
                    deadlockTrainNo = locked2;
                }else if(deadlockIntersectionNo==3){
                    deadlockTrainNo = locked3;
                }else if(deadlockIntersectionNo==4){
                    deadlockTrainNo = locked4;
                }else{
                    if(debug==1) printf("invalid deadlockIntersectionNo \n");
                    // control shouldnever reach here
                    return NULL;
                }
            }
            if(debug==1) printf("deadlockIntersectionNo %d trainNo %d\n", deadlockIntersectionNo, deadlockTrainNo);
            deadLockDetected = 0;
            signalled = 1;
            if(deadlockIntersectionNo==1){
                pthread_cond_signal(&int2open2);
            }else if(deadlockIntersectionNo==2){
                pthread_cond_signal(&int3open2);
            }else if(deadlockIntersectionNo==3){
                pthread_cond_signal(&int4open2);
            }else if(deadlockIntersectionNo==4){
                pthread_cond_signal(&int1open2);
            }else{
                if(debug==1) printf("invalid deadlockIntersectionNo \n");
                // control shouldnever reach here
                return NULL;
            }
            if(debug==1) printf("Signalled \n");
            pthread_mutex_unlock(&deadlockResolve);
        }

        usleep(1000 * 500); // sleep for 500 ms
    }
    return NULL;
}

int main(int argc, char *argv[]) {

    srand(time(NULL));

    if (argc != 2) {
        printf("Usage: ./main <train dirs: [NSWE]+>\n");
        return 1;
    }

    pthread_mutex_init(&rng_mutex, NULL);

    pthread_t dLRTFThread;
    pthread_create(&dLRTFThread, NULL, deadLockResolverThreadFunction, NULL);

    char* train = argv[1];
    // printf("%d\n",strlen(train));
    // printf("%s\n", train);

    int num_trains = 0;
    int total_num_trains = strlen(train);
    pthread_t train_threads[total_num_trains];
    myarg_t argArray[total_num_trains];

    for(int i=0; i<total_num_trains; i++){
        argArray[i].trainNo = i+1;
        argArray[i].trainDir = train[i];
    }

    while (train[num_trains] != '\0') {
        char trainDir = train[num_trains];

        if (trainDir != 'N' && trainDir != 'S' && trainDir != 'E' && trainDir != 'W') {
            printf("Invalid train direction: %c\n", trainDir);
            printf("Usage: ./main <train dirs: [NSEW]+>\n");
            return 1;
        }
        
        if(debug==1) printf("trainDir %c no %d %d\n", argArray[num_trains].trainDir, argArray[num_trains].trainNo, num_trains);
        pthread_create(&train_threads[num_trains], NULL, trainThreadFunction, &argArray[num_trains]);
        if(debug==1) printf("trainDir %c no %d %d\n", argArray[num_trains].trainDir, argArray[num_trains].trainNo, num_trains);
        num_trains = num_trains + 1;
    }

    for(int i=0; i<num_trains; i++){
        if(debug==1) printf("Joining... \n");
        void* buf;
        pthread_join(train_threads[i], &buf);
    }

    return 0;
}