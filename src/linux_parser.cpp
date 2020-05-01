#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "format.h"

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
  string os, kernel, version;
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
  string memoryCategory;
  float memorySize;
  float memoryTotal, memFree;
  string line;
  std::ifstream stream(kProcDirectory+kMeminfoFilename);
  if (stream.is_open()){
    while (std::getline(stream, line)){
      std::istringstream linestream(line);
      while (linestream >> memoryCategory >> memorySize){
        if (memoryCategory=="MemTotal:"){
          memoryTotal = memorySize;
        }
        else if (memoryCategory=="MemFree:"){
          memFree = memorySize;
        }      
      }
    }
  }
  return (memoryTotal-memFree)/memoryTotal; 
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  long up_time;
  long idle_time;
  string line;
  std::ifstream stream(kProcDirectory+kUptimeFilename);
  if (stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> up_time >> idle_time;
  }
  return up_time;
}
// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return ActiveJiffies() + IdleJiffies();
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  vector<CPUStates> active_states {kUser_ , kNice_, kSystem_, kIRQ_, kSoftIRQ_, kSteal_} ;
  vector<string> utilization = LinuxParser::ReadSystemFile();
  long active_time = 0;
  for (auto state: active_states){
    active_time += std::stol(utilization[state]);
  }
  return active_time;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  vector<CPUStates> idle_states {kIdle_, kIOwait_} ;
  vector<string> utilization = LinuxParser::ReadSystemFile();
  long idle_time = 0;
  for (auto state: idle_states){
    idle_time += std::stol(utilization[state]);
  }
  return idle_time;
}

// TODO: Read and return CPU utilization for a process
float LinuxParser::CpuUtilization(int pid) {
  vector<string> CpuUtilization = LinuxParser::ReadPidFile(pid);
  long uptime, utime, stime, cutime, cstime, starttime, totaltime;
  uptime = LinuxParser::UpTime();
  utime = std::stol(CpuUtilization[13]);
  stime = std::stol(CpuUtilization[14]);
  cutime = std::stol(CpuUtilization[15]);
  cstime = std::stol(CpuUtilization[16]);
  starttime = std::stol(CpuUtilization[21]);
  starttime = starttime/sysconf(_SC_CLK_TCK);
  totaltime = (utime + stime + cutime + cstime)/sysconf(_SC_CLK_TCK);

  return 100*totaltime/(uptime-starttime+1e-8); 
}

//Read System CPU Utilization
vector<string> LinuxParser::ReadSystemFile() {
  string line, value;
  vector<string> utilization{};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()){
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      while (linestream >> value){
        if (value!="cpu"){
          utilization.emplace_back(value);
        }
      }
    }
  }
  return utilization;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string processes;
  int totalProcesses;
  string line;
  std::ifstream stream(kProcDirectory+kStatFilename);
  if (stream.is_open()){
    while (std::getline(stream, line)){
      std::istringstream linestream(line);
      while (linestream >> processes >> totalProcesses){
        if (processes=="processes"){
          return totalProcesses;
        }
      }  
    } 
  }
  return totalProcesses;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string procs_running;
  int runningProcesses;
  string line;
  std::ifstream stream(kProcDirectory+kStatFilename);
  if (stream.is_open()){
    while (std::getline(stream, line)){
      std::istringstream linestream(line);
      while (linestream >> procs_running >> runningProcesses){
        if (procs_running=="procs_running"){
          return runningProcesses;
        }
      }  
    } 
  }
  return runningProcesses;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string line; 
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()){
    std::getline(stream, line);
    return line;
  }
  return line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string key, value, line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()){
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      while (linestream >> key >> value){
        if (key=="VmSize:"){
          return Format::kB2MB(std::stol(value));
        }
      }
    }
  }
  return Format::kB2MB(std::stol(value));
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line, key, Uid;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()){
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> Uid){
        if (key == "Uid")
          return Uid;
      }
    }
  }
  return Uid;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string line, user, x, Uid;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()){
    while (std::getline(stream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> x >> Uid){
        if (LinuxParser::Uid(pid)==Uid){
          return user;
        }
      }
    }
  }
  return user;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  vector<string> PidStat = LinuxParser::ReadPidFile(pid);
  long UpTime = std::stol(PidStat[21]);    //[22] value in /proc/[pid]/stat is start_time. So, index=21
  UpTime = LinuxParser::UpTime() - UpTime/sysconf(_SC_CLK_TCK);    //convert to seconds
  return UpTime;
}

vector<string> LinuxParser::ReadPidFile(int pid){
  string line, value;
  vector<string> PidStat {};
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()){
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      while (linestream >> value){
        PidStat.emplace_back(value);
      }
    }
  }
  return PidStat;
}