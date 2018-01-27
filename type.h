#pragma once
#include <algorithm>
#include <arpa/inet.h>
#include <cstdio>
#include <stdlib.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include <errno.h>
#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <unordered_map>
using namespace std;

typedef struct serverReply serverReply;
typedef struct state state;
//typedef struct serverReply;
typedef struct fakeServer fakeServer;
typedef struct dataBase dataBase;
typedef struct recvData recvData;
typedef struct client client;

struct state{
  int state;
  char error;
  int clientFd;
};

struct serverReply {
  int type;
  int length;
  int count;
  char *str;
  // serverReply **element;
};

struct client {
  int clientFd;
  fakeServer *server;
  vector<recvData> *command;
};

struct dataBase {
  unordered_map<string, string> *data;
};

struct fakeServer {
  int sockFd;
  fd_set allFd;
  int fdMaxSize;
  list<client> *clients;
  dataBase data;
};

struct recvData {
  vector<string> *data;
};

const int REDIS_COMMAND_SET = 1;
const int REDIS_COMMAND_EXIST = 2;
const int REDIS_COMMAND_DEL = 3;
const int REDIS_COMMAND_GET = 4;
const int REDIS_REPLY_INTEGER = 1;
const int REDIS_REPLY_STRING = 2;
const int REDIS_REPLY_NIL = 3;
const int REDIS_REPLY_ERROR=4;
const int REDIS_REPLY_SET=5;
const int PORT=6379;
const int MAXSIZE=1024;
const int BACKLOG=10;
