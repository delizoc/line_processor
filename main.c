
/*
Author: Colette DeLizo
Date: 16NOV2021
Project 4 CS 344
Title: Multi-threaded Producer Consumer Pipeline

A program with a pipeline of 4 threads that interact with each other as producers and consumers.
- Input thread is the first thread in the pipeline. It gets input from the user or fileand puts it in a buffer it shares with the next thread in the pipeline.
- Line separator thread is the second thread in the pipeline. It consumes items from the buffer it shares with the input thread. It removes the line separator '\n' from 
  the value and replaces it with a space, and places value in a buffer it shares with the next thread in the pipeline. Thus this thread implements both consumer and producer functionalities.
- Plus sign thread is the third thread in the pipeline. It consumes items from the buffer it shares with the line separator thread. It finds '++' in the item and replaces with '^', 
  and places value in a buffer it shares with the next thread in the pipeline. Thus this thread implements both consumer and producer functionalities.
- Output thread is the fourth thread in the pipeline. It consumes items from the buffer it shares with the plus sign thread and prints the items (only if 80 characters per line)
Source code for producer consumer threads adapted from https://replit.com/@cs344/65prodconspipelinec (example CS344 code for this assignment)
*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>


// Number of character in a line
#define LINE 80

// full_line is used to break out of initial while loop, indicates we have a full line of text to return (80 characters);
int full_line = 1;
// Buffer 1, shared resource between input thread and line_seperator thread
char buffer_1[50][1000];
// Number of items in the buffer
int count_1 = 0;

int check = 0;
int glob = 1;
// Index where the input thread will put the next item
int prod_idx_1 = 0;
// Index where the line_processer thread will pick up the next item
int con_idx_1 = 0;
// Initialize the mutex for buffer 1
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 1
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;


// Buffer 2, shared resource between line_separator thread and plus sign thread
char buffer_2[50][1000];
// Number of items in the buffer
int count_2 = 0;
// Index where the line_separator thread will put the next item
int prod_idx_2 = 0;
// Index where the plus sign thread will pick up the next item
int con_idx_2 = 0;
// Initialize the mutex for buffer 2
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 2
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;

// Buffer 3, shared resource between plus sign thread and output thread
char buffer_3[625][81];
// Number of items in the buffer
int count_3 = 0;
// Index where the plus sign thread will put the next item
int prod_idx_3 = 0;
// Index where the output thread will pick up the next item
int con_idx_3 = 0;
//Index where the plus sign thread will put the next character 
int char_idx = 0;
// Initialize the mutex for buffer 2
pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 2
pthread_cond_t full_3 = PTHREAD_COND_INITIALIZER;



/*
 Put an item in buff_1
*/
void put_buff_1(char *item){
  // Lock the mutex before putting the item in the buffer
  pthread_mutex_lock(&mutex_1);
  // Put the item in the buffer
  strcpy(buffer_1[prod_idx_1], item);
  // Increment the index where the next item will be put.
  prod_idx_1 ++;
  count_1++;
  //Signal to the consumer that the buffer is no longer empty
  pthread_cond_signal(&full_1);
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_1);
  return;
}

/*
 Function that the input thread will run.
 Get input from the user.
 Put the item in the buffer shared with the separator thread.
*/
void *get_input(void *args)
{
  // Get the user input
  char user_input[50000];
  fgets(user_input, 1000, stdin);
  fflush(stdin);
  put_buff_1(user_input);
  return;
}

/*
Get the next item from buffer 1
*/
int get_buff_1(){
  // Lock the mutex before checking if the buffer has data
  pthread_mutex_lock(&mutex_1);
  while (count_1 == 0)
    // Buffer is empty. Wait for the producer to signal that the buffer has data
    pthread_cond_wait(&full_1, &mutex_1);
  char *item = buffer_1[con_idx_1];
  // Increment the index from which the item will be picked up
  con_idx_1 ++;
  count_1--;
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_1);
  // Return the item
  return item;
}

/*
 Put an item in buff_2
*/
void put_buff_2(char *item){
  // Lock the mutex before putting the item in the buffer
  pthread_mutex_lock(&mutex_2);
  // Put the item in the buffer
  strcpy(buffer_2[prod_idx_2],item);
  // Increment the index where the next item will be put.
  prod_idx_2++;
  count_2++;
  // Signal to the consumer that the buffer is no longer empty
  pthread_cond_signal(&full_2);
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_2);
  return;
} 

