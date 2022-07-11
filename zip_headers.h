#ifndef ZIP_HEADERS_H_
#define ZIP_HEADERS_H_

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define BUFFER_SIZE               1024U
#define JPEG_FIRST_START_END_BYTE 0xFFU
#define JPEG_SECOND_START_BYTE    0xD8U
#define JPEG_SECOND_END_BYTE      0xD9U

// Обязательная сигнатура, равна 0x04034b50
#define LOCF_SIGNATURE 0x04034b50U
#define LOCF_1_BYTE    0x04U
#define LOCF_2_BYTE    0x03U
#define LOCF_3_BYTE    0x4BU
#define LOCF_4_BYTE    0x50U


typedef struct {
    // Минимальная версия для распаковки
    uint16_t version_to_extract;
    // Битовый флаг
    uint16_t general_purpose_bit_flag;
    // Метод сжатия (0 - без сжатия, 8 - deflate)
    uint16_t compression_method;
    // Время модификации файла
    uint16_t modification_time;
    // Дата модификации файла
    uint16_t modification_date;
    // Контрольная сумма
    uint32_t crc32;
    // Сжатый размер
    uint32_t compressed_size;
    // Несжатый размер
    uint32_t uncompressed_size;
    // Длина название файла
    uint16_t filename_length;
    // Длина поля с дополнительными данными
    uint16_t extra_field_length;
    // Название файла (размером filenameLength)
    const uint8_t* filename;
    // Дополнительные данные (размером extraFieldLength)
    const uint8_t* extra_field;
} local_file_header_t;

int check_for_cdir(FILE* f_ptr, size_t size);
int find_jpeg_end(FILE* f_ptr, size_t* f_size);

#endif /* ZIP_HEADERS_H_ */
