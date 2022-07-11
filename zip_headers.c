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

static int readw_32b_lsb(FILE* f_ptr, uint32_t* word) {
    int byte[sizeof(uint32_t)];

    for (size_t i = 0; i < sizeof(uint32_t); i++) {
        byte[i] = fgetc(f_ptr);
        if (byte[i] == EOF) {
            return EOF;
        }
    }
    *word = (((uint32_t)byte[0]) << 24) | (((uint32_t)byte[1]) << 16) |
            (((uint32_t)byte[2]) << 8) | (uint32_t)byte[3];
    return sizeof(uint32_t);
}

static int check_byte(FILE* f_ptr, uint8_t* byte_out, uint8_t signature,
                      int* offset) {
    int byte = fgetc(f_ptr);
    *offset += 1;

    if (byte == EOF) {
        return EOF;
    }
    *byte_out = (uint8_t)byte;
    if ((uint8_t)byte != signature) {
        return 0;
    }
    return 1;
}

static int check_locf_signature(FILE* f_ptr, int* offset) {
    int     result   = -1;
    uint8_t bytes[4] = {0}, read_byte = 0;

    while (*((uint32_t*)bytes) != LOCF_SIGNATURE) {
        do {
            if (read_byte != LOCF_4_BYTE) {
                result = check_byte(f_ptr, &read_byte, LOCF_4_BYTE, offset);
                if (result == EOF) {
                    return EOF;
                } else if (result) {
                    bytes[0] = read_byte;
                }
            }
        } while (read_byte != LOCF_4_BYTE);

        result = check_byte(f_ptr, &read_byte, LOCF_3_BYTE, offset);
        if (result == EOF) {
            break;
        }
        if (read_byte == LOCF_4_BYTE || result == 0) {
            (void)result;
            continue;
        }
        bytes[1] = read_byte;


        result = check_byte(f_ptr, &read_byte, LOCF_2_BYTE, offset);
        if (result == EOF) {
            break;
        }
        if (result == LOCF_4_BYTE || result == 0) {
            (void)result;
            continue;
        }
        bytes[2] = read_byte;

        result = check_byte(f_ptr, &read_byte, LOCF_1_BYTE, offset);
        if (result == EOF) {
            break;
        }
        if (result == LOCF_4_BYTE || result == 0) {
            (void)result;
            continue;
        }
        bytes[3] = read_byte;
    }

    return result;
}

static uint32_t field_count = 0;

static void print_field(const char* f_name, const size_t length) {
    printf("%u:\t", ++field_count);
    for (size_t i = 0; i < length; i++) {
        fputc(f_name[i], stdout);
    }
    fputc((int)('\n'), stdout);
}


int find_jpeg_end(FILE* f_ptr, size_t* f_size) {
    int     jpeg_end = 0, result = 0;
    uint8_t byte = 0;

    do {
        result = check_byte(f_ptr, &byte, JPEG_FIRST_START_END_BYTE, &jpeg_end);
        if (result < 0) {
            return 0;
        }
        if (!result) {
            continue;
        }
        result = check_byte(f_ptr, &byte, JPEG_SECOND_START_BYTE, &jpeg_end);
        if (result < 0) {
            return 0;
        }
    } while (!result);

    do {
        result = check_byte(f_ptr, &byte, JPEG_FIRST_START_END_BYTE, &jpeg_end);
        if (result < 0) {
            return 0;
        }
        if (!result) {
            continue;
        }
        result = check_byte(f_ptr, &byte, JPEG_SECOND_END_BYTE, &jpeg_end);
        if (result < 0) {
            return 0;
        }
    } while (!result);

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
