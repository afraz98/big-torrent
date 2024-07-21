#include <stdio.h>
#include <iostream>
#include <bencode.h>
#include "torrent.h"

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cout << "Usage: bigtorrent <torrent-filename>" << std::endl;
        return EXIT_FAILURE;
    }
    
    // Parse torrent file
    std::string filename = "ubuntu-24.04-desktop-amd64.iso.torrent";
    std::cout << "Parsing torrent file: " << argv[1] << std::endl; 

    TorrentInfo torrent_info = TorrentInfo(filename);
    torrent_info.printTorrentData();

    return EXIT_SUCCESS;
}