/*
 Put an item in buff_3
*/
void put_buff_3(char *item){
  // Lock the mutex before putting the item in the buffer
  pthread_mutex_lock(&mutex_3);
  // Put the item in the buffer
  int i =0;
  while(i < strlen(item)){
    //first check to make sure STOP condition is not met, if met exit program
    if (item[0]=='S'&&item[1]=='T'&&item[2]=='O'&&item[3]=='P'&&item[4]==' '&& !item[5]){
      exit(0);
    }
    //put item character in buffer
    buffer_3[prod_idx_3][char_idx] = item[i];
    char_idx++;
    if (char_idx == LINE){
      buffer_3[prod_idx_3][char_idx]= '\0';//add end of line character to indicate new line
      char_idx = 0;                        //reset char_idx to begining of line
      prod_idx_3++;                        //increment to next line in buffer
    }i++;
  }
  
  // Signal to the consumer that the buffer is no longer empty if we have a full line to print
  if (prod_idx_3 > check){
    count_3++;
    glob = 1;
    check++;
    full_line = 0;
    pthread_cond_signal(&full_3);
    pthread_mutex_unlock(&mutex_3);
  }
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_3);
  return;
} 

/*
Get the next item from buffer 2
*/
int get_buff_2(){
  // Lock the mutex before checking if the buffer has data
  pthread_mutex_lock(&mutex_2);
  while (count_2 == 0)
    // Buffer is empty. Wait for the producer to signal that the buffer has data
    pthread_cond_wait(&full_2, &mutex_2);
  char *item = buffer_2[con_idx_2];
  // Increment the index from which the item will be picked up
  con_idx_2++;
  count_2--;
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_2);
  // Return the item
  return item;
}

/*
Get the next item from buffer 2
*/
int get_buff_3(){
  // Lock the mutex before checking if the buffer has data
  pthread_mutex_lock(&mutex_3);
  while (count_3 == 0)
    // Buffer is empty. Wait for the producer to signal that the buffer has data
    pthread_cond_wait(&full_3, &mutex_3);
  char *item = buffer_3[con_idx_3];
  // Increment the index from which the item will be picked up
  con_idx_3 ++;
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_3);
  // Return the item
  return item;
}

/*
 Function that the line_separator thread will run. 
 Consume an item from the buffer shared with the input thread.
 put the upated item in buff_2
*/
void *line_separator(void *args){
  char *item;           
  item = get_buff_1();        //get line from buffer_1
  item[strlen(item) - 1] = ' '; //replace the '/n with a space'
  put_buff_2(item);           //put item into buff_2 
  return;
}
/*
 Function that the plus_sign thread will run. 
 Consume an item from the buffer shared with the line_separator thread.
 put the upated item in buff_3
*/
void *plus_sign(void *args){
  int i;    
  char *orig;  
  const char *rep = '++';
  const char *with = '^';
    orig = get_buff_2();        //get line from buffer_2 
    for (i = 0; i < strlen(orig); ++i) {        //if ++ present in item replace with ^
      if (orig[i] == '+' && orig[i + 1]== '+') {
        orig[i] = '^';
        int idxToDel = i+1;
        memmove(&orig[idxToDel], &orig[idxToDel + 1], strlen(orig) - idxToDel);   // source: https://stackoverflow.com/questions/5457608/how-to-remove-the-character-at-a-given-index-from-a-string-in-c
      }
    }
    put_buff_3(orig); //put in buff_3
    return;
}

/*
 Function that the out_put thread will run. 
 Consume an item from the buffer shared with the plus_sign thread.
 print the item;
*/
void *write_output(void *args){       
  int i;
  int j = 0;
  char *item;  
  for ( i = con_idx_3; i < prod_idx_3; i++){
    item = get_buff_3();        //get line from buffer_1
    printf("%s\n", item);
    fflush(stdout);
  }
  full_line = 1;  //reset initial input, line_separator and plus_sign thread loop to get user input again 
  glob = 1;
  return 0;
}

int main()
{   // Create the threads
    do {
      do{
        pthread_t input_t, separator_t, plus_t;
        pthread_create(&input_t, NULL, get_input, NULL);
        pthread_create(&separator_t, NULL, line_separator, NULL);
        pthread_create(&plus_t, NULL, plus_sign, NULL);
        // Wait for the threads to terminate
        pthread_join(input_t, NULL);
        pthread_join(separator_t, NULL);
        pthread_join(plus_t, NULL);
      }while(full_line);
      pthread_t output_t;
      // Create the threads
      pthread_create(&output_t, NULL, write_output, NULL);
      // Wait for the threads to terminate
      pthread_join(output_t, NULL);
    }while(glob);
  return EXIT_SUCCESS;
}