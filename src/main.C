/**
*
* @file      main.C
* @brief     read ptf config
* @author    chenggang
* @History
*    1.0.0: chenggang,  2017/06/19,  Initial version
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ptfconfig.h"
#include "manager.h"
#include "mptconst.h"

int main(int argc, char* argv[])
{
    TPTFConfig::getInstance()->init("ptf.conf");
    TPTFConfig::getInstance()->print();
    TManager mgr;
    int ret = mgr.init();
    printf("ret:%d\n", ret);
    if(unlikely(0 != ret))
    {
        exit(1);
    }
    return 0;
}
