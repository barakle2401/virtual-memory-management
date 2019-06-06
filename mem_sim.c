/*
 * Barak Levy 
 * 203280185
 * Ex5 - virtual memory 
 * program objective: simulate demand paging method to management memory system 
 * only the demand pages of the process will store into the ram 
 * pages that has been change will be store into the swap file   
 */
#include "mem_sim.h"
int free_frames[MEMORY_SIZE/PAGE_SIZE];
fifo * head;//queue
//load the address to the main mem 
char load(struct sim_database* sim_db,int address)
{
    int physical_address=0;
    int offset = address%PAGE_SIZE;
    int page = address/PAGE_SIZE;
    int actual_size = sim_db->data_bss_size+sim_db->text_size+sim_db->heap_stack_size;
    if(address>=actual_size||address<0)//case out of bounds
    {
        
        return '\0';
    }
    if(sim_db->page_table[page].V==1)//case the page already in the ram 
    {
        physical_address= sim_db->page_table[page].frame*PAGE_SIZE+offset;
        return sim_db->main_memory[physical_address];
    }
    if(!ram_full(sim_db))
    {
        int frame = locate_free_frame(sim_db);//locate the free frame
        if(sim_db->page_table[page].P==0)
        {
           
            source_to_frame(sim_db,page,frame,sim_db->program_fd); //copy from exe to memory 
            update_page_table(sim_db,page,1,0,frame);
         
            //physical_address = (sim_db->page_table[page].frame*PAGE_SIZE)+offset;
            //return sim_db->main_memory[physical_address];
        }
        else
        {//p==1
            if(sim_db->page_table[page].D==0)
            {
                int where = page_belong(sim_db,address);
                
                if(where==DATA_BSS)
                {
                    source_to_frame(sim_db,page,frame,sim_db->program_fd); //copy from exe to memory 
                   
                }
                
                else if(where==HEAP_STACK_SIZE)//new page 
                {   
                   
                  init_new_page(sim_db,frame,page);
                 
                }
                update_page_table(sim_db,page,1,0,frame); 
            }
            else
            {//D==1
                source_to_frame(sim_db,page,frame,sim_db->swapfile_fd);
                update_page_table(sim_db,page,1,1,frame);
                
            }
        }  
    }
    else//ram is full
    {
        int frame = remove_from_fifo();
        int page_to_remove = free_frames[frame];
        if(sim_db->page_table[page_to_remove].D==1)
        {//copy from the pyshical mem to the swap file
            frame_to_swap(sim_db,page_to_remove,frame);
            update_page_table(sim_db,page_to_remove,0,1,-1);
        }
        else
        {
             update_page_table(sim_db,page_to_remove,0,0,-1);
        }
        free_frames[frame] = -1;
        load(sim_db,address);
        
        
    }//return the value
    physical_address= sim_db->page_table[page].frame*PAGE_SIZE+offset;
    return sim_db->main_memory[physical_address];
}

