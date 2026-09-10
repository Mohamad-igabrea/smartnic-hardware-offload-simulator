#include "dma_engine.h"
#include <stdio.h>
#include <unistd.h>

void* dma_thread_func(void* arg)
{
    dma_context_t* ctx = (dma_context_t*)arg;

    printf("[DMA Engine] Thread started. Waiting for packets from Host CPU...\n");

    while (1)
    {
        void* packet_ptr = rb_dequeue(ctx->host_tx_ring);

        if (!packet_ptr)
        {
            printf("[DMA Engine] Stop signal received. Shutting down DMA safely.\n");
            break;
        }

        usleep(150000);

        if (!rb_enqueue(ctx->nic_tx_ring, packet_ptr))
        {
            break;
        }

        printf("[DMA Engine] Successfully transferred packet pointer across PCIe bus.\n");
    }

    return NULL;
}
