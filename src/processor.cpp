#include <vector>
#include <string>
#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
    long total = LinuxParser::Jiffies();
    long idle = LinuxParser::IdleJiffies();
    float deltaTotal = total - prevTotal_;
    float deltaIdle = idle - prevIdle_;
    float deltaUsage = (deltaTotal - deltaIdle)/deltaTotal;
    prevIdle_ = idle;
    prevTotal_ = total;
    return deltaUsage;
}