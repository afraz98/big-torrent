#include <bencode.h>
#include <iostream>
#include <openssl/sha.h>

#include "torrent.h"

namespace Torrent {

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
                    info_dict = torrent_data->dict.values[i];

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

        // Compute SHA1 hash of info dict
        computeSHA1(info_dict, info_hash);
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

    std::string generatePeerID(const char *client_id, const char *client_version) {
        char peer_id[21];

        // Seed the random number generator
        srand(static_cast<unsigned int>(time(NULL)));

        // Format the client identifier part
        snprintf(peer_id, 9, "-%2s%04d-", client_id, atoi(client_version));

        // Generate the random part
        for (int i = 8; i < 20; i++)
            peer_id[i] = '0' + (rand() % 10);
        peer_id[20] = '\0';
        return std::string(peer_id);
    }

    void computeSHA1(bencode_value *info_dict, unsigned char *output_hash) {
        char buffer[10000];  // Adjust size as needed
        size_t offset = 0;
        memset(buffer, 0, sizeof(buffer));
        
        bencode_encode(info_dict, buffer, &offset);
        SHA1((unsigned char *) buffer, 0, output_hash);
    }
} // namespace Torrent