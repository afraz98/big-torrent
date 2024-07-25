#ifndef TORRENT_H
#define TORRENT_H

#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <string>
#include <vector>
#include <bencode.h>

namespace Torrent {

    class TorrentInfo {
        public:
            TorrentInfo(std::string filename, bool verbose);
            void printTorrentData();
            inline std::string getAnnounce() { return announce; }
            inline long long getPiecesLength() { return pieces_length; }
            inline unsigned char* getInfoHash() { return info_hash; }
            inline long long getFileLength() { return length; }
        private:
            bencode_value* data, *info_dict;
            std::string filename, announce, author, comment, name;
            std::vector<std::string>  announce_list;
            long long creation_date, length, pieces_length;
            std::vector<unsigned char> pieces;
            unsigned char info_hash[SHA_DIGEST_LENGTH];
            bool verbose;
    };

    std::string generatePeerID(const char* client_id, const char* client_version);

    void computeSHA1(bencode_value *info_dict, unsigned char *output_hash);

}

#endif // TORRENT_H