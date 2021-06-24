/**
 * @ Description  :
 * @ Version      : 1.0
 * @ Author       : koritafei(koritafei@gmail.com)
 * @ Date         : 2021-06-11 10:26:28
 * @ LastEditors  : koritafei(koritafei@gmail.com)
 * @ LastEditTime : 2021-06-12 03:29:48
 * @ FilePath     : /libevent_l/src/epollDemo/epollClient.cpp
 * @ Copyright (C) 2021 koritafei(koritafei@gmail.com). All rights reserved.
 * */

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_LINE    (1024)
#define SERVER_PORT (7788)

void setnoblocking(int fd) {
  int opts = 0;
  opts     = fcntl(fd, F_GETFL);
  opts |= O_NONBLOCK;
  fcntl(fd, F_SETFL);
}

int main(int argc, char **argv) {
  int                sockfd;
  char               recvline[MAX_LINE + 1] = {0};
  struct sockaddr_in server_addr;
  if (2 != argc) {
    fprintf(stderr, "usage ./client <server IP>");
    exit(0);
  }

  // 创建socket
  if (0 > (sockfd = socket(AF_INET, SOCK_STREAM, 0))) {
    fprintf(stderr, "socket create error");
    exit(0);
  }

  // server addr 赋值
  bzero(&server_addr, sizeof(struct sockaddr_in));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port   = htons(SERVER_PORT);

  if (0 >= inet_pton(AF_INET, argv[1], &server_addr.sin_addr.s_addr)) {
    fprintf(stderr, "inet_pton error for %s\n", argv[1]);
    exit(0);
  }

  // 链接服务端
  if (0 > connect(sockfd,
                  (struct sockaddr *)&server_addr,
                  sizeof(struct sockaddr_in))) {
    perror("connect");
    fprintf(stderr, "connect error\n");
    exit(0);
  }

  setnoblocking(sockfd);
  char input[100];
  int  n     = 0;
  int  count = 0;

  // 不断地从标准输入读入字符串
  while (NULL != fgets(input, 100, stdin)) {
    printf("send [%s]\n", input);
    n = 0;
    // 输入字符发送到服务端
    n = send(sockfd, input, sizeof(input), 0);
    if (0 > n) {
      perror("send");
    }

    n     = 0;
    count = 0;

    // 读取服务器数据
    while (1) {
      n = read(sockfd, recvline + count, MAX_LINE);
      if (MAX_LINE == n) {
        count += n;
        continue;
      } else if (0 > n) {
        perror("recv error");
        break;
      } else {
        count += n;
        recvline[count] = '\0';
        printf("revc data [%s]\n", recvline);
        break;
      }
    }
  }

  return 0;
}