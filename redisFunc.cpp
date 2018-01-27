#include"type.h"

serverReply *redisSet(dataBase *pData, char *s1, char *s2);
serverReply *redisDel(dataBase *pData, char *s);
serverReply *redisExists(dataBase *pData, char *s);
serverReply *redisGet(dataBase *pData, char *s);
int getInt(char *recvString, int len);


serverReply *redisSet(dataBase *pData, char *s1, char *s2) {
  serverReply *tmp;
  tmp = (serverReply *)malloc(sizeof(serverReply));
  tmp->type = REDIS_REPLY_SET;
  (*pData->data)[s1] = s2;
  tmp->str = (char *)malloc(sizeof("OK"));
  memcpy(tmp->str, "OK", sizeof("OK"));
  tmp->length = 2;
  return tmp;
}

serverReply *redisGet(dataBase *pData, char *s) {
  serverReply *tmp;
  tmp = (serverReply*)malloc(sizeof(serverReply));
  auto iter = pData->data->find(s);
  if (iter == pData->data->end()) {
    tmp->type = REDIS_REPLY_NIL;
    return tmp;
  }
  tmp->type = REDIS_REPLY_STRING;
  tmp->str = (char *)malloc(sizeof(iter->second.data()));
  tmp->length = iter->second.size();
  memcpy(tmp->str, (*iter).second.data(), (*iter).second.size());
  return tmp;
}

serverReply *redisExists(dataBase *pData, char *s) {
  serverReply *tmp;
  tmp = (serverReply*)malloc(sizeof(serverReply));
  tmp->type = REDIS_REPLY_INTEGER;
  tmp->count = 0;
  //cout<<s<<endl;
    if (pData->data->find(s) != pData->data->end()) {
      (tmp->count)++;
    }
  //cout<<s<<endl;
  return tmp;
}

serverReply *redisDel(dataBase *pData, char *s) {
  /*serverReply *tmp;
  tmp = (serverReply*)malloc(sizeof(serverReply));
  tmp->type = REDIS_REPLY_INTEGER;
  tmp->count = 0;
  for (int i= 0; i < s.size(); i++) {
    if (pData->data->find(s[i]) != pData->data->end()) {
      pData->data->erase(s[i]);
      (tmp->count)++;
    }
  }
  return tmp;*/
  serverReply *tmp;
  tmp = (serverReply*)malloc(sizeof(serverReply));
  tmp->type = REDIS_REPLY_INTEGER;
  tmp->count = 0;
    if (pData->data->find(s) != pData->data->end()) {
      (tmp->count)++;
    }
  return tmp;
}

int getInt(char *recvString, int len) {
  int num = 0;
  //cout<<len<<endl;
  for (int i = 0; i < len; i++) {
    //cout<<*(recvString+i)<<endl;
    num = num * 10 + *(recvString+i) - '0';
  }
  return num;
}
