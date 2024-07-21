/*
#include <stddef.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <linux/nbd.h>
#include <string.h>

#include "buse.h"

#define ERROR (-1)
#define MB (1 << 20)

int main() 
{

    const char *device_name = "/dev/nbd4";
    size_t size = 256 * MB;
    buse_request_t *curr_request = NULL;
    int return_value = 0;
    int counter = 0;
    int nbd = 0;

    char *IoT_buffer = (char *)malloc(size);
    if(NULL == IoT_buffer)
    {
        return 1;
    }

    nbd = NbdOpen(device_name, size);
    if (ERROR == nbd) 
    {
        printf("NBDOpen failed\n");
        return 1;
    }

    while(1)
    {
        curr_request = NbdGetRequest(nbd);
        switch(curr_request->type)
        {
            case BUSE_CMD_READ: 
            {
                memcpy(curr_request->data, IoT_buffer + curr_request->from, curr_request->len);
                printf("%d:\tread request\t%d bytes\n", counter++, curr_request->len);

                break;
            }
            
            case BUSE_CMD_WRITE: 
            {

                memcpy(IoT_buffer + curr_request->from, curr_request->data, curr_request->len);
                printf("%d:\twrite request\t%d bytes\n", counter++, curr_request->len);

                break;
            }

            default:
            {
                break;
            }
        }
        
        return_value = NbdRequestDone(nbd, curr_request);
        if(ERROR == return_value)
        {
            return 1;
        }

    }
    
    free(IoT_buffer);
    close(nbd);
   
    return 0;

}

