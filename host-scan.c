// host-scan.c
// scan for hosts and maybe ports too

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

// local includes
#include "subnet.h"

void usage() {
  puts("usage: host-scan <subnet/cidr>");
  puts("ex: host-scan 10.10.0.0/16");
  puts("ex: host-scan 192.168.100.100/32");
}

int main(int argc, char ** argv) {
  // hard coded vars; should move to args
  int debug     = 0;
  int startPort = 1;
  int endPort   = 65535;

  // parse args
  if (argc < 2) {
    usage();
  }
  int subnet[5];

  sscanf(argv[1], "%d.%d.%d.%d/%d",   (int *) &subnet[0],
                                      (int *) &subnet[1],
                                      (int *) &subnet[2],
                                      (int *) &subnet[3],
                                      (int *) &subnet[4]);

  if (subnet[4] < 0 || subnet[4] > 32) {
    printf("bad cidr: %d\n", subnet[4]);
    return -1;
  }

  unsigned int userIp = ip4AddrArrToBits(subnet);
  unsigned int mask = getIp4MaskFromCidr(subnet[4]);
  unsigned int ntwAddr;
  unsigned int firstAddr;
  unsigned int bcastAddr;
  unsigned int lastAddr;
  
  if (subnet[4] == 32) {
    ntwAddr = userIp;
    firstAddr = userIp;
    bcastAddr = userIp;
    lastAddr = userIp;
  } else if (subnet[4] == 31) {
    // nonsensical cidr, make it work anyway
    ntwAddr = getIp4NtwAddr(userIp, mask); // ip - 1
    firstAddr = ntwAddr;
    bcastAddr = userIp;
    lastAddr = userIp;
  } else {  
    ntwAddr = getIp4NtwAddr(userIp, mask); // if this is 0 user gave nonsensical address/mask
    firstAddr = ntwAddr + 1;
    bcastAddr = getIp4BcastAddr(ntwAddr, subnet[4]);
    lastAddr = bcastAddr - 1;
  }

  if (debug) {  
    printIp4(userIp);
    printIp4(mask);
    printIp4(ntwAddr);
    printIp4(firstAddr);
    printIp4(bcastAddr);
    printIp4(lastAddr);
  }
  // done parsing/sanitizing user input

  struct sockaddr_in targetSA;
  memset(&targetSA, 0, sizeof targetSA);
  targetSA.sin_family = AF_INET;
  struct in_addr targetAddr;
  memset(&targetAddr, 0, sizeof targetAddr);
  int sockfd;


  // iterate all addresses
  for (int addr = firstAddr; addr <= lastAddr; addr++) {
    printIp4(addr); 

    // TODO: ping establish it's up first; or maybe quit if all <1024 are closed; make it an option

    // htonl/ntohl just swap endianess when not used on struct
    // Will break on big endian (!intel) machine? 
    // Or not because both struct and num will be reversed (since it was created bitwise)?
    targetAddr.s_addr = ntohl(addr); // TODO ^^^^^
    targetSA.sin_addr = targetAddr;

    // port scan
    for (int port = startPort; port <= endPort; port++) {
      sockfd = socket(AF_INET, SOCK_STREAM, 0);
      if (sockfd < 0) {
        perror("socket error");
      }

      targetSA.sin_port = htons(port);
      if (connect(sockfd, (struct sockaddr *)&targetSA, sizeof targetSA) < 0) {
        if (debug) {
          printf("port: %d ; error: %s\n", port, strerror(errno));
        }
      } else {
        printf("port: %d open\n", port);
      }
      fflush(stdout);
      close(sockfd);
    }
    printf("\n");
  }

  return 0;
}
