#include <stdio.h>
#include <errno.h>
#include <mach/mach_host.h>

/*
 * This is what the vm_statistics structure looks like in xnu-2422.1.72 :
 */
//struct vm_statistics64 {
//	natural_t	free_count;		/* # of pages free */
//	natural_t	active_count;		/* # of pages active */
//	natural_t	inactive_count;		/* # of pages inactive */
//	natural_t	wire_count;		/* # of pages wired down */
//	uint64_t	zero_fill_count;	/* # of zero fill pages */
//	uint64_t	reactivations;		/* # of pages reactivated */
//	uint64_t	pageins;		/* # of pageins */
//	uint64_t	pageouts;		/* # of pageouts */
//	uint64_t	faults;			/* # of faults */
//	uint64_t	cow_faults;		/* # of copy-on-writes */
//	uint64_t	lookups;		/* object cache lookups */
//	uint64_t	hits;			/* object cache hits */
//	uint64_t	purges;			/* # of pages purged */
//	natural_t	purgeable_count;	/* # of pages purgeable */
//	/*
//	 * NB: speculative pages are already accounted for in "free_count",
//	 * so "speculative_count" is the number of "free" pages that are
//	 * used to hold data that was read speculatively from disk but
//	 * haven't actually been used by anyone so far.
//	 */
//	natural_t	speculative_count;	/* # of pages speculative */
//
//	/* added for rev1 */
//	uint64_t	decompressions;		/* # of pages decompressed */
//	uint64_t	compressions;		/* # of pages compressed */
//	uint64_t	swapins;		/* # of pages swapped in (via compression segments) */
//	uint64_t	swapouts;		/* # of pages swapped out (via compression segments) */
//	natural_t	compressor_page_count;	/* # of pages used by the compressed pager to hold all the compressed data */
//	natural_t	throttled_count;	/* # of pages throttled */
//	natural_t	external_page_count;	/* # of pages that are file-backed (non-swap) */
//	natural_t	internal_page_count;	/* # of pages that are anonymous */
//	uint64_t	total_uncompressed_pages_in_compressor; /* # of pages (uncompressed) held within the compressor. */
//} __attribute__((aligned(8)));

#define VMSTAT_FAIL_WARN(d) {printf("%s - get_vmstat failed with errno %d", __FUNCTION__, (d));}
#define VMSTAT_FAIL_DIE(d) {VMSTAT_FAIL_WARN(d); abort();}

static int get_vmstat(vm_statistics64_data_t* vmstat)
{
    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
    if (host_statistics64(mach_host_self(), HOST_VM_INFO64, (host_info64_t) vmstat, &count) != KERN_SUCCESS) {
        return errno;
    }
    return 0;
}

static vm_statistics64_data_t _get_vmstat()
{
    vm_statistics64_data_t vmstat;
    int ret = get_vmstat(&vmstat);
    if (ret) {
        VMSTAT_FAIL_WARN(ret);
    }
    return vmstat;
}

unsigned long long vmstat_free_count()
{
    vm_statistics64_data_t vmstat = _get_vmstat();
    return vmstat.free_count;
}

unsigned long long vmstat_inactive_count()
{
    vm_statistics64_data_t vmstat = _get_vmstat();
    return vmstat.inactive_count;
}

//int main(void)
//{
//    printf("A rough thing to just test this\n");
//    printf("%llu\n", vmstat_free_count());
//    printf("%llu\n", vmstat_inactive_count());
//}
