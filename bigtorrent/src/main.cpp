#include <curl/curl.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

#include <bencode.h>
#include "tracker.h"
#include "tracker_response.h"

size_t write_callback(void *contents, size_t size, size_t nmemb, std::string *s) {
    size_t total_size = size * nmemb;
    s->append((char *) contents, total_size);
    return total_size;
}

std::string url_encode(const unsigned char* s, size_t len) {
    std::ostringstream encoded;
    for (size_t i = 0; i < len; ++i) {
        if (isalnum(s[i]) || s[i] == '-' || s[i] == '_' || s[i] == '.' || s[i] == '~') {
            encoded << s[i];
        } else {
            encoded << '%' << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << (int)s[i];
        }
    }
    return encoded.str();
}

std::string url_encode(const char* s, size_t len) {
    std::ostringstream encoded;
    for (size_t i = 0; i < len; ++i) {
        if (isalnum(static_cast<unsigned char>(s[i])) || s[i] == '-' || s[i] == '_' || s[i] == '.' || s[i] == '~') {
            encoded << s[i];
        } else {
            encoded << '%' << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << static_cast<int>(static_cast<unsigned char>(s[i]));
        }
    }
    return encoded.str();
}

Tracker::TrackerResponse sendAnnounceRequest(bool first, const std::string announce_url, const unsigned char* info_hash, const std::string& peer_id, long long length){
    CURL* curl;
    CURLcode res;
    std::string response_data;

    // Initialize curl session
    curl = curl_easy_init();
    if(!curl)
        throw std::runtime_error("Unable to initialize CURL session");

    std::ostringstream url;
    url <<  announce_url
        << "?info_hash=" << url_encode(info_hash, SHA_DIGEST_LENGTH)
        << "&peer_id=" << url_encode(peer_id.c_str(), peer_id.length())
        << "&port=" << "8661"
        << "&uploaded=0"
        << "&downloaded=0"
        << "&left=" << length;
    if(first) url << "&event=" << "started";
    url << "&compact=" << "1";

    // Provide CURL with URL
    curl_easy_setopt(curl, CURLOPT_URL, url.str().c_str());

    // Set callback function to write data to file
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

    // Perform the request
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        throw std::runtime_error("Error sending announce request");
    }

    long response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    std::cout << "Response code :: " << response_code << std::endl;

    // Display received data (tracker response)
    curl_easy_cleanup(curl);

    // Parse tracker response
    return Tracker::TrackerResponse(response_data);
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cout << "Usage: bigtorrent <torrent-filename>" << std::endl;
        return EXIT_FAILURE;
    }
    
    // Parse torrent file
    std::cout << "Parsing torrent file: " << argv[1] << std::endl; 
    Tracker::Tracker torrent_info = Tracker::Tracker(argv[1], true);

    // Send HTTPS request to obtain torrent information
    Tracker::TrackerResponse response = sendAnnounceRequest(true, torrent_info.getAnnounce(), torrent_info.getInfoHash(),  Tracker::generatePeerID("BT", "1000"), torrent_info.getFileLength());
    
    for(auto peer : response.getPeers())
        std::cout << peer.IP << " " << peer.port << std::endl;
    return EXIT_SUCCESS;
}