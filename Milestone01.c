// CS 3853 VM and Cache Simulator Project PT 1
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>  

// Help function
int log2_int(unsigned int x) {
    int r = 0;
    while (x > 1) {
        x >>= 1;
        r++;
    }
    return r;
}

// Main
/* CacheSize: 8 KB to 8 MB in powers of 2: 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192
   Block Size: 8, 16, 32, 64 byte blocks
   Associativity: direct-mapped, 2-way, 4-way, 8-way, or 16-way set associative
   Replacement Policy: round-robin OR random
   Physical Memory: 128 MB to 4096 MB in powers of 2
   Virtual address space is set at 4GB (32 bits)
   Able to handle up to 3 trace files – each represents a different process
*/
int main(int argc, char *argv[]) {

    int cache_size_kb = 0;
    int block_size = 0;
    int associativity = 0;
    char replacement[20];
    int physical_mem_mb = 0;
    char trace_files[3][128]; // store up to 3 trace file names
    int trace_count = 0;
    double percent = 0.0;
    int instructions = 0;

    // ***** Cache Input Parameters *****

    /* 4.1.Simulator Input and Memory Trace Files
    s <cache size - KB> [ 8 to 8192 KB]
    b <block size> [ 8 bytes to 64 bytes ]
    a <associativity> [ 1, 2, 4, 8, 16 ]
    r <replacement policy> [ RR or RND ], Implement one of these 
    p <physical memory - MB> [ 128 MB to 4 GB ]
    u <%physicalmemusedbyOS> [0%to100%]
    n <Instructions / Time Slice [ 1 to All ] = max (Enter -1 for max)
    f <trace file name> [ name of text file with the trace ]

    must accept 1, 2, or 3 trace files as input o Each file will use a -f  to specify it
    */

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0) cache_size_kb = atoi(argv[++i]);
        else if (strcmp(argv[i], "-b") == 0) block_size = atoi(argv[++i]);
        else if (strcmp(argv[i], "-a") == 0) associativity = atoi(argv[++i]);
        else if (strcmp(argv[i], "-r") == 0) strcpy(replacement, argv[++i]);
        else if (strcmp(argv[i], "-p") == 0) physical_mem_mb = atoi(argv[++i]);
        else if (strcmp(argv[i], "-u") == 0) percent = atof(argv[++i]);
        else if (strcmp(argv[i], "-n") == 0) instructions = atoi(argv[++i]);
        else if (strcmp(argv[i], "-f") == 0 && trace_count < 3)
        strcpy(trace_files[trace_count++], argv[++i]);
    }

    // validation
    if (cache_size_kb == 0 || block_size == 0 || associativity == 0 || physical_mem_mb == 0 || trace_count == 0) {
        printf("\nUsage: ./VMProject -s <sizeKB> -b <block> -a <assoc> -r <rr|rnd> -p <physMB> -u <percent> -n <inst> -f <trace1> [-f trace2] [-f trace3]\n\n");
        return 1;
    }

    // ***** Cache Calculated Values *****
    double cache_size_bytes = cache_size_kb * 1024.0;
    int total_blocks = (int)(cache_size_bytes / block_size);
    int num_sets = total_blocks / associativity;
    int index_bits = log2_int(num_sets);
    int offset_bits = log2_int(block_size);

    // int tag_bits = (32 - (index_bits )- offset_bits); // "Assume a 32-bit data bus and 32-bit virtual address space..."
    // Determine tag bits based on PHYSICAL memory, not virtual
    int pa_bits = 20 + log2_int((unsigned int)physical_mem_mb);
    int tag_bits = pa_bits - index_bits - offset_bits;

    // Calculate overhead and implementation memory
    int overhead_bytes = (total_blocks / 8) + (int)(num_sets * 6.5);  // 1 valid bit per block //(num_sets * 6.5)
    double implement_memory_bytes = cache_size_bytes + overhead_bytes; 
    double implement_memory_kb = implement_memory_bytes / 1024.0;
    double cost = implement_memory_kb * 0.07; // Cost per KB

    // ***** Physical Memory Calculated Values *****
    int page_size = 4096; // 4KB
    int num_phys_pages = (physical_mem_mb * 1024 * 1024) / page_size;
    int pages_for_system = (int)((percent / 100.0) * num_phys_pages);
    int page_table_entry_bits = 19; // 1 valid bit + 18 bits for PPN
    int total_ram_page_tables = (int)((512 * 1024) * trace_count * (page_table_entry_bits / 8.0));

// ***** OUTPUT *****
printf("\nCache Simulator - CS 3853\n");

printf("\nTrace File(s):\n");
for (int i = 0; i < trace_count; i++) {
    printf("  %s\n", trace_files[i]);
}

printf("\n***** Cache Input Parameters *****\n");
printf("%-35s %d KB\n", "Cache Size:", cache_size_kb);
printf("%-35s %d bytes\n", "Block Size:", block_size);
printf("%-35s %d\n", "Associativity:", associativity);
printf("%-35s %s\n", "Replacement Policy:",
       (strcmp(replacement, "rr") == 0) ? "Round Robin" : "Random");
printf("%-35s %d MB\n", "Physical Memory:", physical_mem_mb);
printf("%-35s %.1f%%\n", "Percent Memory Used by System:", percent);
printf("%-35s %d\n", "Instructions / Time Slice:", instructions);

printf("\n***** Cache Calculated Values *****\n");
printf("%-35s %d\n", "Total # Blocks:", total_blocks);
printf("%-35s %d bits (based on actual physical memory)\n", "Tag Size:", tag_bits);
printf("%-35s %d bits\n", "Index Size:", index_bits);
printf("%-35s %d\n", "Total # Rows:", num_sets);
printf("%-35s %d bytes\n", "Overhead Size:", overhead_bytes);
printf("%-35s %.2f KB (%.0f bytes)\n", "Implementation Memory Size:",
       implement_memory_kb, implement_memory_bytes);
printf("%-35s $%.2f @ $0.07 per KB\n", "Cost:", cost);

printf("\n***** Physical Memory Calculated Values *****\n");
printf("%-35s %d\n", "Number of Physical Pages:", num_phys_pages);
printf("%-35s %d (%.2f × %d = %d)\n", "Number of Pages for System:",
       pages_for_system, percent / 100.0, num_phys_pages, pages_for_system);
printf("%-35s %d bits (1 valid bit, 18 for PhysPage)\n",
       "Size of Page Table Entry:", page_table_entry_bits);
printf("%-35s %d bytes (512K entries × %d .trc files × 19 / 8)\n",
       "Total RAM for Page Table(s):", total_ram_page_tables, trace_count);
    return 0;
}
