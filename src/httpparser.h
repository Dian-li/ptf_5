//
// Created by dian on 18-4-22.
//

#ifndef STRINGEXCHANGE_HTTPPARSER_H
#define STRINGEXCHANGE_HTTPPARSER_H

#include <map>
#include <cstring>
using std::map;
using std::string;

class httpparser{

public:
    httpparser();
    ~httpparser();
    string m_type;
    string m_url;
    map<string,string> m_headers;
    //string m_body;

    void parse(string str);
};

#endif //STRINGEXCHANGE_HTTPPARSER_H
