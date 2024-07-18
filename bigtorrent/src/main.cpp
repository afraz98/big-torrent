#include <stdio.h>
#include <iostream>
#include <bencode.h>

int main() {
    char* bencoded_str = "i42e";
    bencode_value val = {};
    
    val = bencode_decode_int(&bencoded_str);
    std::cout << val.value.integer << std::endl;
    return 0;
}