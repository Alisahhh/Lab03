#include"type.h"

fakeServer *buildServer();
state *redisConnect(char *ip, int PORT);
client *clientinit(int fd, fakeServer *server);
int getInt(char *recvString, int len);
void reciveAndSend(fakeServer *server);
void networkRead(list<client>::iterator iter);
