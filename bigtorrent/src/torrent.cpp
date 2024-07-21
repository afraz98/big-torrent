#include <bencode.h>
#include <iostream>

#include "torrent.h"

TorrentInfo::TorrentInfo(std::string filename) : 
filename(filename), 
name(""),
creation_date(0),
length(0), 
pieces_length(0) {
    bencode_value* torrent_data = bencode_decode_file(filename.c_str());

   if (!torrent_data)
        throw std::runtime_error("Error parsing torrent file");

    for(int i = 0; i < torrent_data->dict.dlen; i++) {
        bencode_value* key = torrent_data->dict.keys[i];
        if(key->type == B_ENCODED_STRING) {
            if(strcmp(key->string.string, "announce") == 0)
                announce = std::string(torrent_data->dict.values[i]->string.string);
            
            if(strcmp(key->string.string, "announce-list") == 0) {
                for(int j = 0; j < torrent_data->dict.values[i]->list.llen; j++) {
                    announce_list.push_back (
                        std::string(torrent_data->dict.values[i]->list.list_val[j]->list.list_val[0]->string.string));
                }
            }

            if(strcmp(key->string.string, "comment") == 0)
                comment = std::string(torrent_data->dict.values[i]->string.string);

            if(strcmp(key->string.string, "created by") == 0)
                author = std::string(torrent_data->dict.values[i]->string.string);

            if(strcmp(key->string.string, "creation date") == 0)
                creation_date = torrent_data->dict.values[i]->integer;

            if(strcmp(key->string.string, "info") == 0) {
                for(int k = 0; k < torrent_data->dict.values[i]->dict.dlen; k++){
                    // Parse dictionary fields for 'info' dictionary
                    bencode_value* key = torrent_data->dict.values[i]->dict.keys[k];
                    if(key->type == B_ENCODED_STRING) {
                        if(strcmp(key->string.string, "name") == 0)
                            name = std::string(torrent_data->dict.values[i]->dict.values[k]->string.string);
                        if (strcmp(key->string.string, "length") == 0)
                            length = torrent_data->dict.values[i]->dict.values[k]->integer;
                        if(strcmp(key->string.string, "piece length") == 0)
                            pieces_length = torrent_data->dict.values[i]->dict.values[k]->integer;
                        if(strcmp(key->string.string, "pieces") == 0) {
                            unsigned char* piece = (unsigned char *)  torrent_data->dict.values[i]->dict.values[k]->string.string;
                            for(int l = 0; l < pieces_length; l++) {
                                pieces.push_back(*piece);
                                piece++;
                            }
                        }
                    }
                }
            }
        }
    }
}

void TorrentInfo::printTorrentData() {
    std::cout << "======= Torrent data ========" << std::endl;
    std::cout << "announce :: " << announce << std::endl;
    
    std::cout << "announce-list :: {" << std::endl;
    for(auto ann : announce_list)
        std::cout << "\t" << ann << "," << std::endl;
    std::cout << "}" << std::endl;
    
    std::cout << "author :: " << author << std::endl;
    std::cout << "comment :: " << comment << std::endl;
    std::cout << "creation date :: " << creation_date << std::endl;
    std::cout << "name :: " << name << std::endl;
    std::cout << "length :: " << length << std::endl;
    std::cout << "pieces length :: " << pieces_length << std::endl;

    for(int i = 0; i < pieces_length; i++) {
        std::cout << std::hex << (int) pieces.at(i) << "\t";
        if(i % 8 == 0) std::cout << std::endl;
    }
}