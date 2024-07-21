#ifndef BENCODE_H
#define BENCODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    B_ENCODED_INTEGER = 0,
    B_ENCODED_STRING,
    B_ENCODED_LIST,
    B_ENCODED_DICTIONARY
} bencode_type;

typedef struct bencode_value_struct {
    bencode_type type;
    union {
        long long integer;
        struct {
            char* string;
            int strlen;
        } string;
        
        struct {
            struct bencode_value_struct** list_val;
            int llen;
        } list;
        
        struct {
            struct bencode_value_struct** keys;
            struct bencode_value_struct** values;
            int dlen;
        } dict;
    };
} bencode_value;

/*!
 * \fn decode
 * \brief Decode b-encoded file
 * @param filename Filename to decode
 */
bencode_value* bencode_decode_file(const char* filename);

/*!
 * \fn bencode_decode_int
 * \brief Decode b-encoded string containing integer
 * @param fp Pointer to file object containing bencoded data
 * 
 * @return B-encoded value containing b-encoded integer
 */
bencode_value bencode_decode_int(FILE* fp);

/*!
 * \fn bencode_decode_string
 * \brief Decode b-encoded string containing string
 * @param fp Pointer to file object containing bencoded data
 * 
 * @return B-encoded value containing b-encoded string
 */
bencode_value bencode_decode_string(FILE* fp);

bencode_value *parse_list(FILE* fp);

bencode_value *parse_dict(FILE* fp);

bencode_value* parse_object(FILE* fp);

void free_bencode_value(bencode_value* obj);

#ifdef __cplusplus
}
#endif

#endif // BENCODE_H