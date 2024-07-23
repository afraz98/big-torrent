import bencodepy
import hashlib
import urllib.parse
import binascii


def generateAnnounceURL(torrent_file_path):
    # Read and decode the torrent file
    with open(torrent_file_path, 'rb') as file:
        torrent_data = bencodepy.decode(file.read())

    # Extract the 'info' section
    info_section = torrent_data[b'info']

    # Calculate the SHA1 hash of the 'info' section
    info_hash = hashlib.sha1(bencodepy.encode(info_section)).digest()

    # Convert the info_hash to a hex string and URL-encode it
    encoded_info_hash = urllib.parse.quote(binascii.hexlify(info_hash).decode('utf-8'))

    # Extract the announce URL
    announce_url = torrent_data[b'announce'].decode('utf-8')

    # Generate a valid peer_id (20 bytes)
    peer_id = '-PC0001-000000000000'  # Example peer_id, make sure it's 20 characters

    # Construct the parameters
    params = {
        'info_hash': encoded_info_hash,
        'peer_id': peer_id,
        'port': 6881,
        'uploaded': 0,
        'downloaded': 0,
        'left': info_section[b'length'],  # Total size of the torrent in bytes
        'compact': 1,
        'event': 'started'
    }

    # URL encode the parameters
    encoded_params = urllib.parse.urlencode(params)

    # Generate the full announce request URL
    announce_request_url = f"{announce_url}?{encoded_params}"

    # Print the results
    print(f"Announce Request URL: {announce_request_url}")


