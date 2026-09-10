#pragma once


#include "ring_buffer.h"

typedef struct
{
    ring_buffer_t* host_tx_ring;
    ring_buffer_t* nic_tx_ring;
} dma_context_t;

void* dma_thread_func(void* arg);
