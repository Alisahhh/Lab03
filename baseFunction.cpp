#include "redisFunc.h"
#include "type.h"
#include <cassert>
#include <errno.h>
fakeServer *buildServer();
state *redisConnect(char *ip, int PORT);
client *clientinit(int fd, fakeServer *server);
void reciveAndSend(fakeServer *server);
void networkRead(list<client>::iterator iter);

fakeServer *buildServer() {
  fakeServer *tmp;
  tmp = new fakeServer;
  if (tmp == nullptr) {
    return nullptr;
  }
  tmp->sockFd = socket(AF_INET, SOCK_STREAM, 0);
  if (tmp->sockFd == -1) {
    delete tmp;
    return nullptr;
  }
  sockaddr_in myAddr;
  memset(&myAddr, 0, sizeof(myAddr));
  myAddr.sin_family = AF_INET;
  myAddr.sin_port = htons(PORT);
  myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(tmp->sockFd, (sockaddr *)&myAddr, sizeof(myAddr)) == -1) {
    delete tmp;
    cerr << strerror(errno) << endl;
    assert(false);
    return nullptr;
  }
  if (listen(tmp->sockFd, BACKLOG) == -1) {
    free(tmp);
    return nullptr;
  }
  FD_ZERO(&tmp->allFd);
  //cout<<"socket"<<tmp->sockFd<<endl;
  FD_SET(tmp->sockFd, &tmp->allFd);
  tmp->fdMaxSize = tmp->sockFd;
  tmp->clients = new list<client>();
  tmp->data.data = new unordered_map<string, string>();
  return tmp;
}

void reciveAndSend(fakeServer *server) {
  static sockaddr_in peer;
  socklen_t length = sizeof(peer);
  //cout<<"begin select"<<endl;
  //listen(server->sockFd, BACKLOG);
  FD_SET(server->sockFd,&server->allFd);
  int ready = select(server->fdMaxSize + 1, &server->allFd, nullptr, nullptr, nullptr);
  if (ready <= 0)
    return;
  if (FD_ISSET(server->sockFd, &(server->allFd))) {
    int clientFd = accept(server->sockFd, (sockaddr *)&peer, &length);
    if (clientFd == -1)
      return;
    //cout << "accept succeed" << endl;
    FD_SET(clientFd, &(server->allFd));
    if (clientFd > server->fdMaxSize)
      server->fdMaxSize = clientFd;
    client *C = clientinit(clientFd, server);
    server->clients->push_back(*C);
    if(ready==1) return;
  }
  cout<<server->clients->size()<<endl;
  for (auto iter = server->clients->begin(); iter != server->clients->end();) {
    if(FD_ISSET(iter->clientFd,&server->allFd)){
      auto next=iter;
      next++;
      networkRead(iter);
      iter=next;
    }
  }
};

void redisSend(list<client>::iterator iter, serverReply *reply) {
  switch (reply->type) {
  case REDIS_REPLY_NIL: {
    int length = 10;
    char *buf;
    buf = (char *)malloc(length);
    memcpy(buf, "$-1\r\n", sizeof("$-1\r\n"));
    send(iter->clientFd, buf, sizeof(buf), 0);
    free(buf);
    break;
  }
  case REDIS_REPLY_ERROR: {
    send(iter->clientFd, "-\r\n", 3, 0);
    break;
  }
  case REDIS_REPLY_SET:{
    send(iter->clientFd, "+OK\r\n", 5, 0);
    break;
  }
  case REDIS_REPLY_STRING: {
    int length = 1;
    length = 9+reply->length;
    char *buf;
    buf = (char *)malloc(length);
    memcpy(buf, "$", sizeof("$"));
    buf[1]='0'+reply->length;//length <=9 unfinish
    buf[2]='\r';buf[3]='\n';
    memcpy(buf + 4, reply->str, reply->length);
    memcpy(buf + 4 + reply->length, "\r\n", sizeof("\r\n"));
    int num = send(iter->clientFd, buf, length, 0);
    if(num==-1) cout<<"send"<<endl;
    free(buf);
    break;
  }
  case REDIS_REPLY_INTEGER: {
    char *buf;
    buf=(char *)malloc(10);
    buf[0]=':';
    //memcpy(buf+1,&reply->count,4);
    buf[1]='1';
    buf[2]='\r';
    buf[3]='\n';
    int num = send(iter->clientFd, buf, sizeof(buf), 0);
    if(num==-1) cout<<"send"<<endl;
    free(buf);
    break;
  }
  }
}

