#include "../Includes/Packet_Process.hpp"
#include "../Includes/Packet_Capture.hpp"
#include "../Includes/Global_Defines.hpp"

#include <iostream>
#include <algorithm>
#include <zlib.h>

Queue PacketProcess::SendQueue;

void PacketProcess::GetPackets() {
    RawDataBuffer.clear();
    uint8_t Packet[PACKET_SIZE];
    size_t PacketSize;
    for(int PacketIndex = 0; PacketIndex < NUM_PACKETS_TO_FETCH; PacketIndex++) {
        if(!PacketCapture::ReciveQueue.dequeue(Packet, PacketSize)) {
            std::cerr << "Failed to fetch packet from ReciveQueue" << std::endl;
            continue;
        }
        RawDataBuffer.insert(RawDataBuffer.end(), Packet, Packet + PacketSize);
    }
}

void PacketProcess::CompressPacket() {
    if (RawDataBuffer.empty()) {
        std::cerr << "RawDataBuffer is empty" << std::endl;
        return;
    }     
    CompressedDataBuffer.resize(RawDataBuffer.size() + (RawDataBuffer.size() * COMPRESSION_OVERHEAD_PERCENT) + ZLIB_OVERHEAD_BYTES); //  10% overhead + zlib extra space
    uLongf CompressedSize = CompressedDataBuffer.size();
    int result = compress2(CompressedDataBuffer.data(), &CompressedSize, RawDataBuffer.data(), RawDataBuffer.size(), Z_BEST_COMPRESSION);
    if (result == Z_OK) {
        std::cout << "Compressed len is: " << CompressedSize << std::endl;
        CompressedDataBuffer.resize(CompressedSize);
        return;
    }
}

void PacketProcess::StorePackets() {
    int TotalSize = CompressedDataBuffer.size();
    int offset = 0;
    while (offset < TotalSize) {
        size_t Chunk = std::min(MAX_COMPRESSED_PACKET_SIZE, TotalSize - offset);
        if (!SendQueue.enqueue(CompressedDataBuffer.data() + offset, Chunk))
            std::cerr << "Send Queue is full" << std::endl;
        offset += Chunk;
    }
}

void PacketProcess::ProcessStart() {
    GetPackets();
    CompressPacket();
    StorePackets();
}