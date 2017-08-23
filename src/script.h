/**
*
* @file      script.h
* @brief     define script storage and interfaces, it must be thread safe
* @author    chenggang
* @History
*    1.0.0: chenggang,  2017/06/19,  Initial version
*/

#ifndef _SCRIPT_H
#define _SCRIPT_H

#include <string>
using std::string;
#include <stdint.h>

class TStep;
class TScript
{
public:
    TScript(const char* file);
    ~TScript();
    bool init();
    TStep* enter();
    bool exit();
private:
    string m_file;
    string m_lastModify;
    TStep* m_script;
    volatile bool m_bTobeRemoved;
    volatile uint64_t m_nUsing;
    volatile uint64_t m_nUsed;
};

#endif
