#ifndef ZIP_HEADERS_H_
#define ZIP_HEADERS_H_

#include <stdint.h>
#include <stddef.h>

typedef struct {
	uint16_t disk_num;
	uint16_t cdir_start_disk;
	uint16_t disk_cdir_entries;
	uint16_t cdir_entries_num;
	uint32_t cd_size;
	uint32_t cd_offset;
	uint16_t comment_len;
	const uint8_t *comment;
} eocdr_t;

#define EOCDR_BASE_SIZE 22
#define EOCDR_SIGNATURE 0x06054b50

int check_for_eocdr(const uint8_t* input, eocdr_t* out, size_t size);

#endif /* ZIP_HEADERS_H_ */