void frame_to_swap(struct sim_database*sim_db,int page,int frame)
{
    char str[PAGE_SIZE];
    int j =0;
    lseek(sim_db->swapfile_fd,page*PAGE_SIZE,SEEK_SET);//go to the start of the file
    
    
    for(int i=frame*PAGE_SIZE;i<(frame*PAGE_SIZE)+PAGE_SIZE;i++)//store the data into the right frame
    {   
        strncpy(&str[j++],&sim_db->main_memory[i],1);
    }  
    int status = write(sim_db->swapfile_fd,str,PAGE_SIZE);
    if(status!=PAGE_SIZE)
    {
        perror("cannot write\n");
        return;
    } 
    
}
void source_to_frame(struct sim_database* sim_db ,int page,int frame,int source_fd)
{
    char str[PAGE_SIZE];
    int j =0;
    lseek(source_fd,page*PAGE_SIZE,SEEK_SET);//go to the start of the file
    int status = read(source_fd,str,PAGE_SIZE);
    if(status!=PAGE_SIZE)
    {
       
        perror("cannot read\n");
        return;
    }
    
    for(int i=frame*PAGE_SIZE;i<(frame*PAGE_SIZE)+PAGE_SIZE;i++)//store the data into the right frame
    {          
        strncpy(&sim_db->main_memory[i],&str[j++],1);//copy the chars to the main memory
    }
   
    free_frames[frame] = page;
    add_to_fifo(frame);
        
}
void store(struct sim_database* sim_db,int address,char value)
{
    int actua_size = sim_db->text_size+sim_db->data_bss_size+sim_db->heap_stack_size;
    int where = page_belong(sim_db,address);
    if(where==TEXT)
        return;
    if(address>=actua_size|address<0)//case out of bounds
    {   
        return;
    }
    int physical_address=0;
    int offset = address%PAGE_SIZE;
    int page = address/PAGE_SIZE;
    if(sim_db->page_table[page].V==1)
    {
        physical_address = sim_db->page_table[page].frame*PAGE_SIZE+offset;
        sim_db->main_memory[physical_address] = value;
        update_page_table(sim_db,page,1,1,sim_db->page_table[page].frame);
        return;
    }
    if(!ram_full(sim_db))
    {     
        //p==1
        int frame = locate_free_frame(sim_db);
        if(sim_db->page_table[page].D==0)
        {
            int where = page_belong(sim_db,address);
           
            if(where==DATA_BSS)
            {
               
                source_to_frame(sim_db,page,frame,sim_db->program_fd);//copy from exe to memory 
                
            }
            else//new page 
            {   
                
                init_new_page(sim_db,frame,page);
             
            }
        }
        else
        {//D==1
            source_to_frame(sim_db,page,frame,sim_db->swapfile_fd);
            
          
        }
        update_page_table(sim_db,page,1,1,frame);
        physical_address = sim_db->page_table[page].frame*PAGE_SIZE+offset;
        sim_db->main_memory[physical_address] = value;
        return;
    }
    else//ram is full
    {
        int frame = remove_from_fifo();//evacuate a cell for the the new page
        int page_to_remove = free_frames[frame];//find the page that has been extracted
      
        if(sim_db->page_table[page_to_remove].D==1)//if the page to remove has been changed, store in swap
        {
            frame_to_swap(sim_db,page_to_remove,frame);//move data to swap
        }
        update_page_table(sim_db,page_to_remove,0,1,-1);
        free_frames[frame] = -1;
        store(sim_db,address,value);
          
    }  
}
void update_page_table(struct sim_database* sim_db,int page,int v,int d ,int frame)
{
    sim_db->page_table[page].V=v;
    sim_db->page_table[page].D=d;
    sim_db->page_table[page].frame=frame;
    
}
int locate_free_frame(struct sim_database* sim_db)
{
   
    for(int i = 0;i<MEMORY_SIZE/PAGE_SIZE;i++)
    {
        if(free_frames[i]==-1)
            return i;
    }
    printf("could'nt loacte frame\n");
    return -1;
}
struct sim_database * init_system(char exe_file_name[],char swap_file_name[] ,int text_size, int data_bss_size,int
heap_stack_size) 
{
    //define the sizes
    int swap,exe;
    int actual_text_size = text_size/PAGE_SIZE;
    int actual_data_size = data_bss_size/PAGE_SIZE;
    int actual_heapstack_size = heap_stack_size/PAGE_SIZE;
    if((text_size%PAGE_SIZE)!=0)
        actual_text_size+=1;
    if((data_bss_size%PAGE_SIZE)!=0)
        actual_data_size+=1;
    if((heap_stack_size%PAGE_SIZE)!=0)
        actual_heapstack_size+=1;
    for(int i=0;i<MEMORY_SIZE/PAGE_SIZE;i++)
        free_frames[i] = -1;
    struct sim_database*  sim_db;
    sim_db = (struct sim_database*) malloc(sizeof(struct sim_database)); 
    exe =  open(exe_file_name,O_RDONLY,777);
   
    if(exe<0)
    {
        return NULL;
    }//open/create swap file
    swap = open(swap_file_name,O_CREAT|O_TRUNC|O_RDWR,S_IRUSR|S_IWUSR);
    if(swap<0)
    {
        printf("ERROR IN OPEN FILE\n");
    }
    sim_db->program_fd = exe;
    sim_db->swapfile_fd = swap;
    sim_db->text_size = text_size;
    sim_db->data_bss_size=data_bss_size;
    sim_db->heap_stack_size = heap_stack_size;
    //initialize the main memory
    for(int i=0;i<MEMORY_SIZE;i++)
    {
        sim_db->main_memory[i] = '0';
    }
   // update the page table bits
    int total_size = actual_data_size+actual_heapstack_size+actual_text_size;
    for(int i =0;i<total_size;i++)
    {
        sim_db->page_table[i].D=0;
        sim_db->page_table[i].V=0;
        sim_db->page_table[i].frame = -1;
        if(i<actual_text_size)
        {
            sim_db->page_table[i].P = 0;//5
            
        }
        else if (i<(actual_data_size+actual_heapstack_size+actual_text_size))
        {
              
            sim_db->page_table[i].P = 1;
        }
       
    }
   
    return sim_db;
}//insert to the end of the queue
void add_to_fifo(int frame)
{
    if(head==NULL)
    {
        head = (fifo*) malloc(sizeof(fifo));
        head->frame = frame;
        head->next = NULL;
        return;
    }
    fifo * new = (fifo*)malloc(sizeof(fifo));
    fifo *temp=head;
    while(temp->next!=NULL)
    {
        temp = temp->next;
    }
    temp->next = new;
    new->frame = frame;
    new->next = NULL;
    
}
int remove_from_fifo()
{
    int frame = head->frame;
    fifo * temp = head;
    head = head->next;
    free(temp);
    return frame;
}//check is the ram is full 
bool ram_full(struct sim_database* sim_db)
{

    for(int i =0;i<MEMORY_SIZE/PAGE_SIZE;i++)
    {
        if(free_frames[i]==-1)
            return false;
            
    }
    return true;
}// init the system
int init_new_page(struct sim_database* sim_db,int free_frame,int page)
{
    
    for(int i =free_frame*PAGE_SIZE;i<((free_frame*PAGE_SIZE)+PAGE_SIZE);i++)
    {
        sim_db->main_memory[i] = '\0';
    }
    free_frames[free_frame] = page;
    add_to_fifo(free_frame);
    
}
void print_swap(struct sim_database* sim_db)
{
    char str[PAGE_SIZE];
    int i;
    printf("\nSwap memory\n");
    lseek(sim_db->swapfile_fd,0,SEEK_SET);//go to the start of the file
    while(read(sim_db->swapfile_fd,str,PAGE_SIZE))
    {
        for(i=0;i<PAGE_SIZE;i++)
        {
            printf("[%c]\t",str[i]);
        }
        printf("\n");
    }
    
}
void print_memory(struct sim_database* sim_db)
{
    int i=0;
    printf("\n Physical memory\n");
    for(int i=0;i<MEMORY_SIZE;i++)
    {
        printf("[%c]\n",sim_db->main_memory[i]);
    }
}
void print_page_table(struct sim_database* sim_db)
{
    int i;
    printf("\nPage table \n");
    printf("Valid\tDirty\tPermission\tFrame\n");
    for(int i=0;i<NUM_OF_PAGES;i++)
    {
        printf("[%d]\t[%d]\t[%d]\t        [%d]\n",sim_db->page_table[i].V,sim_db->page_table[i].D,sim_db->page_table[i].P,sim_db->page_table[i].frame);
        
    }
}
void print_free_frames()
{
    printf("\nFREE FRAMES:\n");
    for(int i =0;i<MEMORY_SIZE/PAGE_SIZE;i++)
    {
        printf("[%d]\n",free_frames[i]);
    }
}
void printfifo()
{
    printf("\nFIFO QUEUE\n");
    fifo * temp = head;
    printf("\n");
   
    while(temp!=NULL)
    {
        
        printf("%d-->",temp->frame);
        temp = temp->next;
    }
    printf("\n");
}
int page_belong(struct sim_database* mem_sim,int slot)
{
       
	int text_start = 0;
	int text_end = mem_sim->text_size-1;
	int data_bss_start = mem_sim->text_size;
	int data_bss_end = data_bss_start + mem_sim->data_bss_size-1;
	int heap_stack_start = mem_sim->text_size+mem_sim->data_bss_size;
	int heap_stack_end = heap_stack_start + mem_sim->heap_stack_size-1;


	
	if(slot < text_start || slot > heap_stack_end)
		return -1;

	if(slot >= text_start && slot <= text_end)
            return TEXT; 
		
          

	if(slot >= data_bss_start && slot <= data_bss_end)
            return DATA_BSS;
           

	if(slot >= heap_stack_start && slot <= heap_stack_end)
            return HEAP_STACK_SIZE;
           


	

}
void clear_system(struct sim_database *sim_db)
{
    if(!sim_db->program_fd)
    {
       close(sim_db->program_fd); 
    }
    if(!sim_db->swapfile_fd)
    {
       close(sim_db->swapfile_fd); 
    }
    free(sim_db);
    fifo* temp = NULL;
    if(head->next==NULL)
    {
        free(head);
        return;
    }
    while(head!=NULL)
    {
        temp = head->next;
        free(head);
        head = temp;
    }
    
}