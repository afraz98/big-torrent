#ifndef TRACKER_RESPONSE_H
#define TRACKER_RESPONSE_H

#include <bencode.h>
#include <string>
#include <vector>

namespace Tracker {
    struct Peer { 
        std::string IP;
        uint16_t port;
    };

    class TrackerResponse {
        public:
            TrackerResponse(std::string response);
            ~TrackerResponse() = default;

            inline std::vector<Peer> getPeers() { return peer_list; }
            inline int getInterval() { return interval; }
        private:
            void parsePeers(std::string peers);
            bencode_value* raw_tracker_response;
            std::vector<Peer> peer_list;
            int interval, min_interval, seeders, leechers, peers, completed, downloaded;
    }; // class TrackerResponse
} // namespace Tracker

#endif // TRACKER_RESPONSE_H