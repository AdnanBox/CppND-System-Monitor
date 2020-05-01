#include "processor.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
    float epsilon{1e-8};
    long TotalTime = LinuxParser::Jiffies();
    long ActiveTime = LinuxParser::ActiveJiffies();
    long deltaTotal = TotalTime - prevTotalTime_;
    long deltaActive = ActiveTime - prevActiveTime_;

    CPU_Percentage_ = deltaActive/(deltaTotal + epsilon);
    AssignPrevValues(TotalTime, ActiveTime);
    return CPU_Percentage_;
}

void Processor::AssignPrevValues(long TotalTime, long ActiveTime){
    prevActiveTime_ = ActiveTime;
    prevTotalTime_ = TotalTime;
}
