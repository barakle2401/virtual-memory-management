# virtual-memory-management
simulate virtual memory by management technique and demand by paging method - 
to management memory system 
only the demanded page of the process will store into the ram 
pages that has been changed will be store into the swap file     
## program flow chart  :

![alt text](https://github.com/barakle2401/virtual-memory-management/blob/master/memorySimulate.PNG)



This program simulates access to the physical memory while using paging mechanism

how to install the program: open linux terminal, navigate to the folder containing ex5 using the "cd" command (confirm it by using ls command) incase you have makefile, type make and the program will automaticily be compiled, if you don't, type gcc -Wall mem_sim.c main.c -o ex5

and your program will automaticily be compiled

to activate program: open linux terminal, navigate to ex5 executeable file location using "cd" command (confirm it using ls command) and type ./ex5

there are 2 structs - sim_database, page_descriptor, array of free frame in physical address and frame counter

------------------program functions -------------------------------

incase of memory allocation failure, program will exit with message "Memory allocation failed"
struct sim_database* init_system(char exe_file_name[], char swap_file_name[] , int text_size, int data_bss_size, int heap_stack_size); input: the names of execute and swap files, the number of text, data bss and heap_stack in the simulator output: returns a pointer to sim_database

char load(struct sim_database* mem_sim , int address); input: sim_database and logical address output: loads the value from the physical memory

void store(struct sim_database* mem_sim , int address, char value); input: sim_database, logical address, value to store in main memory output: stores the value in the physical memory

void print_memory(struct sim_database* mem_sim); input: sim_database output: prints the main memory

void print_swap (struct sim_database* mem_sim); input: sim_database output: prints the swap file

void print_page_table(struct sim_database* mem_sim); input: sim_database output: prints the page_descriptor table

void clear_system(struct sim_database* mem_sim); input: sim_database output: closes the fds and frees the pointer

int is_available(int next); input: integer between 0 and MEMORY_SIZE / PAGE_SIZE - 1 output: return -1 if there is no available place in the free_frame array

char* get_from_exec(struct sim_database* mem_sim, int page); input: sim_database, page number output: return the whole page from execute file

char* get_from_swap(struct sim_database* mem_sim, int page); input: sim_database, page number output: return the whole page from swap file

void to_swap(struct sim_database* mem_sim, int page); input: sim_database, page number output: moving the wanted page the swap file

char giveback_val(struct sim_database* mem_sim, int offset, int page); input: sim_database, offset, page output: returns the value in main_memory[page*PAGE_SIZE + offset]

void replace_val(struct sim_database* mem_sim, int offset, int page, char value); input: sim_database, offset, page, value to replace output: replace the value in main_memory[page*PAGE_SIZE + offset]

void insert_to_mem(struct sim_database* mem_sim, char* copy, int page); input: sim_database, copy, page output: inserts the copy to the page that been given

void error(char* msg); input: char[] - error messege output: prints in stderr format the error

int main() command: input: none output: This program simulates access to the physical memory while using paging mechanism that has been fully tested with valgrind to test for memory leak, no leak was found.
