#include <stdlib.h>
#include <string.h>

#include "bencode.h"

bencode_value* bencode_decode(char* filename) {}

bencode_value bencode_decode_int(char** data) {
    bencode_value retval;
    (*data)++; // Consume 'i' token
    retval.type = B_ENCODED_INTEGER;
    retval.value.integer = atoll(*data);
    (*data)++; // Consume 'e' token
    return retval;
}

bencode_value bencode_decode_string(char** data) {
    bencode_value retval;
    retval.type = B_ENCODED_STRING;
    int length = atoi(data);
    while(**data != ':')
        (*data)++;

    retval.value.string.data = (char *) malloc(length + 1);
    strncpy(retval.value.string.data, *data + 1, length);
    retval.value.string.data[length] = '\0';
    retval.value.string.length = length;
    *data += length + 1;

    return retval;
}