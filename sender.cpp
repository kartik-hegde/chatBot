#include"util.hpp"

#define DELAY_FACTOR 1000
#define DELAY_FACTOR_SHORT 100

#define MEM_RW(x)   	(*(volatile unsigned long *)(x))

#define SET_COUNT 	8
#define BLOCK_COUNT	8
#define SETS 		64
#define SET_SIZE	(0x1 << 3)

#define TEST_LENGTH	0xFFFFFFF
#define JUNK		0xBABE

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
	
	return 0;
}

int delay_short()
{
	int i;
	int j = 0;

	for(i=0; i<DELAY_FACTOR_SHORT; i++)
		j++; 
}

void print_addr(long int a)
{
	int block = (a & 0x3f);
	int set = (a & 0xfC0)>>6;
	int tag = (a & 0xfff000)>>12;

	printf("Addr: %lx, block: %x , Tag: %x, Set: %x\n", a, block, tag, set);

}

ADDR_PTR* new_addr(ADDR_PTR *BASE_ADDR, int index)
{
	ADDR_PTR* TARGET_ADDR = (ADDR_PTR*) ( (((uintptr_t)BASE_ADDR >> 6) + index) << 6);
 	return TARGET_ADDR;
}

ADDR_PTR* base_address_gen(long int BASE_ADDR)
{
   printf("%lx\n", BASE_ADDR);
   return (ADDR_PTR*) (((BASE_ADDR >> 12) + 2) << 12);

}

#define MSG 0x0000000000001010

int main(int argc, char **argv)
{
	//Strategy: Keep on flushing the cache line, so that receiver knows
   	
	//L1 Data cache = 32 KB, 64 B/line, 8-WAY.
	// That gives us 6bits of Block address, 6 bits of set address
	//L1 Data Cache Latency = 4 cycles for simple access via pointer
	//RAM Latency = 42 cycles

	//A Malloc of 64kB should give us enough data to cover the entire L1.
	ADDR_PTR *BASE_ADDR;
	ADDR_PTR *TARGET_ADDR;
	ADDR_PTR *TARGET_BASE;
	ADDR_PTR  GET_TIME_ADDR;

	int i,j,k,l;

	volatile int time, score;
	long int msg = MSG;

	BASE_ADDR = (ADDR_PTR*) malloc(sizeof(int)* 1024 * 16*16);
	
	BASE_ADDR = base_address_gen((long int)BASE_ADDR);

	print_addr((long int) BASE_ADDR);

	//Now progressively test each set (it can have upto 8 tags)
	for(i=0; i<TEST_LENGTH; i++) 
	{
		msg = MSG;
		for(j=0; j<SETS; j++)
		{
			if(msg & 0x1)
			{
				TARGET_ADDR = new_addr(BASE_ADDR, j);
				//Fill the entire cache set
				TARGET_BASE = TARGET_ADDR;
				for(k=0; k<SET_COUNT; k++)
				{
					//Fill the block
					for(l=0; l<BLOCK_COUNT; l++)
					{	
						*TARGET_ADDR =(long int) JUNK;
						TARGET_ADDR = TARGET_ADDR + 0x1;
					}
					TARGET_BASE= TARGET_BASE + TAG_INCR;	
					TARGET_ADDR = TARGET_BASE;	

				}



				TARGET_ADDR = new_addr(BASE_ADDR, j);
				
				//Fill the entire cache set
				TARGET_BASE = TARGET_ADDR;
				for(k=0; k<SET_COUNT; k++)
				{
					//Fill the block
					for(l=0; l<BLOCK_COUNT; l++)
					{	
						//*TARGET_ADDR = *TARGET_ADDR + 0x1;
						clflush(TARGET_ADDR);
						delay_short();
						TARGET_ADDR = TARGET_ADDR + 0x1;
					}
				TARGET_BASE= TARGET_BASE + TAG_INCR;
				TARGET_ADDR = TARGET_BASE;	

                        	}
			  }

			msg = msg >> 1;
		}

	}


	printf("Please type a message.\n");

	bool sending = true;
	while (sending) {
		char text_buf[128];
		fgets(text_buf, sizeof(text_buf), stdin);
	
		// Put your covert channel code here
	}

	printf("Sender finished.\n");

	return 0;
}


