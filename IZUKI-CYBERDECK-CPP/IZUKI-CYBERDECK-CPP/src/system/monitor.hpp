#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/statvfs.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#endif
#include <thread>
#include <chrono>
namespace izuki::sys {
struct Stats{ double cpu=-1, ram=-1, disk=-1, uptime=-1; std::string ip; };
#ifdef _WIN32
inline unsigned long long filetime_value(const FILETIME&t){ return (static_cast<unsigned long long>(t.dwHighDateTime)<<32)|t.dwLowDateTime; }
inline bool cpu_times(unsigned long long&idle,unsigned long long&total){ FILETIME i,k,u; if(!GetSystemTimes(&i,&k,&u)) return false; idle=filetime_value(i); total=filetime_value(k)+filetime_value(u); return true; }
inline Stats snapshot(int sample_ms=200){ Stats r; unsigned long long i1,t1,i2,t2;
  if(cpu_times(i1,t1)){ Sleep(static_cast<DWORD>(sample_ms)); if(cpu_times(i2,t2)&&t2>t1) r.cpu=100.0*(1.0-double(i2-i1)/double(t2-t1)); }
  MEMORYSTATUSEX m{}; m.dwLength=sizeof(m); if(GlobalMemoryStatusEx(&m)&&m.ullTotalPhys) r.ram=100.0*double(m.ullAvailPhys)/double(m.ullTotalPhys), r.ram=100.0-r.ram;
  ULARGE_INTEGER free{},total{},avail{}; if(GetDiskFreeSpaceExA("C:\\",&free,&total,&avail)&&total.QuadPart) r.disk=100.0*(1.0-double(avail.QuadPart)/double(total.QuadPart));
  r.uptime=double(GetTickCount64())/1000.0;
  char host[NI_MAXHOST]={}; WSADATA wsa{}; if(WSAStartup(MAKEWORD(2,2),&wsa)==0){ if(gethostname(host,sizeof host)==0){ addrinfo h{},*res=nullptr; h.ai_family=AF_INET; h.ai_socktype=SOCK_STREAM; if(getaddrinfo(host,nullptr,&h,&res)==0){ for(auto*p=res;p;p=p->ai_next){ char ip[INET_ADDRSTRLEN]={}; auto*sin=reinterpret_cast<sockaddr_in*>(p->ai_addr); inet_ntop(AF_INET,&sin->sin_addr,ip,sizeof ip); if(std::string(ip)!="127.0.0.1"){ r.ip=ip; break; } } freeaddrinfo(res); } } WSACleanup(); }
  return r; }
inline std::vector<std::string> resolve(const std::string& host){ std::vector<std::string> o; WSADATA wsa{}; if(WSAStartup(MAKEWORD(2,2),&wsa)!=0) return o; addrinfo h{},*res=nullptr; h.ai_socktype=SOCK_STREAM; if(getaddrinfo(host.c_str(),nullptr,&h,&res)!=0){ WSACleanup(); return o; } for(auto*p=res;p;p=p->ai_next){ char b[INET6_ADDRSTRLEN]={}; if(p->ai_family==AF_INET){ auto*sin=reinterpret_cast<sockaddr_in*>(p->ai_addr); inet_ntop(AF_INET,&sin->sin_addr,b,sizeof b); } else { auto*sin=reinterpret_cast<sockaddr_in6*>(p->ai_addr); inet_ntop(AF_INET6,&sin->sin6_addr,b,sizeof b); } o.push_back(b); } freeaddrinfo(res); WSACleanup(); return o; }
#else
inline bool cpu_times(unsigned long long&idle,unsigned long long&total){ std::ifstream f("/proc/stat"); std::string l; if(!std::getline(f,l)) return false; std::istringstream s(l); std::string k; s>>k; unsigned long long v,t=0; std::vector<unsigned long long> x; while(s>>v){x.push_back(v);t+=v;} if(x.size()<5) return false; idle=x[3]+x[4]; total=t; return true; }
inline Stats snapshot(int sample_ms=200){ Stats r; unsigned long long i1,t1,i2,t2;
  if(cpu_times(i1,t1)){ std::this_thread::sleep_for(std::chrono::milliseconds(sample_ms)); if(cpu_times(i2,t2)&&t2>t1) r.cpu=100.0*(1.0-double(i2-i1)/double(t2-t1)); }
  std::ifstream m("/proc/meminfo"); std::string k,u; double tot=0,av=0; unsigned long long v; while(m>>k>>v>>u){ if(k=="MemTotal:") tot=double(v); if(k=="MemAvailable:") av=double(v); }
  if(tot>0) r.ram=100.0*(1.0-av/tot);
  struct statvfs sv; if(statvfs("/",&sv)==0&&sv.f_blocks) r.disk=100.0*(1.0-double(sv.f_bavail)/double(sv.f_blocks));
  std::ifstream up("/proc/uptime"); double u0; if(up>>u0) r.uptime=u0;
  ifaddrs*a=nullptr; if(getifaddrs(&a)==0){ for(auto*p=a;p;p=p->ifa_next){ if(p->ifa_addr&&p->ifa_addr->sa_family==AF_INET&&(p->ifa_flags&IFF_UP)&&!(p->ifa_flags&IFF_LOOPBACK)){ char b[INET_ADDRSTRLEN]; inet_ntop(AF_INET,&((sockaddr_in*)p->ifa_addr)->sin_addr,b,sizeof b); r.ip=b; break; } } freeifaddrs(a); }
  return r; }
inline std::vector<std::string> resolve(const std::string& host){ std::vector<std::string> o; addrinfo h{},*res=nullptr; h.ai_socktype=SOCK_STREAM; if(getaddrinfo(host.c_str(),nullptr,&h,&res)!=0) return o; for(auto*p=res;p;p=p->ai_next){ char b[INET6_ADDRSTRLEN]; void*ad=p->ai_family==AF_INET?(void*)&((sockaddr_in*)p->ai_addr)->sin_addr:(void*)&((sockaddr_in6*)p->ai_addr)->sin6_addr; inet_ntop(p->ai_family,ad,b,sizeof b); o.push_back(b);} freeaddrinfo(res); return o; }
#endif
}
