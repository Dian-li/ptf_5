AUTOMAKE_OPTIONS=foreign 
bin_PROGRAMS=ptf
ptf_SOURCES=httpevent.C  main.C  manager.C  ptfconfig.C  script.C  transaction.C \
mptthreadpool.C step.C  tcpstep.C  stdoutstep.C tcpevent.C socketpool.C timeoutqueue.C\
hashtable/hashtable.c  hashtable/hashtable_itr.c protocol.C  httpstep.C  httpparser.cpp jsoncpp.cpp
AM_CPPFLAGS=-Ihashtable -I /usr/local/include/libxml2 -L /usr/local/lib -g -std=c++11
LIBS=-lconfig -levent -lxml2 -lpthread  -ldl -lhttp_parser -lhiredis
