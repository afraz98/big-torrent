#include <arpa/inet.h>
#include <bencode.h>
#include <curl/curl.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <sys/socket.h>
#include <iomanip>
#include <iostream>
#include <unistd.h>

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

std::string url_encode(std::vector<unsigned char>s, size_t len) {
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

Torrent::TrackerResponse sendAnnounceRequest(bool first, const std::string announce_url, const std::vector<unsigned char> info_hash, const std::string& peer_id, long long length){
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
    return Torrent::TrackerResponse(response_data);
}

bool performHandshake(std::string& ip, uint16_t port, const std::vector<unsigned char> &info_hash, const std::string &peer_id) {
    // Socket setup
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Failed to connect to peer " << ip << std::endl;
        close(sock);
        return false;
    }

    // Build handshake message
    std::string pstr = "BitTorrent protocol";
    unsigned char pstrlen = static_cast<unsigned char>(pstr.size());
    unsigned char reserved[8] = {0};

    std::vector<unsigned char> handshake_msg;
    handshake_msg.push_back(pstrlen);
    handshake_msg.insert(handshake_msg.end(), pstr.begin(), pstr.end());
    handshake_msg.insert(handshake_msg.end(), reserved, reserved + 8);
    handshake_msg.insert(handshake_msg.end(), info_hash.begin(), info_hash.end());
    handshake_msg.insert(handshake_msg.end(), peer_id.begin(), peer_id.end());

    // Send handshake
    if (send(sock, handshake_msg.data(), handshake_msg.size(), 0) != handshake_msg.size()) {
        std::cerr << "Failed to send handshake" << std::endl;
        close(sock);
        return false;
    }

    // Receive handshake
    std::vector<unsigned char> response(68);
    ssize_t bytes_received = recv(sock, response.data(), response.size(), 0);
    if (bytes_received != 68) {
        std::cerr << "Invalid handshake response" << std::endl;
        close(sock);
        return false;
    }

    // Validate the response (info_hash match)
    if (!std::equal(info_hash.begin(), info_hash.end(), response.begin() + 28)) {
        std::cerr << "Info hash mismatch" << std::endl;
        close(sock);
        return false;
    }

    // Handshake successful
    std::cout << "Handshake successful with " << ip << std::endl;
    close(sock);
    return true;        
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cout << "Usage: bigtorrent <torrent-filename>" << std::endl;
        return EXIT_FAILURE;
    }
    
    // Parse torrent file
    std::cout << "Parsing torrent file: " << argv[1] << std::endl; 
    Torrent::Tracker torrent_info = Torrent::Tracker(argv[1], true);

    // Send HTTPS request to obtain torrent information
    Torrent::TrackerResponse response = sendAnnounceRequest (
        true, 
        torrent_info.getAnnounce(), 
        torrent_info.getInfoHash(),  
        torrent_info.getPeerID(), 
        torrent_info.getFileLength()
    );
    
    for(auto peer : response.getPeers())
        performHandshake(peer.IP, peer.port, torrent_info.getInfoHash(), torrent_info.getPeerID());
    return EXIT_SUCCESS;
}