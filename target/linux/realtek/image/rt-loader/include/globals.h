/*
 * rt-loader header
 * (c) 2025 Markus Stockhausen
 */

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#define KSEG0			0x80000000
#define STACK_SIZE		0x10000
#define HEAP_SIZE		0x40000
#define MEMORY_ALIGNMENT	32

#define printf(fmt, ...)	npf_pprintf(board_putchar, NULL, fmt, ##__VA_ARGS__)
#define snprintf		npf_snprintf

#endif  // _GLOBALS_H_
