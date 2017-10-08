
#include"util.hpp"

#define DELAY_FACTOR 5000000

#define MEM_RW(x)   	(*(volatile unsigned long *)(x))

#define SET_COUNT 	8
#define SETS 		64
#define SET_SIZE	64

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

int main(int argc, char **argv)
{
        //Prepare to find out the cache-line mapped:
	//Strategy: Flush each cache-line multiple times and figure out the delay

   	//L1 Data cache = 32 KB, 64 B/line, 8-WAY.
	// That gives us 6bits of Block address, 7 bits of set address
	//L1 Data Cache Latency = 4 cycles for simple access via pointer
	//RAM Latency = 42 cycles

	//A Malloc of 64kB should give us enough data to cover the entire L1.
	ADDR_PTR *BASE_ADDR;
	ADDR_PTR *TARGET_ADDR;
	ADDR_PTR  TAG_INCR=(0x1 << 12);
	ADDR_PTR  GET_TIME_ADDR;

	int i,j,k;
	int time, score;
	int score_array[SETS];

	printf("Performing Malloc \n");

	BASE_ADDR = (ADDR_PTR *)malloc(sizeof(int)* 1024 * 16 * 16);

	printf("Now starting Tests \n");

	//Now progressively test each set (it can have upto 8 tags)
	for(i=0; i<SETS; i++) 
	{
		//Initialize the score
		score = 0;
		score_array[i] = 0;

		//Test many times for each line to minimize noise effects
		for(k=0; k<10; k++)
		{
 			TARGET_ADDR = BASE_ADDR + (i * SET_SIZE);
/*		
                        //Flush the Cache lines
                        for(j=0; j<SET_COUNT; j++)
                        {
                        clflush(TARGET_ADDR);
                        TARGET_ADDR = TARGET_ADDR + TAG_INCR;
                        }

			delay();
*/	
/*			TARGET_ADDR = BASE_ADDR + (i * SET_SIZE);

//			if(i==1)
//			{			
			//Write junk to all the cache lines
			for(j=0; j<SET_COUNT; j++)
			{
			*TARGET_ADDR = JUNK;
			TARGET_ADDR = TARGET_ADDR + TAG_INCR;
			}
			
			}

			else {
*/
			//Flush the Cache lines
			for(j=0; j<SET_COUNT; j++)
			{
			clflush(TARGET_ADDR);
			TARGET_ADDR = TARGET_ADDR + TAG_INCR;
			}

//			}
	
			//Wait for sometime so that the sender can flush
			delay();

			time = 0;

			TARGET_ADDR = BASE_ADDR + (i * SET_SIZE);
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
	}
	
	int set_num = find_max(score_array, SETS);

	printf("Found the Sender base: %d" , set_num);

	//Set the base address now
	BASE_ADDR = BASE_ADDR + (set_num * SET_SIZE);

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


