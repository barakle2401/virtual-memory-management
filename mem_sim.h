/*
 * Barak Levy 
 * 203280185
 
 * 
 */
#ifndef MEM_SIM_H
#define MEM_SIM_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <math.h>
#include <signal.h>
#include <fcntl.h>
#include <bits/fcntl-linux.h>
#define PAGE_SIZE 5
#define NUM_OF_PAGES 25
#define MEMORY_SIZE 20
#define SWAP_SIZE 100

#define TEXT 0
#define DATA_BSS 1
#define HEAP_STACK_SIZE 2
typedef struct page_descriptor
{
unsigned int V; // valid
unsigned int D; // dirty
unsigned int P; // permission
unsigned int frame; //the number of a frame if in case it is page-mapped
}page_descriptor;
struct sim_database
{
page_descriptor page_table[NUM_OF_PAGES]; //pointer to page table
int swapfile_fd; //swap file fd
int program_fd; //executable file fd
char main_memory[MEMORY_SIZE];
int text_size;
int data_bss_size;
int heap_stack_size;
};
typedef struct fifo
{
    int frame;
    struct fifo* next;
}fifo;
void print_free_frames();
void update_page_table(struct sim_database* sim_db,int page,int v,int d ,int frame);
int locate_free_frame(struct sim_database* sim_db);
char load(struct sim_database* sim_db,int address);
struct sim_database * init_system(char exe_file_name[],char swap_file_name[] ,int text_size, int data_bss_size,int
heap_stack_size );
void print_swap(struct sim_database* sim_db);
void print_page_table(struct sim_database* sim_db);
void print_memory(struct sim_database* sim_db);
bool ram_full(struct sim_database* sim_db);
void add_to_fifo();
int remove_from_fifo();
void printfifo();
void print_page_table(struct sim_database* mem_sim);
void print_exe_file(int);
void page_to_frame(struct sim_database* sim_db ,int page,int frame);
int  page_belong(struct sim_database* sim_db,int);
char handle_mem(struct sim_database* sim_db,int address);
void source_to_frame(struct sim_database* sim_db,int page,int frame,int fd);
void frame_to_swap(struct sim_database*sim_db,int page,int frame);

int  init_new_page(struct sim_database* sim_db,int free_frame,int page);
void store(struct sim_database* sim_db,int address,char value);
void clear_system(struct sim_database *sim_db);
#endif /* MEM_SIM_H */

