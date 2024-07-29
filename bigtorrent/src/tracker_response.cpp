#include <bencode.h>
#include <iostream>
#include <cstring>
#include <string>
#include <vector>

#include <arpa/inet.h>  // For inet_ntop
#include "tracker_response.h"

namespace Tracker {
    TrackerResponse::TrackerResponse(std::string response) : 
    interval(0), min_interval(0), seeders(0), leechers(0), peers(0) {
        raw_tracker_response = bencode_decode_str(response.c_str());

        // Parse raw tracker response
        if(raw_tracker_response->type == B_ENCODED_DICTIONARY) {
            for(int i = 0; i < raw_tracker_response->dict.dlen; i++) {
                bencode_value* key = raw_tracker_response->dict.keys[i];
                if(key->type == B_ENCODED_STRING) {
                    if(strcmp(key->string.string, "interval") == 0) {
                        interval = raw_tracker_response->dict.values[i]->integer;
                        std::cout << "interval :: " << interval << std::endl;
                    } else if(strcmp(key->string.string, "min interval") == 0) {
                        min_interval = raw_tracker_response->dict.values[i]->integer;
                        std::cout << "min interval :: " << min_interval << std::endl;
                    } else if(strcmp(key->string.string, "complete") == 0) {
                        seeders = raw_tracker_response->dict.values[i]->integer;
                        std::cout << "seeders :: " << seeders << std::endl;
                    } else if(strcmp(key->string.string, "downloaded") == 0) {
                        downloaded = raw_tracker_response->dict.values[i]->integer;
                        std::cout << "downloaded :: " << downloaded << std::endl;
                    } else if(strcmp(key->string.string, "incomplete") == 0) {
                        leechers = raw_tracker_response->dict.values[i]->integer;
                        std::cout << "leechers :: " << leechers << std::endl; 
                    } else if(strcmp(key->string.string, "peers") == 0) {
                        parsePeers(raw_tracker_response->string.string);
                    }
                }
            }
        }
    }

    void TrackerResponse::parsePeers(std::string peers){
        for (size_t i = 0; i < peers.size(); i += 6) {
            char ip_buffer[INET_ADDRSTRLEN];
            uint32_t ip;
            std::memcpy(&ip, peers.data() + i, 4);
            inet_ntop(AF_INET, &ip, ip_buffer, INET_ADDRSTRLEN);

            uint16_t port;
            std::memcpy(&port, peers.data() + i + 4, 2);
            port = ntohs(port);  // Convert network byte order to host byte order
            peer_list.push_back(Peer{ip_buffer, port});
        }
    }
} // namespace Tracker