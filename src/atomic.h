/**
*
* @file      atomic.h
* @brief     define CAS operation
* @author    chenggang
* @History
*    1.0.0: chenggang,  2017/06/19,  Initial version
*/

#ifndef _ATOMIC_H
#define _ATOMIC_H


#ifdef __linux__
#define CAS(PTR, OLD, NEW) __sync_bool_compare_and_swap(PTR, OLD, NEW)
#elif __APPLE__
#define CAS(PTR, OLD, NEW) __sync_val_compare_and_swap(PTR, OLD, NEW)
#else
#error Need to define CAS 
#endif

#endif
