#include <stdio.h>
#include <stdlib.h>

#include "zip_headers.h"

static void print_eocdr_content(const eocdr_t* eocdir) {
    printf("File has ZIP EOCD content:\n\n");
    printf("Number of this disk\t\t\t= %d\n", eocdir->disk_num);
    printf("Number of disk with start of the CD\t= %d\n",
           eocdir->cdir_start_disk);
    printf("Number of CD entries on this disk\t= %d\n",
           eocdir->disk_cdir_entries);
    printf("Number of Central Directory entries\t= %d\n",
           eocdir->cdir_entries_num);
    printf("Central Directory size in bytes\t\t= %d\n", eocdir->cd_size);
    printf("Central Directory file offset\t\t= %d\n", eocdir->cd_offset);
    printf("\nArchive comment has %d bytes length\n", eocdir->comment_len);
    if (eocdir->comment_len) {
        printf("Archive comment:\n\t%s\n", eocdir->comment);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Err: wrong arg num.\n");
        return EXIT_FAILURE;
    }
    int success = EXIT_SUCCESS;

    FILE* file_ptr = fopen(argv[1], "rb");

    fseek(file_ptr, 0, SEEK_END);
    size_t file_size = ftell(file_ptr);
    rewind(file_ptr);

    uint8_t* buffer = (uint8_t*)malloc(file_size);
    do {
        if (buffer == NULL) {
            printf("Err: memory allocate failure.\n");
            success = EXIT_FAILURE;
            break;
        }

        size_t read_size = fread(buffer, 1, file_size, file_ptr);
        if (read_size != file_size) {
            printf("Err: read size incorrect.\n");
            success = EXIT_FAILURE;
            break;
        }

        eocdr_t end_of_cdir;
        if (!check_for_eocdr(buffer, &end_of_cdir, read_size)) {
            printf("No ZIP archive found.\n");
            break;
        }
        print_eocdr_content(&end_of_cdir);

    } while (0);

    fclose(file_ptr);
    free(buffer);
    return success;
}
