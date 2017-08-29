AUTOMAKE_OPTIONS=foreign 
bin_PROGRAMS=ptf
ptf_SOURCES=httpevent.C  main.C  manager.C  ptfconfig.C  script.C  transaction.C \
mptthreadpool.C step.C  tcpstep.C \
hashtable/hashtable.c  hashtable/hashtable_itr.c
AM_CPPFLAGS=-Ihashtable -I/usr/include/libxml2
LIBS=-lconfig -levent -lxml2
