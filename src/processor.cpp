#include <vector>
#include <string>
#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
    std::vector<std::string> utils = LinuxParser::CpuUtilization();
    
    long user = std::stol(utils[0]);
    long nice = std::stol(utils[1]);
    long system = std::stol(utils[2]);
    long idle = std::stol(utils[3]);
    long iowait = std::stol(utils[4]);
    long irq = std::stol(utils[5]);
    long softirq = std::stol(utils[6]);
    long steal = stol(utils[7]);
    
    long prevIdle = prev_idle_ + prev_iowait_;
    long Idle = idle+ iowait;

    long prevNonIdle = prev_user_ + prev_nice_ + prev_system_ + prev_irq_ \
                        + prev_softirq_ + prev_steal_; 
    long nonIdle = user + nice + system + irq + softirq + steal;

    long prevTotal = prev_idle_ + prevNonIdle;
    long total = Idle + nonIdle;

    float delta_total = total - prevTotal;
    float delta_idle = Idle - prevIdle;
    
    prev_user_ = user;
    prev_nice_ = nice;
    prev_system_ = system;
    prev_idle_ = idle;
    prev_iowait_ = iowait;
    prev_irq_ = irq;
    prev_softirq_ = softirq;
    prev_steal_ = steal;

    return (delta_total - delta_idle) / delta_total;
}