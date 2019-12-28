#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  string value;
  float memtotal;
  float memfree;

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          memtotal = std::stof(value);
        } else if (key == "MemFree:") {
          memfree = std::stof(value);
        }
        if (memtotal > 0 && memfree > 0) {
          return (memtotal-memfree)/memtotal;
        }
      }
    }
  }
  return 0;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string system, idle; 
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> system >> idle;
  }
  return std::stof(system);
 }

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  vector<string> jiffies = CpuUtilization();
  long totalJiffies;

  for (string jiffy : jiffies) {
    totalJiffies += std::atol(jiffy.c_str());
  }
  return totalJiffies;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid [[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  return Jiffies() - IdleJiffies();
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  vector<string> jiffies = CpuUtilization();
  long idle = std::atol(jiffies[3].c_str());
  long iowait = std::atol(jiffies[4].c_str());
  return idle + iowait;
 }

// TODO: Read and return CPU utilization
float LinuxParser::CpuUtilization(int pid) {
  std::stringstream ssStat;
  ssStat << kProcDirectory << pid << kStatFilename;

  std::stringstream ssUptime;
  ssUptime << kProcDirectory << kUptimeFilename;

  long hz = sysconf(_SC_CLK_TCK);

  string utime = ReadFile(ssStat.str(), 13);
  string stime = ReadFile(ssStat.str(), 14);
  string cutime = ReadFile(ssStat.str(), 15);
  string cstime = ReadFile(ssStat.str(), 16);
  string starttime = ReadFile(ssStat.str(), 21);
  string uptime = ReadFile(ssUptime.str(), 0);

  long lutime = std::stol(utime);
  long lstime = std::stol(stime);
  long lcutime = std::stol(cutime);
  long lcstime = std::stol(cstime);
  long lstarttime = std::stol(starttime);
  long luptime = std::stol(uptime);

  long total = lutime + lstime;
  total = total + lcutime +lcstime;
  float secs = luptime - (lstarttime/hz);
  float cpu = ((total / hz) / secs);

  if (cpu > 1) {
    cpu = 0;
  }
  return cpu;
}

vector<string> LinuxParser::CpuUtilization() {
  string desc, user, nice, system, idle, iowait, irq, softirq, steal;
  string line;

   std::ifstream stream(kProcDirectory + kStatFilename);
   if (stream.is_open()) {
     std::getline(stream, line);
     std::istringstream linestream(line);
     linestream >> desc >> user >> nice >> system >> idle >> iowait \
     >> irq >> softirq >> steal;
   }
   return vector<string>{user, nice, system, idle, iowait, irq, softirq, steal};
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return std::stoi(value);
        }
      }
    }
  }
  return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return std::stoi(value);
        }
      }
    }
  }
  return 0;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string line;
  string cmdline;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + "/cmdline");
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cmdline;
    return cmdline;
  }
  return 0;
 }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string ram = "VmSize";
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  while (std::getline(filestream, line)) {
    if (line.compare(0, ram.size(), ram) == 0) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      break;
    }
  }
  value = std::to_string(std::stof(value.c_str())/ 1024);
  return value.substr(0, 6);
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line;
  string key;
  string value;
  string uid;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      // std::replace(line.begin(), line.end(), " ", "_");
      // std::replace(line.begin(), line.end(), ":", " ");
      std::istringstream linestream(line);

      while (linestream >> key >> value) {
        if (key == "Uid:") {
          // std::replace(value.begin(), value.end(), "_", " ");
          return value;
        }
      }
      }
    }
    return "";  
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string uid = Uid(pid);
  string line;
  string user;
  string x;
  string process;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::replace(line.begin(), line.end(), 'x', ' ');
      std::istringstream linestream(line);
      linestream >> user >> process;
        if (process == uid) {
          return user;
        }
    }
  }
  return "";
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string comm, state, ppid, pgrp, session, tty_nr;
  string tgpid, flags, minflt, cminflt, majflt, utime, stime;
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr \
    >> tgpid >> flags >> minflt >> cminflt >> majflt >> utime >> stime;
  }
  return std::stof(stime) / sysconf(_SC_CLK_TCK);
 }

std::string LinuxParser::ReadFile(std::string path, unsigned int position) {
  string line, property, desc;
  std::ifstream stream(path);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    using strIt = std::istream_iterator<std::string>;
    std::vector<std::string> container{strIt{linestream}, strIt{}};
    
    if (container.size() >= position+1) {
      return container.at(position);
    }
  }
  return {};
}