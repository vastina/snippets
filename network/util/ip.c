#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <net/if.h>
#include <unistd.h>

int main()
{
  {
    struct ifaddrs *ifaddr;
    char host[NI_MAXHOST];

    if ( getifaddrs( &ifaddr ) == -1 ) {
      perror( "getifaddrs" );
      return 1;
    }

    for ( struct ifaddrs *ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next ) {
      if ( ifa->ifa_addr == NULL )
        continue;

      int family = ifa->ifa_addr->sa_family;

      if ( family == AF_INET || family == AF_INET6 ) {
        int s = getnameinfo( ifa->ifa_addr,
                             ( family == AF_INET ) ? sizeof( struct sockaddr_in ) : sizeof( struct sockaddr_in6 ),
                             host,
                             NI_MAXHOST,
                             NULL,
                             0,
                             NI_NUMERICHOST );
        if ( s != 0 ) {
          printf( "getnameinfo() failed: %s\n", strerror( s ) );
          return 1;
        }

        printf( "%s address: %s\n", ifa->ifa_name, host );
      }
    }

    freeifaddrs( ifaddr );
  }
  putchar('\n');
  {
    int fd;
    struct ifreq ifr;

    // 创建一个 socket
    fd = socket( AF_INET, SOCK_DGRAM, 0 );
    if ( fd == -1 ) {
      perror( "socket" );
      return 1;
    }

    // 指定网络接口名，例如 "eth0"
    strncpy( ifr.ifr_name, "eth1", IFNAMSIZ - 1 );

    // 获取网络接口地址
    if ( ioctl( fd, SIOCGIFADDR, &ifr ) == -1 ) {
      perror( "ioctl" );
      close( fd );
      return 1;
    }

    close( fd );

    // 提取 IP 地址
    struct sockaddr_in* ipaddr = (struct sockaddr_in*)&ifr.ifr_addr;
    printf( "IP address: %s\n", inet_ntoa( ipaddr->sin_addr ) );
  }
  putchar('\n');
  {
    char name[100];
  if(gethostname(name, sizeof(name)) == -1){
    perror("gethostname");
    return 1;
  }
  struct hostent* host = gethostbyname(name);
  if(host == NULL){
    perror("gethostbyname");
    return 1;
  }
  for(int i = 0; host->h_addr_list[i]; i++){
    struct in_addr addr;
    memcpy(&addr, host->h_addr_list[i], sizeof(addr));
    printf("ip addr: %s\n", inet_ntoa(addr));
  }
  }
}
