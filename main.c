//
//  main.c
//  OS Assignment 2
//
//  Created by Beng Han Ng on 30/11/2015.
//  Copyright © 2015 Beng Han Ng. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define ARRAY_SIZE 16
#define MAX_SIZE 1024

typedef struct circular_buffer{
    void *buffer;
    void *buffer_end;
    void *start;
    void *end;
    int capacity;
    int counter;
    int size;
} circular_buffer;

void init_cb(circular_buffer *cb, int capacity, int size);
void init_cb(circular_buffer *cb, int capacity, int size);
void push_cb(circular_buffer *cb, const void *message);
void pop_cb(circular_buffer *cb, char *message);
void free_cb();
void *read_msg();
void *write_msg();

FILE *fp_read;
FILE *fp_write;
circular_buffer shared_buffer;
pthread_mutex_t thread_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t buffer_lock = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char ** argv) {

    char *output_file = "output.txt";

    pthread_t read_thr1, read_thr2, read_thr3, write_thr1, write_thr2, write_thr3;
    int read1, read2, read3, write1, write2, write3;
    
    init_cb(&shared_buffer, ARRAY_SIZE, MAX_SIZE);
    
    fp_read = fopen(argv[1], "r");
    
    if(fp_read == NULL){
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }
    
    fp_write = fopen(output_file, "w");
    
    if(fp_write == NULL){
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }
    
    if(pthread_mutex_init(&thread_lock, NULL) != 0){
        printf("Mutex unable to be initialized\n");
        return 1;
    }
    
    if(pthread_mutex_init(&buffer_lock, NULL) != 0){
        printf("Mutex unable to be initialized\n");
        return 1;
    }
    
    read1 = pthread_create(&read_thr1, NULL, &read_msg, NULL);
    write1 = pthread_create(&write_thr1, NULL, &write_msg, NULL);
    read2 = pthread_create(&read_thr2, NULL, &read_msg, NULL);
    write2 = pthread_create(&write_thr2, NULL, &write_msg, NULL);
    read3 = pthread_create(&read_thr3, NULL, &read_msg, NULL);
    write3 = pthread_create(&write_thr3, NULL, &write_msg, NULL);
    
    if(read1 != 0 || read2 != 0 || read3 != 0 || write1 != 0 || write2 != 0 || write3 != 0){
        printf("Thread failed to be created.");
        return 0;
    }
    
    if(read1 == 0){
        printf("--Read Thread 1 started to run\n");
    }
    if(read2 == 0){
        printf("--Read Thread 2 started to run\n");
    }
    if(read3 == 0){
        printf("--Read Thread 3 started to run\n");
    }
    if(write1 == 0){
        printf("--Write Thread 1 started to run\n");
    }
    if(write2 == 0){
        printf("--Write Thread 2 started to run\n");
    }
    if(write3 == 0){
        printf("--Write Thread 3 started to run\n");
    }

    pthread_join(read_thr1, NULL);
    pthread_join(write_thr1, NULL);
    pthread_join(read_thr2, NULL);
    pthread_join(write_thr2, NULL);
    pthread_join(read_thr3, NULL);
    pthread_join(write_thr3, NULL);
    
    pthread_mutex_destroy(&buffer_lock);
    pthread_mutex_destroy(&thread_lock);
    
    fclose(fp_write);
    fclose(fp_read);
    
    return 0;
}


void init_cb(circular_buffer *cb, int capacity, int size){
    cb -> buffer = (char *) malloc(capacity * size);
    cb -> buffer_end = (char *) cb -> buffer + capacity * size;
    cb -> capacity = capacity;
    cb -> counter = 0;
    cb -> size = size;
    cb -> start = cb -> buffer;
    cb -> end = cb -> buffer;
}

void push_cb(circular_buffer *cb, const void *message){
    memcpy(cb -> end, message, cb -> size);
    cb -> end = (char *) cb -> end + cb -> size;
    cb -> counter ++;
    printf("Message is recorded into the shared buffer: %s", message);
    if(cb -> end == cb -> buffer_end){
        cb -> end = cb -> buffer;
    }
}

void pop_cb(circular_buffer *cb, char *message){
    if(cb -> counter == 0){
        printf("Buffer Underflow.\n");
        return;
    }
    memcpy(message, cb -> start, cb -> size);
    cb -> start = (char *) cb -> start + cb -> size;
    cb -> counter --;
    printf("Message poped out: %s", message);
    if(cb -> start == cb -> buffer_end){
        cb -> start = cb -> buffer;
    }
}

void free_cb(){
    free(shared_buffer.buffer);
}

void *read_msg(){
    char message[MAX_SIZE];
    printf("Thread id:%d\n", (int)pthread_self());
    pthread_mutex_lock(&thread_lock);
    while(fgets(message, MAX_SIZE, fp_read) != NULL){
        pthread_mutex_lock(&buffer_lock);
        push_cb(&shared_buffer, message);
        pthread_mutex_unlock(&buffer_lock);
    }
    pthread_mutex_unlock(&thread_lock);
    
    return 0;
}

void *write_msg(){
    char message[MAX_SIZE];
    printf("Thread id:%d\n", (int)pthread_self());
    pthread_mutex_lock(&thread_lock);
    while(shared_buffer.counter > 0){
        pthread_mutex_lock(&buffer_lock);
        pop_cb(&shared_buffer, message);
        fprintf(fp_write, "%s", message);
        pthread_mutex_unlock(&buffer_lock);
    }
    pthread_mutex_unlock(&thread_lock);
    
    return 0;
}


