#include <bencode.h>
#include <iostream>
#include <openssl/sha.h>

#include "tracker.h"
#include "tracker_response.h"

namespace Torrent {
    Tracker::Tracker(std::string filename, bool verbose) : 
    filename(filename), 
    verbose(verbose),
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
                    for(int j = 0; j < torrent_data->dict.values[i]->list.llen; j++)
                        announce_list.push_back (std::string(torrent_data->
                                dict.values[i]->list.list_val[j]->list.list_val[0]->string.string));
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
                            if(strcmp(key->string.string, "files") == 0) {
                                // Get file length of all files
                                for(int m = 0; m < torrent_data->dict.values[i]->dict.values[k]->list.llen; m++){
                                    bencode_value* file = torrent_data->dict.values[i]->dict.values[k]->list.list_val[m];
                                    for(int n = 0; n < file->dict.dlen; n++){
                                        bencode_value* key = file->dict.keys[n];
                                        if(key->type == B_ENCODED_STRING) {
                                            if(strcmp(key->string.string, "length")) {
                                                length += file->dict.values[n]->integer;
                                            }
                                        }
                                    }
                                }
                            }

                            if(strcmp(key->string.string, "name") == 0)
                                name = std::string(torrent_data->dict.values[i]->dict.values[k]->string.string);
                            if (strcmp(key->string.string, "length") == 0)
                                length = torrent_data->dict.values[i]->dict.values[k]->integer;
                            if(strcmp(key->string.string, "piece length") == 0)
                                pieces_length = torrent_data->dict.values[i]->dict.values[k]->integer;
                        }
                    }
                }
            }
        }

        // Compute SHA1 hash of info dict
        computeSHA1(info_dict, info_hash);

        // Generate peer ID
        peer_id = generatePeerID("MY", "1000");
    }

    void Tracker::printTrackerData() {
        std::cout << "announce :: " << announce << std::endl;
        
        std::cout << "announce-list :: [\n";
        for(auto ann : announce_list)
            std::cout << "\t" << ann << std::endl;
        std::cout << "]" << std::endl;
        
        std::cout << "author :: " << author << std::endl;
        std::cout << "comment :: " << comment << std::endl;
        std::cout << "creation date :: " << creation_date << std::endl;
        std::cout << "name :: " << name << std::endl;
        std::cout << "length :: " << length << std::endl;
        std::cout << "pieces length :: " << pieces_length << std::endl;
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

    void computeSHA1(bencode_value *info_dict, std::vector<unsigned char> output_hash) {
        // Ensure the output vector has the correct size (SHA_DIGEST_LENGTH = 20 bytes)
        output_hash.resize(SHA_DIGEST_LENGTH);

        // Calculate the SHA1 hash
        SHA1(reinterpret_cast<unsigned char*>(info_dict), 0, output_hash.data());
    }
} // namespace Tracker