#pragma once

typedef unsigned int natural_t;
typedef unsigned long long uint64_t;

uint64_t vmstat_free_count();
uint64_t vmstat_inactive_count();
uint64_t vmstat_wire_count();
uint64_t vmstat_zero_fill_count();
uint64_t vmstat_reactivations();
uint64_t vmstat_pageins();
uint64_t vmstat_pageouts();
uint64_t vmstat_faults();
uint64_t vmstat_cow_faults();
uint64_t vmstat_lookups();
uint64_t vmstat_hits();
uint64_t vmstat_purges();
uint64_t vmstat_purgable_count();
uint64_t vmstat_speculative_count();
uint64_t vmstat_decompressions();
uint64_t vmstat_compressions();
uint64_t vmstat_swapins();
uint64_t vmstat_swapouts();
natural_t vmstat_compressor_page_count();
natural_t vmstat_throttled_count();
natural_t vmstat_external_page_count();
natural_t vmstat_internal_page_count();
uint64_t vmstat_total_uncompressed_pages_in_compressor();

