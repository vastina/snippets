#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

void errorhandling( const char* message, int error )
{
  fprintf( stderr, "%s : %d \n", message, error );
};

int main(int argc, char *argv[]){
    // struct hostent *host;
    // host = gethostbyname("stanford.edu");

    // printf("%s \n",host->h_name);
    // for(int i=0; host->h_aliases[i]; ++i)
    //     printf("%s\n", host->h_aliases[i]);
    // printf("addrtype: %d\n", host->h_addrtype);
    // printf("length?: %d\n", host->h_length);
    // for(int i=0; host->h_addr_list[i]; ++i)
    //     printf("%s\n", host->h_addr_list[i]);

    // int sock = socket(AF_INET, SOCK_STREAM, 0);
    // int sndbufsz, recvbufsz;
    // int sb1 = sizeof(sndbufsz);
    // int sb2 = sizeof(recvbufsz);
    // int state = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*)&sndbufsz, &sb1);
    // if(state) printf("sb\n");
    // printf("%d\n", sndbufsz);

    // state = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*)&recvbufsz, &sb2);
    // if(state) printf("sb\n");
    // printf("%d\n", recvbufsz);

    // sndbufsz = BUFSIZ;  recvbufsz = BUFSIZ;

    // state = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*)&sndbufsz, sb1 );
    // state = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*)&sndbufsz, &sb1);
    // if(state) printf("sb\n");
    // printf("%d\n", sndbufsz);

    // state = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*)&recvbufsz, sb2 );
    // state = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*)&recvbufsz, &sb2);
    // if(state) printf("sb\n");
    // printf("%d\n", recvbufsz);



    // struct sockaddr_in serveaddr;
    // memset(&serveaddr, 0, sizeof(serveaddr));
    // serveaddr.sin_addr.s_addr= inet_addr("171.67.215.200");
    // const char* sb = "171.67.215.200";
    // struct hostent *host2 = gethostbyaddr((void*)sb, 4, AF_INET);
    

    // printf("%s \n",host2->h_name);
    // for(int i=0; host2->h_aliases[i]; ++i)
    //     printf("%s\n", host2->h_aliases[i]);
    // printf("%d\n", host2->h_addrtype);
    // printf("%d\n", host2->h_length);
    // for(int i=0; host2->h_addr_list[i]; ++i)
    //     printf("%s\n", host2->h_addr_list[i]);

    // struct addrinfo hints, *res, *p;
  // int status;
  // char ipstr[INET6_ADDRSTRLEN];
  // memset( &hints, 0, sizeof hints );
  // hints.ai_family = AF_UNSPEC; // IPv4 或 IPv6
  // hints.ai_socktype = SOCK_STREAM;
  // if ( ( status = getaddrinfo( host.c_str(), NULL, &hints, &res ) ) != 0 ) {
  //   fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( status ) );
  //   return;
  // }
  // for ( p = res; p != NULL; p = p->ai_next ) {
  //   void* addr;

  //   // 获取地址结构体中的 IP 地址
  //   if ( p->ai_family == AF_INET ) { // IPv4
  //     struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
  //     addr = &( ipv4->sin_addr );
  //     inet_ntop( p->ai_family, addr, ipstr, sizeof ipstr );
  //     printf( "ip: %s\n", ipstr );
  //     break;
  //   } else { // IPv6
  //     struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)p->ai_addr;
  //     addr = &( ipv6->sin6_addr );
  //   }

  //   // 将二进制地址转换成文本表示，并打印

  //   // printf("%s: %s\n", ipver, ipstr);
  // }  freeaddrinfo( res );

  int so = socket( AF_INET, SOCK_STREAM, 0 );
  if ( so == -1 ) {
    errorhandling( "invalid socket: ", errno );
    return;
  }
  struct sockaddr_in serveaddr;
  memset( &serveaddr, 0, sizeof( serveaddr ) );
  serveaddr.sin_family = AF_INET;
serveaddr.sin_addr.s_addr = inet_addr(host.c_str());
  serveaddr.sin_addr.s_addr = inet_addr( "171.67.215.200" );

  short PORT = 1453;
#if 0
  printf("input port:");
	scanf("%hd", &PORT);
#endif
  serveaddr.sin_port = htons( PORT );
  if ( connect( so, (struct sockaddr*)&serveaddr, sizeof( serveaddr ) ) == -1 )
    errorhandling( "connect fail: ", errno );
  

  char buffer[BUFSIZ] = { 0 };
  // strcpy(buffer, path.c_str());
  // strcpy(buffer,
  // "GET / HTTP/1.1\r\n"
  // "Host: www.stanford.edu\r\n"
  // "Connection: close\r\n");
  printf( "path: %s\n", path.c_str() );
  strcpy( buffer, "GET " );
  strcpy( buffer, path.c_str() );
  strcpy( buffer,
          " HTTP/1.1\r\n"
          "Connection: close\r\n" );
  std::cout << buffer << std::endl;
  if ( write( so, buffer, strlen( buffer ) ) == -1 )
    errorhandling( "write fail: ", errno );

  if ( read( so, buffer, BUFSIZ ) == -1 )
    errorhandling( "read fail: ", errno );
  else
    cout << buffer << endl;

  // cerr << "Function called: get_URL(" << host << ", " << path << ")\n";
  // cerr << "Warning: get_URL() has not been implemented yet.\n";

    return 0;
}