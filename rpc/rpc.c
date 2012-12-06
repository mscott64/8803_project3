// rpc.c

#include <assert.h>
#include <constants.h>
#include <fcntl.h>
#include <lowres.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <rpc.h>
#include <shared.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int queue[Q_SIZE];
int num = 0;
int add = 0;
int rem = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;

void *boss(void *data);
void *worker(void *data);

int port_num = DEFAULT_RPC_ADDR;
int num_threads = NUM_THREADS_SERVER;

int main(int argc, char *argv[])
{
  int c;
  opterr = 0;
  while((c = getopt(argc, argv, "up:t:")) != -1)
  {
    switch(c)
    {
    case 'p':
      port_num = atoi(optarg);
      break;
    case't':
      num_threads = atoi(optarg);
      break;
    case '?':
      if(optopt == 'p' || optopt == 't')
	printf("Option -%c requires an argument\n", optopt);
      else
	printf("Unknown option -%c\n", optopt);
    case 'u':
    default:
      printf("Usage:\n%s [-p port_num] [-t num_threads]\n", argv[0]);
      printf("%s -u prints the usage\n", argv[0]);
      return 0;
    }
  }
  if(port_num < 1)
  {
    printf("Invalid port number\n");
    return 0;
  }
  if(num_threads < 1)
  {
    printf("Invalid number of threads\n");
    return 0;
  }
  
  rpc_create();
  return 0;
}

void rpc_create(void)
{
  pthread_t boss_thread;
  pthread_t workers[num_threads];
  int e, i;
  e = pthread_create(&boss_thread, NULL, boss, NULL);
  assert(e == 0);

  for(i = 0; i < num_threads; i++)
  {
    e = pthread_create(&workers[i], NULL, worker, NULL);
    assert(e == 0);
  }

  e = pthread_join(boss_thread, NULL);
  assert(e == 0);

  for(i = 0; i < num_threads; i++)
  {
    e = pthread_join(workers[i], NULL);
    assert(e == 0);
  }
}

void *boss(void *data)
{
  int hSocket, hServerSocket; /* handle to socket */
  struct sockaddr_in address; /* Internet socket address struct */
  int nAddressSize = sizeof(struct sockaddr_in);
  int nHostPort = port_num;

  hServerSocket = socket(AF_INET, SOCK_STREAM, 0);
  if(hServerSocket == SOCKET_ERROR)
  {
    printf("Could not make a socket\n");
    return NULL;
  }

  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(nHostPort);
  address.sin_family = AF_INET;

  if(bind(hServerSocket, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR)
  {
    printf("Could not connect to host\n");
    return NULL;
  }

  if(listen(hServerSocket, Q_SIZE) == SOCKET_ERROR)
  {
    printf("Could not listen\n");
    return NULL;
  }
  printf("Listening...\n");
  while(1)
  {
    hSocket = accept(hServerSocket, (struct sockaddr *)&address, (socklen_t *)&nAddressSize);
    pthread_mutex_lock(&lock);
    while(num == Q_SIZE)
      pthread_cond_wait(&full, &lock);
    queue[add] = hSocket;
    add = (add + 1) % Q_SIZE;
    num++;
    pthread_mutex_unlock(&lock);
    pthread_cond_signal(&empty);
  }
  return NULL;
}

void *worker(void *data)
{
  int hSocket;
  while(1)
  {
    char pBuffer[BUFFER_SIZE];
    char path[BUFFER_SIZE];
    char *scheme;
    char url[BUFFER_SIZE];
    char *hostname;
    
    pthread_mutex_lock(&lock);
    while(num == 0)
      pthread_cond_wait(&empty, &lock);
    hSocket = queue[rem];
    rem = (rem + 1) % Q_SIZE;
    num--;
    pthread_mutex_unlock(&lock);
    pthread_cond_signal(&full);
    
    /* Process information */ 
    read(hSocket, pBuffer, BUFFER_SIZE);
    
    key_t key;
    void *shmem;
    
    if(sscanf(pBuffer, "%[^ ] %d", url, &key) < 2)
    {
      send_error(hSocket, BAD_REQUEST, "Not the accepted protocol");
      continue;
    }
    parse_url(url, &scheme, &hostname, path);
    
    int fd = open(path, O_RDONLY);
    if(fd < 0)
    {
      send_error(hSocket, INTERNAL_ERROR, "Unable to open image file");
      continue;
    }
    
    /* Open the output shared memory segment */
    int id = shmget(key, 0, SVSHM_MODE);
    if(id < 0)
    {
      send_error(hSocket, INTERNAL_ERROR, "Unable to retrieve shared memory segment");
      continue;
    }
    
    shmem = shmat(id, NULL, 0);
    if(shmem == (void *)-1)
    {
      send_error(hSocket, INTERNAL_ERROR, "Unable to attach shared memory segment");
      continue;
    }

    /* Compress the image */
    int img_size;
    int e = change_res_JPEG(fd, (char **)&shmem, &img_size);
    shmdt(shmem);
    if(e == 0)
    {
      send_error(hSocket, INTERNAL_ERROR, "Unable to grab image");
      continue;
    } else {
      printf("New image size: %d\n", img_size);
      sprintf(pBuffer, "%s", SUCCESS);
      write(hSocket, pBuffer, strlen(pBuffer));
    }
    
    if(close(hSocket) == SOCKET_ERROR)
    {
      printf("Could not close socket\n");
      continue;
    }
  }
  return NULL;
}
