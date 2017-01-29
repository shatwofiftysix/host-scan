// subnet.h
// handle human readable ip as computer usable bit string
// creates big endian bit string (ignoring system architecture)

unsigned int ip4AddrArrToBits(int * ipArr);
int bitsToIp4AddrArr(unsigned int ipInt, int * ipArr);
int printIp4(int ip);
unsigned int getIp4MaskFromCidr(int cidr);
unsigned int getIp4NtwAddr(unsigned int ip, unsigned int mask);
unsigned int getIp4BcastAddr (unsigned int ntwAddr, int cidr);
