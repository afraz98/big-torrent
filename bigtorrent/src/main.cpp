#include <stdio.h>
#include <iostream>
#include <bencode.h>

void print_bencode_value(bencode_value *obj) {
    if (!obj) {
        printf("NULL\n");
        return;
    }

    switch (obj->type) {
        case B_ENCODED_INTEGER:
            printf("%lld", obj->integer);
            break;
        case B_ENCODED_STRING:
            printf("%s", obj->string.string);
            break;
        case B_ENCODED_LIST:
            printf("[\n");
            for (int i = 0; i < obj->list.llen; ++i) {
                print_bencode_value(obj->list.list_val[i]);
                printf(",");
            }
            printf("]\n");
            break;
        case B_ENCODED_DICTIONARY:
            printf("{\n");
            for (int i = 0; i < obj->dict.dlen; ++i) {
                print_bencode_value(obj->dict.keys[i]);
                printf(": ");
                print_bencode_value(obj->dict.values[i]);
                printf("\n");
            }
            printf("}\n");
            break;
        default:
            printf("Unknown type\n");
            break;
    }
}

int main(int argc, char* argv[]) {
    // Parse torrent file
    bencode_value *torrent_data = bencode_decode_file("ubuntu-24.04-desktop-amd64.iso.torrent");

   if (torrent_data)
        free_bencode_value(torrent_data);

    return EXIT_SUCCESS;
}