#include "hiredis.h"


int main(){
  redisContext *c = redisConnect("127.0.0.1", 6379);
  if (c != NULL && c->err) {
      printf("Error: %s\n", c->errstr);
      // handle error
    } else {
      printf("Connected to Redis\n");
    }
    redisReply *reply;
    reply = (redisReply *)redisCommand(c,"SET %s %s","foo","bar");
    freeReplyObject(reply);
    reply = (redisReply *)redisCommand(c,"SET %s %s","foo","bar1");
    freeReplyObject(reply);
    reply = (redisReply *)redisCommand(c,"GET %s","foo");
    printf("%s\n",reply->str);
    freeReplyObject(reply);
    redisFree(c);
}
