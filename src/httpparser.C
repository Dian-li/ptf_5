//
// Created by dian on 18-4-22.
//

#include "httpparser.h"
httpparser::httpparser() {}

httpparser::~httpparser() {}

void httpparser::parse(string str) {
    const string c = "\n";
    string::size_type pos1,pos2;
    pos1 = 0;
    pos2 = str.find(c);
    string header = str.substr(pos1,pos2-pos1);
    int pos4 = header.find(" ");
    if(pos4 != string::npos){
        m_type = header.substr(0,pos4);
        const string c2 = " ";
        int pos5 = header.find(c2,pos4+c2.size());
        if(pos5 != string::npos){
            m_url = header.substr(pos4+c2.size(),pos5-pos4);
        }
    }

    pos1 = pos2+c.size();
    pos2 = str.find(c,pos1);
    while(string::npos != pos2){
        string headerkv = str.substr(pos1,pos2-pos1);
        string::size_type pos3 = headerkv.find(":");
        if(pos3 != string::npos){
            string headerkey = headerkv.substr(0,pos3);
            string headervalue = headerkv.substr(pos3+2,headerkv.size()-pos3);
            m_headers.insert(std::pair<string,string>(headerkey,headervalue));
        }
        pos1 = pos2+c.size();
        pos2 = str.find(c,pos1);
    }

}
