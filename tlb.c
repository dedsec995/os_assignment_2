#include <stdio.h>
#include <stdlib.h>
#include <math.h>
char  ADDRESS_SPACE_SIZE = 16; //16 bit address space
char  * PAGE_SIZE_B = NULL; //? byte page size
short unsigned int * PPN = NULL; //Physical Page Number
short unsigned int * NEW_ADDRESS = NULL; //New Virtual Address to service
short   unsigned int * TLB_SIZE = NULL; //? Translation Lookaside Buffer entries
unsigned char REPL; //0 for FIFO, 1 for Random, 2 for Clock 
int seed;

int main(int argc, char * argv[]){
    if(argc < 3 || argc > 4){//You will take in 3 arguments: a binary test file, the replacement algorithm (0, 1, or 2), and a seed for the Random replacement if used
        printf("You must enter an input file to test and a replacement policy!\n");
        exit(-1);
    }
    
    FILE * fp;
    PAGE_SIZE_B = malloc(1);
    NEW_ADDRESS = malloc(2); 
    PPN = malloc(2);
    TLB_SIZE = malloc(1);
    
    fp = fopen(argv[1], "rb");
    REPL = atoi(argv[2]);
    if(argc == 4){
        seed = atoi(argv[3]);
        srand(seed);
    }
    fread(PAGE_SIZE_B, 1, 1, fp); //The first byte of the binary will be the page size
    fread(TLB_SIZE, 1, 1, fp); //The second byte of the binary will be the TLB size
    short int PAGE_TABLE_ENTRIES;
    //PAGE_TABLE_ENTRIES = You must figure out how many Page Table Entries there are!
    //Iterate 2 bytes at a time the correct number of PTEs to get the physical address mappings
    for(int i = 0; i < PAGE_TABLE_ENTRIES; i++){
        if (fread(PPN, 2, 1, fp) == 1){
            //Here we read 1 (2 byte) Physical Address at a time; you must map it to the corresponding Page Table Entry
        }else{
            printf("Something went wrong with fread!\n");
            exit(-1);
        }
    }

    while (fread(NEW_ADDRESS, 2, 1, fp) == 1) {
        //Below is how the print statement should be formatted
        //printf("VA:%x -- PA:%x\n", VIRTUAL_ADDRESS, TRANSLATED_PHYSICAL_ADDRESS);
    }
    //Below is how the print statement should be formatted
    //printf("Page Faults: %d\nTLB hits: %d\nTlb misses: %d\n", PAGE_FAULTS, TLB_HITS, TLB_MISSES);
    free(PAGE_SIZE_B);
    free(NEW_ADDRESS);
    free(PPN);
    free(TLB_SIZE);
    fclose(fp);
}