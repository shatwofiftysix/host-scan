// subnet.c
// handle human readable ip as computer usable bit string
// creates big endian bit string (ignoring system architecture)

// example:
// int subnet[] = {192, 168, 100, 99, 24};
// unsigned int userIp = ip4AddrArrToBits(subnet);
// unsigned int mask = getIp4MaskFromCidr(subnet[4]);
// unsigned int ntwAddr = getIp4NtwAddr(userIp, mask);
// unsigned int firstAddr = ntwAddr + 1;
// unsigned int bcastAddr = getIp4BcastAddr(ntwAddr, subnet[4]);
// unsigned int lastAddr = bcastAddr - 1;

// for debugging endianess:
// struct sockaddr_in myaddr;
// myaddr.sin_family = AF_INET;
// myaddr.sin_port = htons(55);
// inet_aton("127.0.0.1", &myaddr.sin_addr.s_addr);

// printf("struct localhost: %d\n", myaddr.sin_addr.s_addr);
// printf("ntohl(struct) localhost: %d\n", ntohl(myaddr.sin_addr.s_addr));
// printf("calc localhost: %d\n", addr);
// printf("htonl(calc) localhost: %d\n", htonl(addr));
// printf("big localhost: %d\n", 0x7f000001);
// printf("little localhost: %d\n", 0x100007f);

// struct localhost: 16777343
// ntohl(struct) localhost: 2130706433
// calc localhost: 2130706433
// htonl(calc) localhost: 16777343
// big localhost: 2130706433
// little localhost: 16777343


#include <stdio.h>
#include <math.h>

unsigned int ip4AddrArrToBits(int * ipArr) {
  unsigned int ip =    (ipArr[0] << 24 |
                        ipArr[1] << 16 |
                        ipArr[2] << 8  |
                        ipArr[3]);

  return ip;
}

int bitsToIp4AddrArr(unsigned int ipInt, int * ipArr) {
  ipArr[0] = (ipInt & 0xff000000) >> 24;
  ipArr[1] = (ipInt & 0x00ff0000) >> 16;
  ipArr[2] = (ipInt & 0x0000ff00) >> 8;
  ipArr[3] = (ipInt & 0x000000ff);
  
  return 0;
}

int printIp4(int ip) {
  printf("%d.%d.%d.%d\n", (ip & 0xff000000) >> 24,
                          (ip & 0x00ff0000) >> 16,
                          (ip & 0x0000ff00) >> 8,
                          (ip & 0x000000ff));
  return 0;
}

unsigned int getIp4MaskFromCidr(int cidr) {
  // use shift because pow trick only works with big endian: 2^cidr - 1
  return 4294967295 << (32-cidr);
}

unsigned int getIp4NtwAddr(unsigned int ip, unsigned int mask) {
  // eg. 10.10.10.10 & 255.255.0.0 = 10.10.0.0
  return ip & mask;
}

unsigned int getIp4BcastAddr (unsigned int ntwAddr, int cidr) {
  // eg. 0.0.0.255 | 10.10.10 = 10.10.10.255
  return (4294967295 >> cidr) | ntwAddr;
}
