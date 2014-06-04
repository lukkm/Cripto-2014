#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int
main(int argc, char **argv)
{
    int i, j;

    if (argc < 2) {
        printf("Usage: %s filename.\n", argv[0]);
        return 1;
    }

    FILE * f = fopen(argv[1], 'r');

    struct stat buf;
    fstat(f, &buf);
    printf("Size: %d\n", buf.st_size);

    return 0;
}
