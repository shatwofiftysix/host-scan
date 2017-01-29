# host-scan
connection based port scanner

## Build and Run
gcc -Wall -lm host-scan.c -o host-scan

./host-scan 255.255.255.255/24

There could be problems on big-endian machines with the way the subnet library assembles the IP. I haven't tested it yet.