void networkRead(list<client>::iterator iter) {
  serverReply *reply;
  recvData *data;
  char *buf;
  reply = (serverReply *)malloc(sizeof(serverReply));
  buf = (char *)malloc(MAXSIZE * sizeof(char));
    int numbyte = recv(iter->clientFd, buf, MAXSIZE - 1, 0);
    if (numbyte == -1) {

      return;
    }
    if (numbyte == 0) {
      close(iter->clientFd);
      (*iter).server->clients->erase(iter);
      FD_CLR(iter->clientFd, &(*iter).server->allFd);
      free(reply);
      free(buf);
      return;
    }
    buf[numbyte + 1] = '\0';
    char ch[5] = "";
    ch[0] = '\r';
    ch[1] = '\n';
    char *addr = strstr(buf, ch);
    int loopTot = getInt(buf + 1, addr - buf - 1);
    buf = addr + 2;
    addr = strstr(buf, ch);
    int length = getInt(buf + 1, addr - buf - 1);
    buf = addr + 2;
    char opt[10] = "";
    memcpy(opt, buf, length);
    addr = strstr(buf, ch);
    buf = addr + 2;
    if (opt[0] == 'S') {
      addr = strstr(buf, ch);
      length = getInt(buf + 1, addr - buf - 1);
      cout << "lengths1" << length << endl;
      buf = addr + 2;
      char *s1 = (char *)malloc(length + 1);
      memcpy(s1, buf, length);
      s1[length] = '\0';
      addr = strstr(buf, ch);
      buf = addr + 2;
      addr = strstr(buf, ch);
      length = getInt(buf + 1, addr - buf - 1);
      buf = addr + 2;
      char *s2 = (char *)malloc(length + 1);
      memcpy(s2, buf, length);
      s2[length] = '\0';
      addr = strstr(buf, ch);
      buf = addr + 2;
      reply = redisSet(&(iter->server->data), s1, s2);
      free(s1);
      free(s2);
    } else if (opt[0] == 'G') {
      addr = strstr(buf, ch);
      length = getInt(buf + 1, addr - buf - 1);
      buf = addr + 2;
      char *s = (char *)malloc(length + 1);
      memcpy(s, buf, length);
      s[length] = '\0';
      cout << s << endl;
      addr = strstr(buf, ch);
      buf = addr + 2;
      reply = redisGet(&(iter->server->data), s);
      free(s);
    } else if (opt[0] == 'E') {
      addr = strstr(buf, ch);
      length = getInt(buf + 1, addr - buf - 1);
      cout<<length<<endl;
      cout<<buf<<endl;
      buf = addr + 2;
      cout<<buf<<endl;
      char *s = (char *)malloc(length + 1);
      memcpy(s, buf, length);
      reply = redisExists(&(iter->server->data), s);
      free(s);
    } else if (opt[0] == 'D') {
      addr = strstr(buf, ch);
      length = getInt(buf + 1, addr - buf - 1);
      buf = addr + 2;
      char *s = (char *)malloc(length + 1);
      memcpy(s, buf, length);
      s[length] = '\0';
      cout << s << endl;
      addr = strstr(buf, ch);
      buf = addr + 2;
      reply = redisDel(&(iter->server->data), s);
      free(s);
    }
    redisSend(iter, reply);
    free(reply);
  //}
}

client *clientinit(int fd, fakeServer *server) {
  client *tmp;
  tmp = (client *)malloc(sizeof(client));
  tmp->clientFd = fd;
  tmp->server = server;
  tmp->command = new vector<recvData>();
  return tmp;
}
