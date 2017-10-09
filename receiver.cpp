#include "util.hpp"
#include "inttypes.h"

#define DELAY_FACTOR 100000000
#define DELAY_FACTOR_SHORT 100

#define WAY_COUNT 	8
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
	
void print_time_score(int time_array[], int score_array[], int size)
{
	for(int i=0; i<size; i++)
	{
		printf("set #%d: time = %d, score = %d\n", i, time_array[i], score_array[i]);
	}
}	

void print_addr(long int a)
{
	int block = (a & 0x3f);
	int set = (a & 0xfC0)>>6;
	int tag = (a & 0xfff000)>>12;

	printf("Addr: %lx, block: %x , Tag: %x, Set: %x\n", a, block, tag, set);

}

ADDR_PTR* base_address_gen(long int BASE_ADDR)
{
	return (ADDR_PTR*)(((BASE_ADDR >> 12) + 2) << 12);
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
	ADDR_PTR  TAG_INCR=(0x1 << 13);
	ADDR_PTR  GET_TIME_ADDR;

	int i,j,k;
	volatile int time;
	int score_array[SETS];
    	int time_array[SETS];

	printf("Performing Malloc \n");

	BASE_ADDR = (ADDR_PTR *)malloc(sizeof(int)* 1024 * 16 * 16 * 8);
	BASE_ADDR = base_address_gen((long int)BASE_ADDR);

	TARGET_BASE = BASE_ADDR;
	printf("Now starting Tests \n");

	//Initialize the score
	for(i=0; i < SETS; i++)
	{
		score_array[i] = 0;
        	time_array[i] = 0;
	}

	for(k=0; k<NUM_TESTS; k++) 
	{
		TARGET_BASE = BASE_ADDR;

		//print_addr((long int) TARGET_BASE);
		//Test many times for each line to minimize noise effects
		for(i=0; i<SETS; i++)
		{
            
 			TARGET_ADDR = TARGET_BASE;

			//Write junk to all the cache lines
			for(j=0; j<WAY_COUNT; j++)
			{
				*TARGET_ADDR = JUNK;
				TARGET_ADDR = TARGET_ADDR + TAG_INCR;
			}

			//Read the Junks
 			TARGET_ADDR = TARGET_BASE;
			for(j=0; j<WAY_COUNT; j++)
			{
				*TARGET_ADDR = *TARGET_ADDR + 0x1;
				TARGET_ADDR = TARGET_ADDR + TAG_INCR;
			}
		}

		delay();
		for(i=0; i<SETS; i++)
		{
			time = 0;

			TARGET_ADDR = TARGET_BASE;

			//calculate the access time to every way in each cache set 
			for(j=0; j<WAY_COUNT; j++)
			{
				GET_TIME_ADDR = (ADDR_PTR)TARGET_ADDR + rand() % 8;
				time = time + measure_one_block_access_time(GET_TIME_ADDR);
				TARGET_ADDR = TARGET_ADDR + TAG_INCR;
			}

			//printf("Time taken for %d set in %dth iteration is: %d\n", i,k,time);
            		time_array[i] = time_array[i] + time;

			//If the sender
			if(time > THRESHOLD)
				//Accumulate the score
				score_array[i] = score_array[i] + 0x1;

			TARGET_BASE = TARGET_BASE + SET_SIZE;
		}

	}

	print_time_score(time_array, score_array, SETS);

	//printf("Found the Sender base: %d\n" , set_num);

	////Set the base address now
	//BASE_ADDR = BASE_ADDR + (set_num * SET_SIZE);
	//TARGET_BASE = BASE_ADDR;
	
	print_addr((long int) BASE_ADDR);
	while(1){
		//Read Write
//		for(i=0; i<3;i++) {
                        TARGET_ADDR = TARGET_BASE;
			//Write junk to all the cache lines
                        for(j=0; j<WAY_COUNT; j++)
                        {
                        	*TARGET_ADDR = JUNK;
                        	TARGET_ADDR = TARGET_ADDR + TAG_INCR;
                        
                        }
                        
                        //Read the Junks
                        TARGET_ADDR = TARGET_BASE;
                        for(j=0; j<WAY_COUNT; j++)
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
                        for(j=0; j<WAY_COUNT; j++)
                        {
                        	GET_TIME_ADDR = (ADDR_PTR)TARGET_ADDR;
                        	time = time + measure_one_block_access_time(GET_TIME_ADDR);
                        	TARGET_ADDR = TARGET_ADDR + TAG_INCR;
                        }
                        
//                        printf("Time taken is: %d\n", time);
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


