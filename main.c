#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "ring_buffer.h"
#include "dma_engine.h"
#include "checksum_engine.h"

void* checksum_thread_func(void* arg)
{
    ring_buffer_t* nic_tx_ring = (ring_buffer_t*)arg;

    printf("[SmartNIC Core] Thread started. Ready to process packets...\n");

    while (1)
    {
        packet_t* pkt = (packet_t*)rb_dequeue(nic_tx_ring);

        if (!pkt)
        {
            printf("[SmartNIC Core] Stop signal received. Shutting down.\n");
            break;
        }

        process_packet_checksum(pkt);

        printf("[Network/MAC] Packet transmitted to wire. Freeing memory...\n");
        printf("----------------------------------------------------\n");

        if (pkt->payload != NULL) {
            free(pkt->payload);
        }
        free(pkt);
    }

    return NULL;
}

int main()
{
    printf("=== SmartNIC Hardware Offload Simulation ===\n\n");

    ring_buffer_t* host_tx_ring = rb_create(128);
    ring_buffer_t* nic_tx_ring = rb_create(128);

    dma_context_t dma_ctx;
    dma_ctx.host_tx_ring = host_tx_ring;
    dma_ctx.nic_tx_ring = nic_tx_ring;

    pthread_t dma_tid, checksum_tid;
    pthread_create(&dma_tid, NULL, dma_thread_func, &dma_ctx);
    pthread_create(&checksum_tid, NULL, checksum_thread_func, nic_tx_ring);

    sleep(1);
    printf("\n[Host CPU] Generating 3 application network packets...\n\n");

    for (int i = 1; i <= 3; i++)
    {
        packet_t* pkt = calloc(1, sizeof(packet_t));

        pkt->ip.identification = htons(1000 + i);
        pkt->ip.protocol = 6;
        pkt->tcp.src_port = htons(8080);
        pkt->tcp.dst_port = htons(443);
        char temp_msg[64];
        snprintf(temp_msg, sizeof(temp_msg), "Hello SmartNIC Payload %d", i);

        pkt->payload_len = strlen(temp_msg);

        pkt->payload = (uint8_t*)malloc(pkt->payload_len);

        memcpy(pkt->payload, temp_msg, pkt->payload_len);

        printf("[Host CPU] Pushing Packet %d pointer to Host Ring...\n", i);
        rb_enqueue(host_tx_ring, pkt);

        usleep(300000);
    }

    sleep(1);
    printf("\n[System Shutdown] Initiating graceful shutdown sequence...\n");

    rb_stop(host_tx_ring);

    pthread_join(dma_tid, NULL);
    printf("[System Shutdown] DMA Engine stopped safely.\n");

    rb_stop(nic_tx_ring);

    pthread_join(checksum_tid, NULL);
    printf("[System Shutdown] SmartNIC Core stopped safely.\n");

    rb_destroy(host_tx_ring);
    rb_destroy(nic_tx_ring);

    printf("\n=== Simulation Finished Successfully ===\n");
    return 0;
}
