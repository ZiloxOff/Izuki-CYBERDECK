#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
namespace izuki::blue {
struct Conn{ std::string local; unsigned lport=0, rport=0; std::string state; };
inline std::vector<Conn> tcp_connections(const std::string& path="/proc/net/tcp"){ std::vector<Conn> o;
#ifdef _WIN32
  (void)path;
  return o;
#else
 std::ifstream f(path); std::string l; std::getline(f,l);
  while(std::getline(f,l)){ std::istringstream s(l); std::string idx,loc,rem,st; s>>idx>>loc>>rem>>st; auto c=loc.find(':'), d=rem.find(':'); if(c==std::string::npos||d==std::string::npos) continue;
    Conn x; x.lport=std::stoul(loc.substr(c+1),nullptr,16); x.rport=std::stoul(rem.substr(d+1),nullptr,16); x.local=loc; x.state=st=="0A"?"LISTEN":st=="01"?"ESTABLISHED":st; o.push_back(x);} return o;
#endif
}
inline std::vector<std::string> alerts(const std::vector<Conn>& cs){ static const std::set<unsigned> bad={23,4444,31337}; std::vector<std::string> o; for(auto&c:cs) if(bad.count(c.rport)||(c.state=="LISTEN"&&bad.count(c.lport))) o.push_back("suspect_port local="+std::to_string(c.lport)+" remote="+std::to_string(c.rport)+" "+c.state); return o; }
}
