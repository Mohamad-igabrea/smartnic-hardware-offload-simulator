#include "checksum_engine.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    uint32_t src_ip;
    uint32_t dst_ip;
    uint8_t zero;
    uint8_t protocol;
    uint16_t tcp_length;
} __attribute__((packed)) tcp_pseudo_hdr_t;

uint16_t calculate_checksum(void* addr, int count)
{
    register uint32_t sum = 0;
    uint16_t* ptr = addr;

    while (count > 1)
    {
        sum += *ptr++;
        count -= 2;
    }

    if (count > 0)
    {
        sum += *(uint8_t*)ptr;
    }

    while (sum >> 16)
    {
        sum = (sum & 0xffff) + (sum >> 16);
    }

    return ~sum;
}

void process_packet_checksum(packet_t* pkt)
{
    pkt->ip.checksum = 0;
    pkt->ip.checksum = calculate_checksum(&pkt->ip, sizeof(ipv4_hdr_t));


    pkt->tcp.checksum = 0;

    size_t tcp_segment_len = sizeof(tcp_hdr_t) + pkt->payload_len;
    size_t total_len = sizeof(tcp_pseudo_hdr_t) + tcp_segment_len;

    tcp_pseudo_hdr_t pseudo;
    pseudo.src_ip = pkt->ip.src_ip;
    pseudo.dst_ip = pkt->ip.dst_ip;
    pseudo.zero = 0;
    pseudo.protocol = pkt->ip.protocol;
    pseudo.tcp_length = htons((uint16_t)tcp_segment_len);

    uint8_t* tmp_buf = malloc(total_len);
    if (!tmp_buf)
    {
        fprintf(stderr, "[Checksum Engine] Failed to allocate buffer for TCP checksum calculation\n");
        return;
    }

    memcpy(tmp_buf, &pseudo, sizeof(tcp_pseudo_hdr_t));

    memcpy(tmp_buf + sizeof(tcp_pseudo_hdr_t), &pkt->tcp, sizeof(tcp_hdr_t));

    if (pkt->payload != NULL && pkt->payload_len > 0)
    {
        size_t offset = sizeof(tcp_pseudo_hdr_t) + sizeof(tcp_hdr_t);
        memcpy(tmp_buf + offset, pkt->payload, pkt->payload_len);
    }

    pkt->tcp.checksum = calculate_checksum(tmp_buf, (int)total_len);

    free(tmp_buf);

    printf("[Checksum Engine] IP Checksum calculated: 0x%04X\n", ntohs(pkt->ip.checksum));
    printf("[Checksum Engine] TCP Checksum calculated: 0x%04X\n", ntohs(pkt->tcp.checksum));
}
