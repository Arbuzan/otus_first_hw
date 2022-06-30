#include "zip_headers.h"

static const uint8_t* readw_16b(const uint8_t* p_in, uint16_t* p_out) {
    *p_out = *(uint16_t*)(p_in);
    return p_in + sizeof(uint16_t);
}

static const uint8_t* readw_32b(const uint8_t* p_in, uint32_t* p_out) {
    *p_out = *(uint32_t*)(p_in);
    return p_in + sizeof(uint32_t);
}

static void print_field(const uint8_t* f_name, const uint16_t length) {
	for(uint16_t i = 0; i < length; i++) {
		printf("%c", f_name[i]);
	}
	printf("\n");
}

size_t find_jpeg_end(uint8_t* buf, FILE* f_ptr, size_t* f_size) {
	size_t read_size = 0, jpeg_end = 0;
	uint16_t signature = 0;

    while(f_size) {
		read_size = fread(buf, sizeof(uint8_t), BUFFER_SIZE, f_ptr);
		for(size_t pos = 1; pos < read_size; pos++) {
			signature = (buf[pos-1] << 8) | (buf[pos]);
			if(signature == JPEG_START_SIGN) {
				continue;
			}
			if(signature == JPEG_END_SIGN) {
				jpeg_end = pos * sizeof(uint8_t);
				break;
			}
		}
		if(signature == JPEG_END_SIGN) {
			*f_size -= jpeg_end;
			return jpeg_end + 1;
		}
		*f_size -= read_size;
    }
    return 0;
}

int check_for_cdir(uint8_t* buf, size_t buf_start_pos, FILE* f_ptr, size_t f_size) {
	int fail = 1;
	size_t read_size = BUFFER_SIZE;
	uint32_t signature = 0;
	local_file_header_t locf_header;
	const uint8_t* p = buf + buf_start_pos;

	while(f_size) {
		for(size_t pos = buf_start_pos; pos < read_size; pos++) {
			p = readw_32b(p, &signature);
			if(signature == LOCF_SIGNATURE) {
				p = readw_16b(p, &locf_header.version_to_extract);
				p = readw_16b(p, &locf_header.general_purpose_bit_flag);
				p = readw_16b(p, &locf_header.compression_method);
				p = readw_16b(p, &locf_header.modification_time);
				p = readw_16b(p, &locf_header.modification_date);
				p = readw_32b(p, &locf_header.crc32);
				p = readw_32b(p, &locf_header.compressed_size);
				p = readw_32b(p, &locf_header.uncompressed_size);
				p = readw_16b(p, &locf_header.filename_length);
				p = readw_16b(p, &locf_header.extra_field_length);
				locf_header.filename = p++;
				locf_header.extra_field = p++;
				print_field(locf_header.filename, locf_header.filename_length);
				fail = 0;
			}
		}
		f_size -= read_size - buf_start_pos;
		p = buf;
		read_size = fread(buf, sizeof(uint8_t), BUFFER_SIZE, f_ptr);
		if(!read_size && !fail) {
			printf("ZIP archive found. File list above ^\n");
			break;
		} else if(!read_size) {
			printf("No ZIP archive found\n");
			break;
		}
		if(buf_start_pos) {
			buf_start_pos = 0;
		}
	}
	return fail;
}
