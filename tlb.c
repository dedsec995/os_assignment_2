#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    unsigned int vpn;
    unsigned int ppn;
    unsigned int valid;
} PageTableEntry;

typedef struct {
    unsigned int vpn; 
    unsigned int ppn;
    unsigned int valid;
} MemoryEntry;

typedef struct {
    unsigned int vpn; 
    unsigned int ppn; 
    int referenced;
} TLBEntry;

char ADDRESS_SPACE_SIZE = 16; // 16-bit address space
char* PAGE_SIZE_B = NULL; // ? byte page size
short unsigned int* NEW_ADDRESS = NULL; // New Virtual Address to service
short unsigned int* TLB_SIZE = NULL; // ? Translation Lookaside Buffer entries
unsigned char REPL; // 0 for FIFO, 1 for Random, 2 for Clock
int seed;

int main(int argc, char* argv[]) {
    if (argc < 3 || argc > 4) { // You will take in 3 arguments: a binary test file, the replacement algorithm (0, 1, or 2), and a seed for the Random replacement if used
        printf("You must enter an input file to test and a replacement policy!\n");
        exit(-1);
    }

    // Initializing the counters
    int PAGE_FAULTS = 0;
    int TLB_HITS = 0;
    int TLB_MISSES = 0;
    int clock_point = 0;

    //---------------------Binary File Loading and Reading-------------------------------

    FILE* fp;
    PAGE_SIZE_B = malloc(1);
    NEW_ADDRESS = malloc(2);
    TLB_SIZE = malloc(1);

    fp = fopen(argv[1], "rb");
    REPL = atoi(argv[2]);
    if (argc == 4) {
        seed = atoi(argv[3]);
        srand(seed);
    }
    fread(PAGE_SIZE_B, 1, 1, fp); // The first byte of the binary will be the page size
    fread(TLB_SIZE, 1, 1, fp);    // The second byte of the binary will be the TLB size

    //---------------------Binary File Loading and Reading-------------------------------

    int PAGE_TABLE_ENTRIES = 2048;
    PageTableEntry* PAGE_TABLE = NULL;
    PAGE_TABLE = malloc(PAGE_TABLE_ENTRIES * sizeof(PageTableEntry));

    int PPN_MEMORY_ENTRIES = pow(2, ADDRESS_SPACE_SIZE) / *PAGE_SIZE_B;
    MemoryEntry* MEMORY_TABLE = NULL;
    MEMORY_TABLE = malloc(PPN_MEMORY_ENTRIES * sizeof(MemoryEntry));

    // Iterate 2 bytes at a time the correct number of PTEs to get the physical address mappings
    for (int i = 0; i < PPN_MEMORY_ENTRIES; i++) {
        if (fread(&MEMORY_TABLE[i].ppn, 2, 1, fp) == 1) {
            MEMORY_TABLE[i].vpn = i; //
        }
        else {
            printf("Something went wrong with fread!\n");
            exit(-1);
        }
    }

    //-----------------------------------------Creating Page Table---------------------------------------------
    
    int page_table_index = 0;

    for (int i = 0; i < PAGE_TABLE_ENTRIES; i++) {
            PAGE_TABLE[i].vpn = MEMORY_TABLE[i].vpn;
            PAGE_TABLE[i].ppn = PAGE_TABLE[i].ppn;
            PAGE_TABLE[i].valid = PAGE_TABLE[i].valid;
        }

    //-----------------------------------------Creating Page Table---------------------------------------------


    //-----------------------------------------Creating TLB---------------------------------------------
    int TLB_ENTRIES = *TLB_SIZE;
    TLBEntry* TLB = malloc(TLB_ENTRIES * sizeof(TLBEntry));
    int tlb_index = 0; // Index for the TLB replacement

    for (int i = 0; i < *TLB_SIZE; i++) {
        TLB[i].vpn = -1;
        TLB[i].ppn = -1;
        TLB[i].referenced = 0;
    }

    //-----------------------------------------Creating TLB---------------------------------------------


    while (fread(NEW_ADDRESS, 2, 1, fp) == 1) {
        int tlb_hit = 0;
        int page_hit = 0;
        unsigned int PPN;
        unsigned int VPN;
        unsigned int offset;
        unsigned int TRANSLATED_PHYSICAL_ADDRESS;

        VPN = *NEW_ADDRESS / *PAGE_SIZE_B;
        // printf("\nVPN:%hu\n",VPN);
        for (int i = 0; i < TLB_ENTRIES; i++) {
            // printf("TLB[%d]: %hu\n",i,TLB[i].vpn);
            if (TLB[i].vpn == VPN) {
                PPN = TLB[i].ppn;
                offset = *NEW_ADDRESS % *PAGE_SIZE_B;
                TRANSLATED_PHYSICAL_ADDRESS = (PPN * *PAGE_SIZE_B) + offset;
                tlb_hit = 1;
                TLB[i].referenced = 1;
                TLB_HITS++;
                // printf("Inside");
                break;
            }
        }
        if(!tlb_hit){
            if(REPL == 0){ // FIFO
                TLB_MISSES++;
                for(int i = 0; i < PAGE_TABLE_ENTRIES; i++){
                    if(PAGE_TABLE[i].vpn == VPN){
                        page_hit = 1;
                        break;
                    }
                }
                if(!page_hit){
                    PPN = MEMORY_TABLE[VPN].ppn;
                    offset = *NEW_ADDRESS % *PAGE_SIZE_B;
                    TRANSLATED_PHYSICAL_ADDRESS = (PPN * *PAGE_SIZE_B) + offset;
                    PAGE_TABLE[page_table_index].vpn = VPN;
                    PAGE_TABLE[page_table_index].ppn = PPN;
                    page_table_index++;
                    if(page_table_index == PAGE_TABLE_ENTRIES){
                        page_table_index = 0;
                    }
                    PAGE_FAULTS++;
                    }
                PPN = MEMORY_TABLE[VPN].ppn;
                offset = *NEW_ADDRESS % *PAGE_SIZE_B;
                TRANSLATED_PHYSICAL_ADDRESS = (PPN * *PAGE_SIZE_B) + offset;
                TLB[tlb_index].vpn = VPN;
                TLB[tlb_index].ppn = PPN;
                tlb_index++;
                if(tlb_index == TLB_ENTRIES){
                    tlb_index = 0;
                }
            }
            else if(REPL == 1){ // RAND
                TLB_MISSES++;
                PPN = MEMORY_TABLE[VPN].ppn;
                offset = *NEW_ADDRESS % *PAGE_SIZE_B;
                TRANSLATED_PHYSICAL_ADDRESS = (PPN * *PAGE_SIZE_B) + offset;
                int replaceIndex = rand() % *TLB_SIZE;
                TLB[replaceIndex].vpn = VPN;
                TLB[replaceIndex].ppn = PPN;
            }
            else if(REPL == 2){ // CLOCK
                PPN = MEMORY_TABLE[VPN].ppn;
                offset = *NEW_ADDRESS % *PAGE_SIZE_B;
                TRANSLATED_PHYSICAL_ADDRESS = (PPN * *PAGE_SIZE_B) + offset;
                int replaced_entry_index = -1;
                while (1) {
                    clock_point = (clock_point + 1) % *TLB_SIZE;
                    if (TLB[clock_point].referenced == 0) {
                        replaced_entry_index = clock_point;
                        break;
                    } else {
                        TLB[clock_point].referenced = 0;
                    }
                }

                TLB[replaced_entry_index].vpn = VPN;
                TLB[replaced_entry_index].ppn = PPN;
                TLB[replaced_entry_index].referenced = 1;
                clock_point = replaced_entry_index;

                TLB_MISSES++;
            }
            else{
                printf("The replacement Police number is wrong");
                exit(-1);
            }
        }
        // Print the Virtual Address and Translated Physical Address
        printf("VA:%x -- PA:%x\n", *NEW_ADDRESS, TRANSLATED_PHYSICAL_ADDRESS);
    }
    // Below is how the print statement should be formatted
    printf("Page Faults: %d\nTLB hits: %d\nTlb misses: %d\n", PAGE_FAULTS, TLB_HITS, TLB_MISSES);
    free(PAGE_SIZE_B);
    free(NEW_ADDRESS);
    free(TLB_SIZE);
    free(PAGE_TABLE);
    free(MEMORY_TABLE);
    fclose(fp);
}
