main:server

hiredis.o:hiredis.cpp
	g++ -c hiredis.cpp

server:fakeredis.o baseFunction.o redisFunc.o type.h
	g++ -o server fakeredis.o baseFunction.o redisFunc.o

redis.o:fakeredis.cpp type.h
	g++ -c fakeredis.cpp

baseFunction.o:baseFunction.cpp type.h
	g++ -c baseFunction.cpp

redisFunc.o:redisFunc.cpp type.h
	g++ -c redisFunc.cpp

sds.o:sds.cpp sds.h
	g++ -c sds.cpp
