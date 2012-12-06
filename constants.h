#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <sys/ipc.h>
#include <sys/shm.h>

#define DEFAULT_PORT_NUM 1234
#define DEFAULT_PROXY_ADDR 5678
#define DEFAULT_RPC_ADDR 9012
#define DEFAULT_MACHINE "michelle-VirtualBox"
#define DEFAULT_REQUESTS 500
#define DEFAULT_NUM_FILES 50
#define GET "GET http://michelle-VirtualBox:1234/files/file"
#define NUM_THREADS_CLIENT 5
#define NUM_THREADS_SERVER 10
#define Q_SIZE 1000
#define SOCKET_ERROR -1
#define BUFFER_SIZE 10000
#define NOT_FOUND "404 Not Found "
#define BAD_REQUEST "400 Bad Request "
#define INTERNAL_ERROR "500 Internal Error "
#define NOT_IMPLEMENTED "501 Method not implemented "
#define HOST_NAME_SIZE 1000
#define LOCAL_IP_ADDR "127.0.1.1"
#define LOCAL_GET "LOCAL_GET"
#define SUCCESS "Successful write"
#define SHMEM_PATH "."
#define SVSHM_MODE SHM_R | SHM_W | (SHM_R>>3) | (SHM_W>>3) | (SHM_R>>6) | (SHM_W>>6)

#endif /* CONSTANTS_H */
