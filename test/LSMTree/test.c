/*************************************************************************
	> File Name: test.c
	> Author: 
	> Mail: 
	> Created Time: 四  7/ 9 09:01:18 2020
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <assert.h>

#include "lsm.h"
#include "test.h"


/*
 * gcc -ggdb -g -O0 test.c lsm.c -o lsm
 */

/*
 * Function: Given a pointer to a lsm object, print tree.
 */
void test_print_tree(lsm *tree)
{
    struct stat s;
    if (stat(tree->disk1, &s))
    {
        perror("print: fstat \n");
    }

    if (s.st_size == 0  && tree->next_empty != 0)
    {
        printf("data fits in the buffer\n");
        print_buffer_data(tree);
    }
    if (s.st_size > 0  && tree->next_empty == 0)
    {
        printf("data fits in the disk\n");
        print_disk_data(tree);
    }
    if (s.st_size > 0  && tree->next_empty != 0)
    {
        printf("data is in buffer & on disk\n");
        print_buffer_data(tree);
        print_disk_data(tree);
    }
}

/*
 * Function: Put data_size items into the lsm tree object.
 * Args:
 *     tree: pointer to the lsm object.
 *     data_size: int specifying how much data to add.
 *     buffer_size: int specifying in-memory buffer size.
 *     sorted: bool if true, data will be sorted on disk.
 */
int test_put(lsm *tree, int data_size, int buffer_size, bool sorted, bool timing)
{
    srand(0);
    int r = 0;
    clock_t start, end;
    start = clock();
    for (int i = 0; i < data_size; ++i)
    {
        keyType k;
        valType v;
        k = (keyType) i;
        v = (valType) i;  // usually this should be rand
        r = put(&k, &v, tree);
        assert(r == 0);
    }
    end = clock();
    if (timing)
    {
        double time_elapsed = (double) (end - start) / CLOCKS_PER_SEC;
        printf("%f, \n", time_elapsed);
    }

    test_print_tree(tree);

    return r;
}

int main(int argc, char *argv[])
{
    printf("Usage: %s data_size buffer_size nops put/get/upd/thr [sorted] [put_prob] [update_prob]\n", argv[0]);
    assert(argc >= 5);
    clock_t start, end;

    int r = 0;
    int data_size = atoi(argv[1]);
    int buffer_size = atoi(argv[2]);
    int nops = atoi(argv[3]);
    char testing[4];
    strncpy(testing, argv[4], 3);
    bool sorted = false;
    if (argc == 6)
    {
        sorted = argv[5];
    }
    //set probabilities for throughput.
    float put_prob = 33.0;
    float update_prob = 33.0;
    if (argc == 8)
    {
        put_prob = atoi(argv[6]);
        update_prob = atoi(argv[7]);
    }

    lsm *tree;
    tree = init_new_lsm(buffer_size, sorted);

    if (strcmp(testing, "put") == 0)
    {
        // TEST PUT
        r = test_put(tree, data_size, buffer_size, sorted, true);
    }


}
