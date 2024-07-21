#ifndef TORRENT_H
#define TORRENT_H

#include <string>
#include <vector>
#include <bencode.h>

class TorrentInfo {
    public:
        TorrentInfo(std::string filename);
        void printTorrentData();

    private:
        bencode_value* data;
        std::string filename, announce, author, comment, name;
        std::vector<std::string>  announce_list;
        long long creation_date, length, pieces_length;
        std::vector<unsigned char> pieces;
};

#endif // TORRENT_H