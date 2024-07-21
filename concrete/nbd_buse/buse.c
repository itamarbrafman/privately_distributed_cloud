#define _POSIX_C_SOURCE (200809L)
#include <stdio.h> 
#include <linux/nbd.h> /*struct nbd_reply*/
#include <sys/socket.h> /*socketpair*/ 
#include <fcntl.h> /*O_RDWR*/       
#include <sys/ioctl.h>  /*ioctl*/  
#include <unistd.h> /*close*/   
#include <sys/time.h> /*fd_set*/ /*struct timeval*/
#include <stdlib.h> /*exit*/
#include <assert.h> /*assert*/
#include <arpa/inet.h> /*ntohl*/
#include <string.h> /*memcpy*/
#include "buse.h"
#include <pthread.h> /*mutex*/

#define ERROR (-1)
#define SUCCESS (0)
#define BLOCK_SIZE (4096)


#ifdef WORDS_BIGENDIAN
u_int64_t ntohll(u_int64_t a) {
  return a;
}
#else
u_int64_t ntohll(u_int64_t a) {
  u_int32_t lo = a & 0xffffffff;
  u_int32_t hi = a >> 32U;
  lo = ntohl(lo);
  hi = ntohl(hi);
  return ((u_int64_t) lo) << 32U | hi;
}
#endif
#define htonll ntohll

/* pthread_mutex_t g_mutex; */

typedef struct daemon_request
{
    buse_request_t request;
    struct nbd_reply reply;
    char data[1];
} daemon_request_t;

static int ReadRequest(int nbd_socket, struct nbd_request *request);
static int ReadData(int nbd_socket, char *data, int32_t data_len);

int NbdOpen(const char *file_name, size_t size_in_bytes)
{
    int sp[2];
    int nbd = 0;
    pid_t pid = 0;

    if (ERROR == socketpair(AF_UNIX, SOCK_STREAM, 0, sp)) 
    {
        perror("socketpair");

        return ERROR;
    }

    nbd = open(file_name, O_RDWR);
    if (-1 == nbd) 
    {
        close(sp[0]);
        close(sp[1]);        
        close(nbd);
        perror("open");

        return ERROR;
    }

    if (-1 == ioctl(nbd, NBD_SET_SIZE, size_in_bytes)) 
    {
        close(sp[0]);
        close(sp[1]);
        close(nbd);
        perror("ioctl");

        return ERROR;
    }  

    pid = fork();
    if (pid == 0) 
    {
        close(sp[1]);

        if (-1 == ioctl(nbd, NBD_SET_SOCK, sp[0])) 
        {
            close(sp[0]);
            close(sp[1]);        
            close(nbd);
            perror("ioctl");

            return ERROR;
        }   
        if (-1 == ioctl(nbd, NBD_DO_IT)) 
        {
            close(sp[0]);
            close(sp[1]);            
            close(nbd);            
            perror("ioctl");

            return ERROR;
        }

        close(nbd);  
        close(sp[0]);
        return 0;


    }
    else if (pid < 0)
    {
        close(sp[0]);
        close(sp[1]);
        close(nbd);
        perror("fork");

        return ERROR;
    }

    close(nbd);  
    close(sp[0]);

    return sp[1]; 
}

buse_request_t *NbdGetRequest(int nbd_socket)
{
    struct nbd_request request;
    int return_value = 0;
    daemon_request_t *daemon_request = NULL;

    assert(nbd_socket > 0);

    return_value = ReadRequest(nbd_socket, &request);
    if(0 > return_value)
    {
        return NULL;
    }
    
    daemon_request = (daemon_request_t *)malloc(sizeof(daemon_request_t) + 
                                              ntohl(request.len) - 1);
    if(NULL == daemon_request)
    {
        return NULL;
    }

    daemon_request->request.type = (buse_command_t)(ntohl(request.type));
    daemon_request->request.len = ntohl(request.len);
    daemon_request->request.from = ntohll(request.from);
    daemon_request->request.data = daemon_request->data;

    daemon_request->reply.magic = htonl(NBD_REPLY_MAGIC);
    daemon_request->reply.error = htonl(0);
    memcpy(daemon_request->reply.handle, request.handle, sizeof(daemon_request->reply.handle));
    
    if(BUSE_CMD_WRITE == daemon_request->request.type)
    {
        return_value = ReadData(nbd_socket, daemon_request->data, daemon_request->request.len);
        if(0 > return_value)
        {
            return NULL;
        }
    }

    return (buse_request_t *)daemon_request;
}

static int ReadData(int nbd_socket, char *data, int32_t data_len)
{
    ssize_t bytes_read = 0;
    size_t left_to_read = data_len;

    while(0 < left_to_read)
    {
        bytes_read = read(nbd_socket, data, left_to_read);
        if(0 > bytes_read)
        {
            return ERROR;
        }
        data += bytes_read;
        left_to_read -= bytes_read;
    }
    
    return SUCCESS; 
}

static int WriteData(int nbd_socket, char *data, int32_t data_len)
{
    ssize_t bytes_written = 0;
    size_t left_to_write = data_len;

    while(0 < left_to_write)
    {
        bytes_written = write(nbd_socket, data, left_to_write);

        if(0 > bytes_written)
        {
            return ERROR;
        }
        data += bytes_written;
        left_to_write -= bytes_written;
    }
    
    return SUCCESS; 
}

static int ReadRequest(int nbd_socket, struct nbd_request *request)
{
    ssize_t bytes_read = 0;
    size_t left_to_read = sizeof(*request);

    while(0 != left_to_read)
    {
        bytes_read = read(nbd_socket, request, left_to_read);
        if(0 > bytes_read)
        {
            return ERROR;
        }
        request = (struct nbd_request *)((char *)request + bytes_read);
        left_to_read -= bytes_read;
    }
    
    return SUCCESS; 
}

int NbdRequestDone(int nbd_socket, buse_request_t *requests)
{
    struct nbd_reply *reply = &(((daemon_request_t *)requests)->reply);
    int return_value = 0;
    size_t reply_len = 0;

    assert(nbd_socket > 0);
    assert(NULL != requests);
    reply_len = (BUSE_CMD_READ == requests->type) ? sizeof(struct nbd_reply) + requests->len : sizeof(struct nbd_reply);

   /* pthread_mutex_lock(&g_mutex);*/
    return_value = WriteData(nbd_socket, (char *)reply, reply_len);
    /*pthread_mutex_unlock(&g_mutex);  */
    
    free(requests);
    if(0 > return_value)
    {
        return ERROR;
    }    


    return 0;
}   