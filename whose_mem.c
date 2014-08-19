#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <mach/mach_host.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/kern_memorystatus.h>

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

static double total_minus_free_over_total()
{
    int mib[6];
    mib[0] = CTL_HW;
    mib[1] = HW_PAGESIZE;

    unsigned int pagesize = 0;
    size_t length;
    length = sizeof(pagesize);
    if (sysctl(mib, 2, &pagesize, &length, NULL, 0) < 0) {
        printf("Failed to get memory usage: %d", errno);
        exit(-1);
    }

    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;

    vm_statistics64_data_t vmstat;
    if (host_statistics64(mach_host_self(), HOST_VM_INFO64, (host_info64_t) &vmstat, &count) != KERN_SUCCESS) {
        printf("Failed to get host memory info: %d", errno);
        exit(-1);
    }

    unsigned long long free_count = vmstat.free_count;
    unsigned long long inactive_count = vmstat.inactive_count;

    unsigned long long bytesFree = (free_count + inactive_count) * pagesize;
    unsigned long long bytesTotal = getPhysMemoryBytes();

    double used = (double)(bytesTotal - bytesFree) / (double)bytesTotal;

    return (used * 100.0);
}

static int get_percent_free(unsigned int* level)
{
    int error;

    error = memorystatus_get_level((user_addr_t) level);

    if( error ) {
        printf("memorystatus_get_level failed:");
        exit(-1);
    }
    return error;
}

int main(void)
{
    unsigned int percent_free;
    unsigned int pressure_level;

    get_percent_free(&percent_free);
    get_pressure_level(&pressure_level);
    printf("100-minus-total-vs-free-estimate: %f, memory free percentage: %d, pressure level: %d\n",
            100-total_minus_free_over_total(), percent_free, pressure_level*20);
}

