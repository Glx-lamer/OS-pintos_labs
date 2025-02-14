#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/thread.h"
#include "threads/malloc.h"
#include "threads/palloc.h"

void 
test_max_mem_malloc ()
{
    int64_t iteracts = 0;
    char *used_mem = (char *) malloc(256 * sizeof(char));
    for(; used_mem; iteracts++)
    {
        used_mem = (char *) malloc(256 * sizeof(char));
    }
    msg("\n>>> %d times mem can be allocated with malloc()", iteracts);
}

void 
test_max_mem_calloc ()
{
    int64_t iteracts = 0;
    int *used_mem = (int *) calloc(128, sizeof(int));
    for(; used_mem; iteracts++)
    {
        used_mem = (int *) calloc(128, sizeof(int));
    }
    msg("\n>>> %d times mem can be allocated with calloc()", iteracts);
}

void 
test_max_mem_palloc ()
{
    int64_t iteracts = 0;
    char *used_mem = (char *) palloc_get_page(PAL_ZERO);
    for(; used_mem; iteracts++)
    {
        used_mem = (char *) palloc_get_page(PAL_ZERO);
    }
    msg("\n>>> %d times mem can be allocated with palloc()", iteracts);
}