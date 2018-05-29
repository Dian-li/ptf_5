/*
 * testcase.h
 *
 *  Created on: 2018年5月29日
 *      Author: dian
 */

#ifndef SRC_MODEL_TESTCASE_H_
#define SRC_MODEL_TESTCASE_H_

#include <cstring>

using  std::string;

class Testcase {
 public:
    int tps;
    int duration;
    int concurrenceMode;
    int timedelay;
    float threshold;
    int pressureMode;
    string protocol;
    string filename;
    string file;
    int step;
    int targetTps;

 private:

 public:
    Testcase(){}
    ~Testcase(){}

 private:
};

#endif /* SRC_MODEL_TESTCASE_H_ */
