#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "zip_headers.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("No argument provided.\n");
        return EXIT_FAILURE;
    }

    struct stat file_attributes;
    if (stat(argv[1], &file_attributes) == -1) {
        perror("File opening error");
        return EXIT_FAILURE;
    }

    if (!S_ISREG(file_attributes.st_mode)) {
        printf("Not a regular file.\n");
        return EXIT_FAILURE;
    }

    FILE*  file_ptr  = fopen(argv[1], "rb");
    int    success   = EXIT_SUCCESS;
    size_t file_size = file_attributes.st_size;

    do {
        if (!find_jpeg_end(file_ptr, &file_size)) {
            printf("File has no jpeg signature.\n");
            success = EXIT_FAILURE;
            break;
        }

        success = check_for_cdir(file_ptr, file_size);
        if (success) {
            printf("ZIP archive found. File list above ^\n");
        } else {
            printf("No ZIP archive found\n");
        }

    } while (0);

    fclose(file_ptr);
    return success;
}
