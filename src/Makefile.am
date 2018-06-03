AUTOMAKE_OPTIONS=foreign 
bin_PROGRAMS=ptf
ptf_SOURCES=httpevent.C  main.C  manager.C  ptfconfig.C  script.C  transaction.C \
mptthreadpool.C step.C  tcpstep.C  stdoutstep.C tcpevent.C socketpool.C timeoutqueue.C\
hashtable/hashtable.c  hashtable/hashtable_itr.c protocol.C  httpstep.C  httpparser.C jsoncpp.cpp
#AM_CPPFLAGS=-Ihashtable -I/usr/local/opt/libxml2/include/libxml2 -g -std=c++11
AM_CXXFLAGS=-Ihashtable -I/usr/local/opt/libxml2/include/libxml2 -g -Wall -std=c++11
LIBS=-lconfig -levent -lxml2 -lpthread  -ldl  -lhiredis
#-lhttp_parser
