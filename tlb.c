#include <stdio.h>
#include <stdlib.h>
#include <math.h>

char ADDRESS_SPACE_SIZE = 16; // 16-bit address space

typedef struct {
    unsigned int vpn; // Virtual Page Number
    unsigned int ppn; // Physical Page Number
} PageTableEntry;

typedef struct {
    unsigned int vpn; // Virtual Page Number
    unsigned int ppn; // Physical Page Number
    int referenced;
} TLBEntry;

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
    int clock_hand = 0;

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
    unsigned int PAGE_SIZE = *PAGE_SIZE_B;
    int PAGE_TABLE_ENTRIES = pow(2, ADDRESS_SPACE_SIZE) / *PAGE_SIZE_B;
    PageTableEntry* PAGE_TABLE = NULL;
    PAGE_TABLE = malloc(PAGE_TABLE_ENTRIES * sizeof(PageTableEntry));

    // Iterate 2 bytes at a time the correct number of PTEs to get the physical address mappings
    for (int i = 0; i < PAGE_TABLE_ENTRIES; i++) {
        if (fread(&PAGE_TABLE[i].ppn, 2, 1, fp) == 1) {
            PAGE_TABLE[i].vpn = i; // Assign the Virtual Page Number to the corresponding Page Table Entry
        }
        else {
            printf("Something went wrong with fread!\n");
            exit(-1);
        }
    }

    int TLB_ENTRIES = *TLB_SIZE;
    TLBEntry* TLB = malloc(TLB_ENTRIES * sizeof(TLBEntry));
    int tlbIndex = 0; // Index for the TLB replacement

    for (int i = 0; i < *TLB_SIZE; i++) {
        TLB[i].vpn = -1;
        TLB[i].ppn = -1;
        TLB[i].referenced = 0;
    }

    while (fread(NEW_ADDRESS, 2, 1, fp) == 1) {
        int tlbHit = 0;
        unsigned int PPN;
        unsigned int VPN;
        unsigned int offset;
        unsigned int TRANSLATED_PHYSICAL_ADDRESS;

        VPN = *NEW_ADDRESS / *PAGE_SIZE_B; // Calculate the Virtual Page Number
        // printf("\nVPN:%hu\n",VPN);
        for (int i = 0; i < TLB_ENTRIES; i++) {
            // printf("TLB[%d]: %hu\n",i,TLB[i].vpn);
            if (TLB[i].vpn == VPN) {
                PPN = TLB[i].ppn;
                offset = *NEW_ADDRESS % *PAGE_SIZE_B;
                TRANSLATED_PHYSICAL_ADDRESS = (PPN * *PAGE_SIZE_B) + offset;
                tlbHit = 1;
                TLB[i].referenced = 1;
                TLB_HITS++;
                // printf("Inside");
                break;
            }
        }
        if(!tlbHit){
            if(REPL == 0){
                TLB_MISSES++;
                PPN = PAGE_TABLE[VPN].ppn;
                offset = *NEW_ADDRESS % *PAGE_SIZE_B;
                TRANSLATED_PHYSICAL_ADDRESS = (PPN * *PAGE_SIZE_B) + offset;
                TLB[tlbIndex].vpn = VPN;
                TLB[tlbIndex].ppn = PPN;
                tlbIndex++;
                if(tlbIndex == TLB_ENTRIES){
                    tlbIndex = 0;
                }
            }
            else if(REPL == 1){
                TLB_MISSES++;
                PPN = PAGE_TABLE[VPN].ppn;
                offset = *NEW_ADDRESS % *PAGE_SIZE_B;
                TRANSLATED_PHYSICAL_ADDRESS = (PPN * *PAGE_SIZE_B) + offset;
                int replaceIndex = rand() % *TLB_SIZE;
                TLB[replaceIndex].vpn = VPN;
                TLB[replaceIndex].ppn = PPN;
            }
            else if(REPL == 2){
                PPN = PAGE_TABLE[VPN].ppn;
                offset = *NEW_ADDRESS % *PAGE_SIZE_B;
                TRANSLATED_PHYSICAL_ADDRESS = (PPN * *PAGE_SIZE_B) + offset;
                int replaced_entry_index = -1;
                while (1) {
                    clock_hand = (clock_hand + 1) % *TLB_SIZE;
                    if (TLB[clock_hand].referenced == 0) {
                        replaced_entry_index = clock_hand;
                        break;
                    } else {
                        TLB[clock_hand].referenced = 0; // Reset the reference bit
                    }
                }

                // Update the TLB with the new entry
                TLB[replaced_entry_index].vpn = VPN;
                TLB[replaced_entry_index].ppn = PPN;
                TLB[replaced_entry_index].referenced = 1;
                clock_hand = replaced_entry_index;

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
    fclose(fp);
}
