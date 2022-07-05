#include "zip_headers.h"

static int readw_16b_lsb(FILE* f_ptr, uint16_t* halfword) {
    int byte[sizeof(uint16_t)];

    for (size_t i = 0; i < sizeof(uint16_t); i++) {
        byte[i] = fgetc(f_ptr);
        if (byte[i] == EOF) {
            return EOF;
        }
    }
    *halfword = (((uint16_t)byte[1]) << 8) | (uint16_t)(byte[0]);
    return sizeof(uint16_t);
}

static int readw_16b_msb(FILE* f_ptr, uint16_t* halfword) {
    int byte[sizeof(uint16_t)];

    for (size_t i = 0; i < sizeof(uint16_t); i++) {
        byte[i] = fgetc(f_ptr);
    }
    if ((byte[0] == EOF) && (byte[1] == EOF)) {
        return EOF;
    }
    *halfword = (((uint8_t)byte[0]) << 8) | (uint8_t)(byte[1]);
    return sizeof(uint16_t);
}

static int readw_32b_lsb(FILE* f_ptr, uint32_t* word) {
    int byte[sizeof(uint32_t)];

    for (size_t i = 0; i < sizeof(uint32_t); i++) {
        byte[i] = fgetc(f_ptr);
        if (byte[i] == EOF) {
            return EOF;
        }
    }
    *word = (((uint8_t)byte[0]) << 24) | (((uint8_t)byte[1]) << 16) |
            (((uint8_t)byte[2]) << 8) | (uint8_t)byte[3];
    return sizeof(uint32_t);
}

static int check_locf_signature(FILE* f_ptr, int* offset) {
    int byte[sizeof(uint32_t)];

    byte[0] = fgetc(f_ptr);
    if (byte[0] == EOF) {
        return EOF;
    }

    if (byte[0] != LOCF_4_BYTE) {
        *offset += sizeof(uint8_t);
        return 0;
    }

    byte[1] = fgetc(f_ptr);
    if (byte[1] == EOF) {
        return EOF;
    }

    if (byte[1] != LOCF_3_BYTE) {
        *offset += sizeof(uint8_t) * 2;
        return 0;
    }

    byte[2] = fgetc(f_ptr);
    if (byte[2] == EOF) {
        return EOF;
    }

    if (byte[2] != LOCF_2_BYTE) {
        *offset += sizeof(uint8_t) * 3;
        return 0;
    }

    byte[3] = fgetc(f_ptr);
    if (byte[3] == EOF) {
        return EOF;
    }

    if (byte[3] != LOCF_1_BYTE) {
        *offset += sizeof(uint8_t) * 4;
        return 0;
    }

    *offset += sizeof(uint32_t);
    return 1;
}

static void print_field(const char* f_name, const size_t length) {
    for (size_t i = 0; i < length; i++) {
        fputc(f_name[i], stdout);
    }
    fputc((int)('\n'), stdout);
}


int find_jpeg_end(FILE* f_ptr, size_t* f_size) {
    int      jpeg_end  = 0, result = 0;
    int remaining_size = *f_size;
    uint16_t signature = 0, jpeg_start = 0;

    do {
    	result = readw_16b_msb(f_ptr, &signature);
        if (result < 0) {
        	remaining_size -= jpeg_end;
            return EOF;
        }
    	jpeg_end += result;
        if (signature != JPEG_START_SIGN && jpeg_end == 0) {
            return EOF;
        }
        if(!jpeg_start) {
        	jpeg_start = signature;
        }
        if(signature == JPEG_END_SIGN) {
        	break;
        }
    } while (jpeg_end < remaining_size);

    if(jpeg_start != JPEG_START_SIGN && signature != JPEG_END_SIGN) {
    	return EOF;
    } else if(jpeg_start && signature != JPEG_END_SIGN) {
    	return 0;
    }
    *f_size -= jpeg_end;
    return 1;
}

int check_for_cdir(FILE* f_ptr, size_t f_size) {
    int                 offset, result = 0, success = 0;
    size_t              read_size = 0;
    local_file_header_t locf_header;

    while (f_size) {
        offset = 0;
        result = check_locf_signature(f_ptr, &offset);
        if (result == EOF) {
            return success;
        }
        if (result) {
            offset += readw_16b_lsb(f_ptr, &locf_header.version_to_extract);
            offset +=
                readw_16b_lsb(f_ptr, &locf_header.general_purpose_bit_flag);
            offset += readw_16b_lsb(f_ptr, &locf_header.compression_method);
            offset += readw_16b_lsb(f_ptr, &locf_header.modification_time);
            offset += readw_16b_lsb(f_ptr, &locf_header.modification_date);
            offset += readw_32b_lsb(f_ptr, &locf_header.crc32);
            offset += readw_32b_lsb(f_ptr, &locf_header.compressed_size);
            offset += readw_32b_lsb(f_ptr, &locf_header.uncompressed_size);
            offset += readw_16b_lsb(f_ptr, &locf_header.filename_length);
            offset += readw_16b_lsb(f_ptr, &locf_header.extra_field_length);

            if (locf_header.filename_length < BUFFER_SIZE) {
                char msg_buf[locf_header.filename_length];

                read_size = fread(msg_buf, sizeof(uint8_t),
                                  locf_header.filename_length, f_ptr);
                if (!read_size) {
                    break;
                }
                success = 1;
                print_field(msg_buf, read_size);
                offset += read_size;
            }
        }
        f_size -= offset;
    }

    return success;
}
