#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "linux_parser.h"
#include <ctime>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  long prevTotalTime_{0}, prevActiveTime_{0};
  float CPU_Percentage_;

  void AssignPrevValues(long TotalTime, long ActiveTime);
};

#endif