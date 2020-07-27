#include "client.h"

double GetThroughput(int fd, int size, char* buffer);
int main(int argc, char **argv)
{
    char recvline[4096], sendline[4096];
    struct sockaddr_in servaddr;
    double throughput;

    if (argc != 2)
    {
        printf("usage: client <ipaddress>\n");
        return 0;
    }

    char buf[] = "test", recvbuf[1024];
    struct timeval t, tend;
    unsigned long long secDiff;
    int sockfd, n;
    char* buffer;
    char buff[4096];

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }

    int flag = 1;
    int result = setsockopt(sockfd,        /* socket affected */
                            IPPROTO_TCP,   /* set option at TCP level */
                            TCP_NODELAY,   /* name of option */
                            (char *)&flag, /* the cast is historical cruft */
                            sizeof(int));  /* length of option value */
    if (result < 0)
    {
        perror("setsockopt");
        return 1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(6666);
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
    {
        printf("inet_pton error for %s\n", argv[1]);
        return 0;
    }
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }

    /* Initialize send/receive buffer. */
    InitializeBuffer(&buffer);



    for (int i=0; i <= 10; i++) {
        int size = 1 << i;
        printf("%d\t", size);


        throughput = GetThroughput(sockfd, size, buffer);
        printf("%lf\tMbps\n", throughput / 1024.0);
        

    }
    close(sockfd);

    return 0;
}


double GetThroughput(int fd, int size, char* buffer)
{
    char buff[100];
    int secDiff, n;
    double throughput;
    struct timeval t, tend;

    memset(buff, 0, sizeof(buff));
    gettimeofday(&t, NULL);
    sizeSend(fd, buffer, BUFFERSIZE, 0, size);
    
    n = recv(fd, buff, 5, 0);
    if (strcmp(buff, "done") == 0) {
        gettimeofday(&tend, NULL);
        secDiff = (tend.tv_sec - t.tv_sec) * 1000000 + tend.tv_usec - t.tv_usec;
        throughput = ((double)BUFFERSIZE / (double)secDiff) * (double)1000000 * 8.0 / 1024;
        //printf("%lf ms, throughput = %lf Mbps = %lf MB/s\n", (double)secDiff, throughput, throughput / 8.0 / 1024);
        //printf("%lf\tGbps\n", throughput / 1024.0 / 1024.0);
    }
    else {
        fprintf(stderr, "[panic] what is server doing?\n");
        exit(1);
    }
    return throughput;
}


