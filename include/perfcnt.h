/* Reading/Configuring Performance Counters on IA32 Architecture */

#ifndef __PERFCNT_H__
#define __PERFCNT_H__

#include <types.h>

// Pentium Processor Performance Monitoring Events //
// see IA32 Reference Manual Vol.3B Appendix A     //

#define DATA_READ				0x00 
#define DATA_WRITE				0x01
#define DATA_TLB_MISS				0x02
#define DATA_READ_MISS				0x03
#define DATA_WRITE_MISS				0x04
#define WRITE_HIT_TO_M_OR_E_STATE_LINES		0x05
#define DATA_CACHE_LINES_WRITTEN_BACK		0x06
#define EXTERNAL_SNOOPS				0x07
#define EXTERNAL_DATA_CACHE_SNOOP_HITS		0x08
#define MEMORY_ACCESSES_IN_BOTH_PIPES		0x09
#define BANK_CONFLICTS				0x0A
#define MISALIGNED_DATA_MEMORY_OR_IO_REFERENCES	0x0B
#define CODE_READ				0x0C
#define CODE_TLB_MISS				0x0D
#define CODE_CACHE_MISS				0x0E
#define ANY_SEGMENT_REGISTER_LOADED		0x0F
#define BRANCHES				0x12
#define BTB_HITS				0x13
#define TAKEN_BRANCH_OR_BTB_HIT			0x14
#define PIPELINE_FLUSHES			0x15
#define INSTRUCTIONS_EXECUTED			0x16
#define INSTRUCTIONS_EXECUTED_V_PIPE		0x17
#define BUS_CYCLE_DURATION			0x18
#define WRITE_BUFFER_FULL_STALL_DURATION	0x19
#define WAITING_FOR_DATA_MEMORY_READ_STALL_DUR	0x1A
#define STALL_ON_WRITE_TO_AN_E_OR_M_STATE_LINE	0x1B
#define LOCKED_BUS_CYCLE			0x1C
#define IO_READ_OR_WRITE_CYCLE			0x1D
#define NONCACHEABLE_MEMORY_READS		0x1E
#define PIPELINE_AGI_STALLS			0x1F
#define FLOPS					0x22
#define BREAKPOINT_MATCH_ON_DR0			0x23
#define BREAKPOINT_MATCH_ON_DR1			0x24
#define BREAKPOINT_MATCH_ON_DR2			0x25
#define BREAKPOINT_MATCH_ON_DR3			0x26
#define HARDWARE_INTERRUPTS			0x27
#define DATA_READ_OR_WRITE			0x28
#define DATA_READ_MISS_OR_WRITE_MISS		0x29

#define PERF_EVT_SEL0		0x186
#define PERF_EVT_SEL1		0x187
#define PERF_CTR0		0xC1
#define PERF_CTR1		0xC2

#define AMD_PERF_EVT_SEL	0xC0010000
#define AMD_PERF_CTR		0xC0010004

void perf_evtsel(uint8 id, uint8 evt);
void perf_cntenable(uint8 id);
uint64 perf_read_cnt(uint8 id);

bool cpuinfo(void);
void perfmon(void);

#endif 
