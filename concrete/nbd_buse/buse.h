#ifndef __BUSE_H__
#define __BUSE_H__

#ifdef __cplusplus
extern "C" {
#endif    

#include <stddef.h>    /*size_t*/
#include <stdint.h> /*uint64_t, uint32_t*/

typedef enum buse_command
{
	BUSE_CMD_READ,
	BUSE_CMD_WRITE,
	BUSE_CMD_DISC,
	BUSE_CMD_FLUSH,
	BUSE_CMD_TRIM
} buse_command_t;

typedef struct buse_request
{
	uint64_t from;
    char *data;
	int32_t len;
	buse_command_t type;
} buse_request_t;

int NbdOpen(const char *file_name, size_t size_in_bytes);

buse_request_t *NbdGetRequest(int nbd_socket);

int NbdRequestDone(int nbd_socket, buse_request_t *requests);

#ifdef __cplusplus
} // End of extern "C" block
#endif

#endif /* __BUSE_H__*/
