#include <stdio.h>
#include <stdlib.h>
#include <argtable2.h>
#include "bmp.h"
#include "encript.h"
#include "main.h"

int
main(int argc, char **argv)
{
    /* Mandatory parameters */
    struct arg_lit  * distribute = arg_lit0("d", "distribute", "Distribute the secret image");
    struct arg_lit  * recover = arg_lit0("r", "recover", "Recover a secret image");
    struct arg_file * in  = arg_file0("s", "secret", "<input>", "File to distribute/recover");
    struct arg_int * k  = arg_int0("k", NULL, "<int>", "Necessary shadows to reveal the secret");

    /* Optional parameters */
    struct arg_str * dir = arg_str0(NULL, "dir", "<string>", "Directory to use for distributing/recovering a secret");
    struct arg_int * n = arg_int0("n", NULL, "<int>", "Number of shadows to generate when distributing");

    struct arg_end * end = arg_end(10);

    void * argtable[] = {distribute, recover, in, k, dir, n, end};
    int errors;

    // Parse the arguments and validate them
    errors = arg_parse(argc, argv, argtable);

    /* Validate required parameters */
    if ((distribute->count == 0 && recover->count == 0) || in->count == 0 || k->count == 0) {
        printf("Please input all the required parameters (either '-d' or '-r', -s <file>, -k <int>)\n");
        errors++;
    }

    /* Validate flags */
    if (distribute->count > 0 && recover->count > 0) {
        printf("Cannot use distribute and recover in the same run\n");
        errors++;
    }

    /* Validate k value */
    if (k->ival[0] < 2 || k->ival[0] > 3) {
        printf("k must be in the range [2, 3]\n");
        errors++;
    }

    /* Validate n value if set */
    if (n->count > 0) {
        if (n->ival[0] < 3 || n->ival[0] > 8) {
            printf("n must be in the range [3, 8]\n");
            errors++;
        }
    }

    // TODO: Validate image is a bmp file.

    if (errors > 0) {
        arg_print_errors(stdout, end, argv[0]);
        printf("Wrong parameter values\n");
        return 1;
    }
    
    if (recover->count > 0) {
        image_t * secret_image = recovery(dir->sval[0], k->ival[0]);
        write_bitmap_file(secret_image);
        //print_matrix(secret_image);
    }

    return 0;
}


