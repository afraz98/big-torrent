#ifndef BENCODE_H
#define BENCODE_H

typedef enum {
    B_ENCODED_INTEGER,
    B_ENCODED_STRING,
    B_ENCODED_LIST,
    B_ENCODED_DICTIONARY
} bencode_type;

typedef struct {
    bencode_type type;
    union {
        long long int integer;
        struct {
            int length;
            char* data;
        } string;
    } value;
} bencode_value;

/*!
 * \fn decode
 * \brief Decode b-encoded file
 * @param filename Filename to decode
 */
bencode_value* bencode_decode(char* filename);

/*!
 * \fn bencode_decode_int
 * \brief Decode b-encoded string containing integer
 * @param data B-encoded data containing integer
 * 
 * @return B-encoded value containing b-encoded integer
 */
bencode_value bencode_decode_int(char** data);

/*!
 * \fn bencode_decode_string
 * \brief Decode b-encoded string containing string
 * @param data B-encoded data containing integer
 * 
 * @return B-encoded value containing b-encoded string
 */
bencode_value bencode_decode_string(char** data);

#endif // BENCODE_H