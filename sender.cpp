
#include"util.hpp"

#define DELAY_FACTOR 100000
#define DELAY_FACTOR_SHORT 100

#define MEM_RW(x)   	(*(volatile unsigned long *)(x))

#define SET_COUNT 	8
#define BLOCK_COUNT	8
#define SETS 		64
#define SET_SIZE	(0x1 << 3)

#define TEST_LENGTH	0xFFFFFF
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
	ADDR_PTR  TAG_INCR=(0x1 << 12);
	
	int i,j,k;

	BASE_ADDR = (ADDR_PTR *)malloc(sizeof(int)* 1024 * 16*16);
	
	print_addr((long int) BASE_ADDR);

	//Now progressively test each set (it can have upto 8 tags)
	for(i=0; i<TEST_LENGTH; i++) 
	{
		TARGET_ADDR = BASE_ADDR;
/*
			//Flush the Cache lines
			for(j=0; j<SET_COUNT; j++)
			{
			clflush(TARGET_ADDR);
			delay_short();
			TARGET_ADDR = TARGET_ADDR + TAG_INCR;
			}
*/
			//Fill the entire cache set
                        TARGET_ADDR = BASE_ADDR;
			TARGET_BASE = BASE_ADDR;
                        for(j=0; j<SET_COUNT; j++)
                        {
				//Fill the block
				for(k=0; k<BLOCK_COUNT; k++)
                        	{	
					*TARGET_ADDR =(long int) JUNK;
					TARGET_ADDR = TARGET_ADDR + 0x1;
				}
                        TARGET_BASE= TARGET_BASE + TAG_INCR;
			TARGET_ADDR = TARGET_BASE;	

                        }

			//Read the entire Cache set
                        TARGET_ADDR = BASE_ADDR;
                        TARGET_BASE = BASE_ADDR;
                        for(j=0; j<SET_COUNT; j++)
                        {
                                for(k=0; k<BLOCK_COUNT; k++)
                                {
                                        *TARGET_ADDR = *TARGET_ADDR + (long int) 0x1;
                                        TARGET_ADDR = TARGET_ADDR + 0x1;
                                }
                        TARGET_BASE= TARGET_BASE + TAG_INCR;
                        TARGET_ADDR = TARGET_BASE;

                        }
/*

                        TARGET_ADDR = BASE_ADDR;
                        //Flush the Cache lines
                        for(j=0; j<SET_COUNT; j++)
                        {
                        clflush(TARGET_ADDR);
                        delay_short();
                        TARGET_ADDR = TARGET_ADDR + TAG_INCR;
                        }
		//Wait for sometime so that the sender can populate
*/		delay();

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


