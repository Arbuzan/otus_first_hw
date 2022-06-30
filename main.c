#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include "zip_headers.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("No argument provided.\n");
        return EXIT_FAILURE;
    }

    struct stat file_attributes;
    if(stat(argv[1], &file_attributes) == -1){
    	perror("File opening error");
    	return EXIT_FAILURE;
    }

    if(!S_ISREG(file_attributes.st_mode)){
    	printf("Not a regular file.\n");
    	return EXIT_FAILURE;
    }

    FILE* file_ptr = fopen(argv[1], "rb");
    int success = EXIT_SUCCESS;
    size_t file_size = file_attributes.st_size;
    size_t jpeg_end_pos = 0;

    uint8_t* buffer = (uint8_t*)malloc(BUFFER_SIZE);

    do {
        if (buffer == NULL) {
            printf("Memory allocate failure.\n");
            success = EXIT_FAILURE;
            break;
        }

        jpeg_end_pos = find_jpeg_end(buffer, file_ptr, &file_size);
        if(!jpeg_end_pos) {
        	printf("File has no jpeg signature.\n");
        	success = EXIT_FAILURE;
        	break;
        }

        success = check_for_cdir(buffer, jpeg_end_pos, file_ptr, file_size);

//        eocdr_t end_of_cdir;
//        if (!check_for_eocdr(buffer, &end_of_cdir, read_size)) {
//            printf("No ZIP archive found.\n");
//            break;
//        }
//        print_eocdr_content(&end_of_cdir);

    } while (0);

    fclose(file_ptr);
    free(buffer);
    return success;
}
