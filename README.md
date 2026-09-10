# SmartNIC Hardware Offload Simulator

A multithreaded SmartNIC hardware-offload simulation written in C.

The project simulates how network packets can move from a host CPU to a network interface through a DMA-like transfer mechanism, while offloading IPv4 and TCP checksum computation to a separate SmartNIC processing thread.

## Architecture

```text
Host CPU
   |
   v
Host TX Ring Buffer
   |
   v
DMA Engine Thread
   |
   v
NIC TX Ring Buffer
   |
   v
SmartNIC Checksum Thread
   |
   v
IPv4 / TCP Checksum Processing
   |
   v
Simulated Network Transmission
