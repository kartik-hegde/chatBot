
#include"util.hpp"
#include "inttypes.h"

#define DELAY_FACTOR 10000000
#define DELAY_FACTOR_SHORT 100

#define MEM_RW(x)   	(*(volatile unsigned long *)(x))

#define SET_COUNT 	8
#define SETS 		64
#define SET_SIZE	(0x1 << 3)

#define NUM_TESTS	10
#define THRESHOLD	1000
#define JUNK		0xDEADBEEF

inline void clflush(volatile void *p)
            {
                asm volatile ("clflush (%0)" :: "r"(p));
            }
int delay()
{
	int i;
	int j = 0;

	for(i=0; i<DELAY_FACTOR; i++)
		j++; 
	
}

int delay_short()
{
	int i;
	int j = 0;

	for(i=0; i<DELAY_FACTOR_SHORT; i++)
		j++; 
}
	
int find_max(int score_array[], int size)
{
	int i;
	int max_value=0, max_index=0;

	for(i=0; i<size; i++)
	{
		if(score_array[i] > max_value)
		{	
			max_value = score_array[i];
			max_index = i;
		}
	}

	return max_index;
}	

void print_addr(long int a)
{
	int block = (a & 0x3f);
	int set = (a & 0xfC0)>>6;
	int tag = (a & 0xfff000)>>12;

	printf("Addr: %lx, block: %x , Tag: %x, Set: %x\n", a, block, tag, set);

}

int main(int argc, char **argv)
{
        //Prepare to find out the cache-line mapped:
	//Strategy: Flush each cache-line multiple times and figure out the delay

   	//L1 Data cache = 32 KB, 64 B/line, 8-WAY.
	// That gives us 6bits of Block address, 6 bits of set address
	//L1 Data Cache Latency = 4 cycles for simple access via pointer
	//RAM Latency = 42 cycles

	//A Malloc of 64kB should give us enough data to cover the entire L1.
	ADDR_PTR *BASE_ADDR;
	ADDR_PTR *TARGET_ADDR;
	ADDR_PTR *TARGET_BASE;
	ADDR_PTR  TAG_INCR=(0x1 << 9);
	ADDR_PTR  GET_TIME_ADDR;

	int i,j,k;
	volatile int time, score;
	int score_array[SETS];

	printf("Performing Malloc \n");

	BASE_ADDR = (ADDR_PTR *)malloc(sizeof(int)* 1024 * 16 * 16 * 8);
	TARGET_BASE = BASE_ADDR;
	printf("Now starting Tests \n");

	//Flush the caches for the test addresses
        for(i=0; i<SETS; i++)
        {
 		TARGET_ADDR = TARGET_BASE;
		 for(j=0; j<SET_COUNT; j++)
			{
			clflush(TARGET_ADDR);
			TARGET_ADDR = TARGET_ADDR + TAG_INCR;
			}
              	TARGET_BASE = TARGET_BASE + SET_SIZE;
	}

	delay();

	//Now progressively test each set (it can hav( upt( 8 tags)


	TARGET_BASE = BASE_ADDR;

	for(i=0; i<SETS; i++) 
	{
		//Initialize the score
		score = 0;
		score_array[i] = 0;

		print_addr((long int) TARGET_BASE);
		//Test many times for each line to minimize noise effects
		for(k=0; k<NUM_TESTS; k++)
		{
 			TARGET_ADDR = TARGET_BASE;

			
			//Write junk to all the cache lines
			for(j=0; j<SET_COUNT; j++)
			{
			*TARGET_ADDR = JUNK;
			TARGET_ADDR = TARGET_ADDR + TAG_INCR;
			
			}
			
			//Read the Junks
 			TARGET_ADDR = TARGET_BASE;
			for(j=0; j<SET_COUNT; j++)
			{
			*TARGET_ADDR = *TARGET_ADDR + 0x1;
			TARGET_ADDR = TARGET_ADDR + TAG_INCR;
			
			}
			
			//Wait for sometime so that the sender can populate
			delay();

			time = 0;

			TARGET_ADDR = TARGET_BASE;


			//calculate the access time to each cache set 
			for(j=0; j<SET_COUNT; j++)
			{
			GET_TIME_ADDR = (ADDR_PTR)TARGET_ADDR;
			time = time + measure_one_block_access_time(GET_TIME_ADDR);
			TARGET_ADDR = TARGET_ADDR + TAG_INCR;
			}
			
			printf("Time taken for %d set in %dth iteration is: %d\n", i,k,time);

			//If the sender 
			if(time > THRESHOLD)
				score++;
		
			
		}
		//Accumulate the score
		score_array[i]= score_array[i] + score;

		TARGET_BASE = TARGET_BASE + SET_SIZE;
	}
	
	int set_num = find_max(score_array, SETS);

	printf("Found the Sender base: %d\n" , set_num);

	//Set the base address now
	BASE_ADDR = BASE_ADDR + (set_num * SET_SIZE);
	TARGET_BASE = BASE_ADDR;
	
	print_addr((long int) BASE_ADDR);
	while(1){
		//Read Write
//		for(i=0; i<3;i++) {
                        TARGET_ADDR = TARGET_BASE;
			//Write junk to all the cache lines
                        for(j=0; j<SET_COUNT; j++)
                        {
                        *TARGET_ADDR = JUNK;
                        TARGET_ADDR = TARGET_ADDR + TAG_INCR;
                        
                        }
                        
                        //Read the Junks
                        TARGET_ADDR = TARGET_BASE;
                        for(j=0; j<SET_COUNT; j++)
                        {
                        *TARGET_ADDR = *TARGET_ADDR + 0x1;
                        TARGET_ADDR = TARGET_ADDR + TAG_INCR;
                        
                        }
//		}
                        
                        //Wait for sometime so that the sender can populate
                        delay();
                        time = 0;
                        
                        TARGET_ADDR = TARGET_BASE;
                        //calculate the access time to each cache set 
                        for(j=0; j<SET_COUNT; j++)
                        {
                        GET_TIME_ADDR = (ADDR_PTR)TARGET_ADDR;
                        time = time + measure_one_block_access_time(GET_TIME_ADDR);
                        TARGET_ADDR = TARGET_ADDR + TAG_INCR;
                        }
                        
                        printf("Time taken is: %d\n", time);
	}

	printf("Please press enter.\n");

	char text_buf[2];
	fgets(text_buf, sizeof(text_buf), stdin);

	printf("Receiver now listening.\n");

	bool listening = true;
	while (listening) {

		// Put your covert channel code here

	}

	printf("Receiver finished.\n");

	return 0;
}


