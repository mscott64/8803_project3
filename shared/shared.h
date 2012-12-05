#ifndef SHARED_H
#define SHARED_H

void send_error(int hSocket, char *error_msg, char *descrip);

int parse_url(char *url, char **scheme, char **hostname, char *path);

#endif /* SHARED_H */
