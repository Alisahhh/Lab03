#include"type.h"

serverReply *redisSet(dataBase *pData, char *s1, char *s2);
//serverReply *redisDel(dataBase *pData, vector<string> s);
//serverReply *redisExists(dataBase *pData, vector<string> s);
serverReply *redisGet(dataBase *pData, char *s);
int getInt(char *recvString, int len);
serverReply *redisDel(dataBase *pData, char *s);
serverReply *redisExists(dataBase *pData, char *s);
