cmake_minimum_required(VERSION 2.8.4)
project(ptf)

set(SOURCE_FILES httpevent.C  main.C  manager.C  ptfconfig.C  script.C  transaction.C
mptthreadpool.C step.C  tcpstep.C  stdoutstep.C tcpevent.C socketpool.C timeoutqueue.C
hashtable/hashtable.c  hashtable/hashtable_itr.c protocol.C  httpstep.C  httpparser.C jsoncpp.cpp)

add_custom_target(ptf COMMAND make -C ${ptf_SOURCE_DIR}
        CLION_EXE_DIR=${PROJECT_BINARY_DIR})