#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "atlas_header.h"

#define IOT_WRITE (1)
#define IOT_READ (0)

static int ReadAll(int fd, char* buf, size_t count);


int main()
{
    struct sockaddr_in sockAddr;
    int opt = 1;
    int counter = 0;
    char* storage = (char*)calloc(512 * 1024 * 1024, 1);
    int dataSock, srvSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    setsockopt(srvSock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));

	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sockAddr.sin_port = htons(29000);
    bind(srvSock, (struct sockaddr *)&sockAddr, sizeof(sockAddr));

    listen(srvSock, 3);

    dataSock = accept(srvSock, NULL, NULL);

    printf("Ready...\n");

    while (1)
    {

        AtlasHeader atlasHdr;

        if (ReadAll(dataSock, (char*)&atlasHdr, sizeof(atlasHdr)) == -1) break;

        if(IOT_WRITE == atlasHdr.m_type)
        {
            printf("%d\t write %d bytes\ton offset %d\n",counter++, atlasHdr.m_dataLen, atlasHdr.m_iotOffset);
        }
        else if(IOT_READ == atlasHdr.m_type)
        {
            printf("%d\t read %d bytes\ton offset %d\n",counter++, atlasHdr.m_dataLen, atlasHdr.m_iotOffset);
        }
        else
        {
            printf("%d\t not write nor read\n", counter++);
        }


        switch (atlasHdr.m_type)
        {
        case 0:
            send(dataSock, &atlasHdr, sizeof(atlasHdr), 0);
            send(dataSock, storage+atlasHdr.m_iotOffset, atlasHdr.m_dataLen, 0);
            break;
        
        case 1:
            if (ReadAll(dataSock, storage+atlasHdr.m_iotOffset, atlasHdr.m_dataLen) == -1) break;
            send(dataSock, &atlasHdr, sizeof(atlasHdr), 0);
            break;
        
        default:
            break;
        }

        usleep(5000);
    }

    close(dataSock);
    close(srvSock);
    printf("Exit.\n");
}


static int ReadAll(int fd, char* buf, size_t count)
{
    int bytes_read;

    while (count > 0)
    {
        bytes_read = recv(fd, buf, count, 0);

        if (bytes_read <= 0)
        {
            return -1;
        }

        buf += bytes_read;
        count -= bytes_read;
    }

    return 0;
}
