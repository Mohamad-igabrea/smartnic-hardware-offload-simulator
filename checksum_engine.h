#pragma once


#include <stdint.h>
#include <stddef.h>
// Big-Endian 16-bit: Value MUST be in Network Byte Order. Use ntohs().
typedef uint16_t be16_t;

// Big-Endian 32-bit: Value MUST be in Network Byte Order. Use ntohl().
typedef uint32_t be32_t;


typedef struct
{
    uint8_t dst_mac[6];
    uint8_t src_mac[6];
    be16_t ethertype;
} __attribute__((packed)) eth_hdr_t;

typedef struct
{
    uint8_t version_ihl;
    uint8_t tos;
    be16_t total_length;
    be16_t identification;
    be16_t fragment_offset;
    uint8_t ttl;
    uint8_t protocol;
    be16_t checksum;
    be32_t src_ip;
    be32_t dst_ip;
} __attribute__((packed)) ipv4_hdr_t;

typedef struct
{
    be16_t src_port;
    be16_t dst_port;
    be32_t seq_num;
    be32_t ack_num;
    uint8_t data_offset_reserved;
    uint8_t flags;
    be16_t window_size;
    be16_t checksum;
    be16_t urgent_ptr;
} __attribute__((packed)) tcp_hdr_t;

typedef struct
{
    eth_hdr_t eth;
    ipv4_hdr_t ip;
    tcp_hdr_t tcp;
    uint8_t* payload;
    size_t payload_len;
} packet_t;

uint16_t calculate_checksum(void* addr, int count);
void process_packet_checksum(packet_t* pkt);
