#include "zip_headers.h"

static const uint8_t* readw_16b(const uint8_t* p_in, uint16_t* p_out) {
    *p_out = *(uint16_t*)(p_in);
    return p_in + sizeof(uint16_t);
}

static const uint8_t* readw_32b(const uint8_t* p_in, uint32_t* p_out) {
    *p_out = *(uint32_t*)(p_in);
    return p_in + sizeof(uint32_t);
}

int check_for_eocdr(const uint8_t* input, eocdr_t* out, size_t size) {
    const uint8_t* p = input + size - EOCDR_BASE_SIZE;

    for (size_t comment_len = 0; comment_len <= UINT16_MAX; comment_len++) {
        if (size < EOCDR_BASE_SIZE + comment_len) {
            break;
        }


        if (*(uint32_t*)p == EOCDR_SIGNATURE) {
            p += sizeof(uint32_t);
            p = readw_16b(p, &out->disk_num);
            p = readw_16b(p, &out->cdir_start_disk);
            p = readw_16b(p, &out->disk_cdir_entries);
            p = readw_16b(p, &out->cdir_entries_num);
            p = readw_32b(p, &out->cd_size);
            p = readw_32b(p, &out->cd_offset);
            p = readw_16b(p, &out->comment_len);

            out->comment = p;

            if (out->comment_len == comment_len) {
                return 1;
            }
        }
        p++;
    }

    return 0;
}
