#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <hiredis/hiredis.h>

void doTest()
{
	//redis默认监听端口为6387 可以再配置文件中修改
	redisContext* c = redisConnect("127.0.0.1", 6379);
	if ( c->err)
	{
		redisFree(c);
		printf("Connect to redisServer faile\n");
		return ;
	}
	printf("Connect to redisServer Success\n");

	const char* command1 = "SET stest1 value1";
	redisReply* r = (redisReply*)redisCommand(c, command1);

	if( NULL == r)
	{
		printf("Execut command1 failure\n");
		redisFree(c);
		return;
	}
	if( !(r->type == REDIS_REPLY_STATUS && strcasecmp(r->str,"OK")==0))
	{
		printf("Failed to execute command[%s]\n",command1);
		freeReplyObject(r);
		redisFree(c);
		return;
	}
	freeReplyObject(r);
	printf("Succeed to execute command[%s]\n", command1);

	const char* command3 = "GET stest1";
	r = (redisReply*)redisCommand(c, command3);
	if ( r->type != REDIS_REPLY_STRING)
	{
		printf("Failed to execute command[%s]\n",command3);
		freeReplyObject(r);
		redisFree(c);
		return;
	}
	printf("The value of 'stest1' is %s\n", r->str);
	freeReplyObject(r);
	printf("Succeed to execute command[%s]\n", command3);

	const char* command4 = "GET stest2";
	r = (redisReply*)redisCommand(c, command4);
	if ( r->type != REDIS_REPLY_NIL)
	{
		printf("Failed to execute command[%s]\n",command4);
		freeReplyObject(r);
		redisFree(c);
		return;
	}
	freeReplyObject(r);
	printf("Succeed to execute command[%s]\n", command4);


	redisFree(c);

}

int main()
{
	doTest();
	return 0;
}
