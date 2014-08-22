#include "vm_primitives.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/kern_memorystatus.h>

#define debug_print(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

static unsigned int read_sysctl_int(const char* name)
{
    unsigned int var;
    size_t var_size;
    int error;

    var_size = sizeof(var);
    error = sysctlbyname(name, &var, &var_size, NULL, 0);
    if( error ) {
        printf("%s failed to get %s due to %d\n", __FUNCTION__, name, error);
        exit(-1);
    }
    return var;
}

static int get_pressure_level(unsigned int *current_level)
{

    *current_level = 0;
    *current_level = read_sysctl_int("kern.memorystatus_vm_pressure_level");
    return 0;
}

static unsigned long long getPhysMemoryBytes()
{
    int mib[2];
    unsigned long long physical_memory;
    size_t length;

    mib[0] = CTL_HW;
    mib[1] = HW_MEMSIZE;
    length = sizeof(physical_memory);
    sysctl(mib, 2, &physical_memory, &length, NULL, 0);
    return physical_memory;
}

static unsigned int pagesize()
{
    //memoize, bitch.
    static unsigned int pagesize = 0;
    if (!pagesize) {
        int mib[6];
        mib[0] = CTL_HW;
        mib[1] = HW_PAGESIZE;

        debug_print("Reading pagesize from sysctl\n", errno);
        size_t length;
        length = sizeof(pagesize);
        if (sysctl(mib, 2, &pagesize, &length, NULL, 0) < 0) {
            pagesize = 0;
            printf("Failed to get memory usage: %d", errno);
            exit(-1);
        }
    }
    return pagesize;
}

static double total_minus_free_over_total()
{
    unsigned long long free_count = vmstat_free_count();
    unsigned long long inactive_count = vmstat_inactive_count();

    unsigned long long bytesFree = (free_count + inactive_count) * pagesize();
    unsigned long long bytesTotal = getPhysMemoryBytes();

    double used = (double)(bytesTotal - bytesFree) / (double)bytesTotal;

    return (used * 100.0);
}

static int get_percent_free(unsigned int* level)
{
    int error;

    // This is how AAPL's memory_pressure tool reports "System-wide memory free percentage":
    error = memorystatus_get_level((user_addr_t) level);

    if( error ) {
        printf("memorystatus_get_level failed:");
        exit(-1);
    }
    return error;
}

void usage()
{
    printf("utsl... HAHAHA YOU'VE BEEN SUFFIELDED\n");
}

int main(int argc, char **argv)
{
    int mode_help = 0;
    int mode_fields = 0;
    int mode_continuous = 0;
    int mode_data = 1;
    int sleep_between_samples = 1;

    int c;

    while ((c = getopt(argc, argv, "hfcds:")) != -1) {
        switch (c) {
            case 'h':
                mode_help = 1;
                break;
            case 'f':
                mode_fields = 1;
                break;
            case 'c':
                mode_continuous =1;
                break;
            case 'd':
                mode_data = 1;
                break;
            case 's':
                sleep_between_samples = atoi(optarg);
                break;
            default:
                usage();
                abort();
                break;
        }
    }

    if (mode_help) {
        usage();
        return 0;
    } else if (mode_fields) {
        printf( "vmstat_free_count, "
                "vmstat_inactive_count, "
                "vmstat_wire_count, "
                "vmstat_zero_fill_count, "
                "vmstat_reactivations, "
                "vmstat_pageins, "
                "vmstat_pageouts, "
                "vmstat_faults, "
                "vmstat_cow_faults, "
                "vmstat_lookups, "
                "vmstat_hits, "
                "vmstat_purges, "
                "vmstat_purgable_count, "
                "vmstat_speculative_count, "
                "vmstat_decompressions, "
                "vmstat_compressions, "
                "vmstat_swapins, "
                "vmstat_swapouts, "
                "vmstat_compressor_page_count, "
                "vmstat_throttled_count, "
                "vmstat_external_page_count, "
                "vmstat_internal_page_count, "
                "vmstat_total_uncompressed_pages_in_compressor, "
                "100-minus-total-vs-free-estimate, "
                "memory free percentage, "
                "pressure level\n");
        return 0;
    } else {
        assert(mode_data && "mode_data should not be false here");
        do {
            unsigned int percent_free;
            unsigned int pressure_level;

            get_percent_free(&percent_free);
            get_pressure_level(&pressure_level);
            printf( "%llu, %llu, %llu, %llu, %llu, %llu, %llu, %llu, %llu, %llu, %llu, %llu, %llu, %llu, %llu, %llu, %llu, %llu, %u, %u, %u, %u, %llu, %f, %d, %d\n",
                    vmstat_free_count(),
                    vmstat_inactive_count(),
                    vmstat_wire_count(),
                    vmstat_zero_fill_count(),
                    vmstat_reactivations(),
                    vmstat_pageins(),
                    vmstat_pageouts(),
                    vmstat_faults(),
                    vmstat_cow_faults(),
                    vmstat_lookups(),
                    vmstat_hits(),
                    vmstat_purges(),
                    vmstat_purgable_count(),
                    vmstat_speculative_count(),
                    vmstat_decompressions(),
                    vmstat_compressions(),
                    vmstat_swapins(),
                    vmstat_swapouts(),
                    vmstat_compressor_page_count(),
                    vmstat_throttled_count(),
                    vmstat_external_page_count(),
                    vmstat_internal_page_count(),
                    vmstat_total_uncompressed_pages_in_compressor(),
                    100-total_minus_free_over_total(),
                    percent_free,
                    pressure_level*20
                    );
            sleep((mode_continuous%2) * sleep_between_samples);
        } while (mode_continuous);
    }
    return 0;
}

