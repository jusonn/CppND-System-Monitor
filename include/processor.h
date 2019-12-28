#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
 long prev_user_;
 long prev_nice_;
 long prev_system_;
 long prev_idle_;
 long prev_iowait_;
 long prev_irq_;
 long prev_softirq_;
 long prev_steal_;
};

#endif