#include <stdio.h>
#include <stdlib.h>
#include <jemalloc/jemalloc.h>

// Using jemalloc-master from github
//
// gcc jemalloc_test1.c -ljemalloc
//
// MALLOC_CONF=prof_leak:true,lg_prof_sample:0,prof_final:true LD_PRELOAD=/usr/local/lib/libjemalloc.so.2 ./a.out
//
// jeprof --show_bytes --pdf a.out jeprof.689.1.f.heap > a.pdf

void do_something(size_t i)
{
	// Leak some memory
	malloc(i * 4);
}

void do_something_else(size_t i)
{
	// Leak some memory
	malloc(i * 4);
}

int main(int argc, char **argv)
{
	size_t i, sz;

	for (i = 0; i < 100; i++)
	{
		do_something(i);
	}

	bool active = true;
	//mallctl("prof.dump", NULL, NULL, NULL, 0);
	mallctl("prof.active", NULL, NULL, &active, sizeof(bool));

	for (i = 0; i < 10; i++)
	{
		do_something_else(i);
	}
	mallctl("prof.dump", NULL, NULL, NULL, 0);

	return 0;
}
