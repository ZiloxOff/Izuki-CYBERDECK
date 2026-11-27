#include <iostream>
#include "modules/utilities.hpp"
#include "modules/blue_team.hpp"
#include "system/monitor.hpp"
#include "core/store.hpp"
#include "core/plugins.hpp"
#include <sstream>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
using namespace izuki;
int main(int argc,char**argv){ std::vector<std::string> a(argv+1,argv+argc);
  auto usage=[] {
    std::cout<<"IZUKI CYBERDECK 0.1.0 Alpha (C++)\nusage: cyberdeck <sha256|b64|b64d|hex|hexd|url|urld|json|regex|count|pass|uuid|ts|stats|resolve|conns|plugins|plugin> [args]\n";
    return 1;
  };
  core::History h("cyberdeck.history");
  auto arg=[](const std::vector<std::string>&v,size_t i){ return v.size()>i?v[i]:std::string(); };
  auto execute=[&](const std::vector<std::string>&args)->int {
    if(args.empty()) return usage();
    const std::string c=args[0]; std::string o;
    try {
    if(c=="sha256") std::cout<<util::sha256(arg(args,1))<<"\n";
    else if(c=="b64") std::cout<<util::b64enc(arg(args,1))<<"\n";
    else if(c=="b64d"){ if(!util::b64dec(arg(args,1),o)){ std::cerr<<"invalid base64\n"; return 2;} std::cout<<o<<"\n"; }
    else if(c=="hex") std::cout<<util::hexenc(arg(args,1))<<"\n";
    else if(c=="hexd"){ if(!util::hexdec(arg(args,1),o)){ std::cerr<<"invalid hex\n"; return 2;} std::cout<<o<<"\n"; }
    else if(c=="url") std::cout<<util::urlenc(arg(args,1))<<"\n"; else if(c=="urld") std::cout<<util::urldec(arg(args,1))<<"\n";
    else if(c=="json"){ bool v=util::json_valid(arg(args,1)); std::cout<<(v?"valid":"invalid")<<"\n"; return v?0:2; }
    else if(c=="regex") for(auto&m:util::regex_test(arg(args,1),arg(args,2))) std::cout<<m<<"\n";
    else if(c=="count"){ auto k=util::count(arg(args,1)); std::cout<<"chars="<<k.chars<<" words="<<k.words<<" lines="<<k.lines<<"\n"; }
    else if(c=="pass") std::cout<<util::password(args.size()>1?std::stoul(args[1]):20)<<"\n";
    else if(c=="uuid") std::cout<<util::uuid4()<<"\n";
    else if(c=="ts") std::cout<<util::ts_to_iso(std::stoll(arg(args,1)))<<"\n";
    else if(c=="stats"){ auto s=sys::snapshot(); std::cout<<"cpu="<<s.cpu<<"% ram="<<s.ram<<"% disk="<<s.disk<<"% uptime="<<s.uptime<<"s ip="<<(s.ip.empty()?"n/a":s.ip)<<" gpu=unavailable(not implemented)\n"; }
    else if(c=="resolve"){ auto r=sys::resolve(arg(args,1)); if(r.empty()){ std::cerr<<"resolution failed\n"; return 2;} for(auto&x:r) std::cout<<x<<"\n"; h.log("network","resolve",arg(args,1)); }
    else if(c=="conns"){ auto cs=blue::tcp_connections(); for(auto&x:cs) std::cout<<x.local<<" -> :"<<x.rport<<" "<<x.state<<"\n"; for(auto&al:blue::alerts(cs)) std::cout<<"ALERT "<<al<<"\n"; }
    else if(c=="plugins") for(auto&p:core::discover("plugins")) std::cout<<p<<"\n";
    else if(c=="plugin"){ int rc=core::run("plugins",arg(args,1),arg(args,2),o); std::cout<<o; return rc==0?0:2; }
    else if(c=="help") return usage();
    else { std::cerr<<"Commande inconnue: "<<c<<"\n"; return usage(); }
    }catch(const std::exception&e){ std::cerr<<"error: "<<e.what()<<"\n"; return 2; }
    return 0;
  };

  if(a.empty()){
#ifdef _WIN32
    HANDLE console=GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode=0;
    if(console!=INVALID_HANDLE_VALUE&&GetConsoleMode(console,&mode)) SetConsoleMode(console,mode|ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
    while(true){
      auto s=sys::snapshot(50);
      std::cout<<"\033[2J\033[H"
               <<"\033[96m┏━━━━━━━━━━━━━━━━━━━━━━━━ IZUKI CYBERDECK 0.1.0 ━━━━━━━━━━━━━━━━━━━━━━━━┓\033[0m\n"
               <<"\033[96m┃\033[0m SYSTEM CPU "<<s.cpu<<"%  |  RAM "<<s.ram<<"%  |  DISK "<<s.disk<<"%  |  UPTIME "<<s.uptime<<"s\n"
               <<"\033[96m┃\033[0m IP "<<(s.ip.empty()?"n/a":s.ip)<<"  |  MODE TERMINAL  |  STATUS ONLINE\n"
               <<"\033[96m┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\033[0m\n"
               <<"\033[96m┃\033[0m COMMANDES: stats  sha256  b64  b64d  hex  uuid  pass  resolve\n"
               <<"\033[96m┃\033[0m            conns  plugins  plugin  help  clear  exit\n"
               <<"\033[96m┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛\033[0m\n\n";
      std::cout<<"\033[92mizuki@cyberdeck\033[0m:\033[94m~\033[0m$ ";
      std::string line; if(!std::getline(std::cin,line)) break;
      while(!line.empty()&&(line.back()==' '||line.back()=='\t')) line.pop_back();
      size_t pos=0; while(pos<line.size()&&(line[pos]==' '||line[pos]=='\t')) ++pos; line=line.substr(pos);
      if(line.empty()) continue;
      std::istringstream input(line); std::string command,rest;
      input>>command; std::getline(input,rest); while(!rest.empty()&&(rest[0]==' '||rest[0]=='\t')) rest.erase(rest.begin());
      if(command=="exit"||command=="quit") break;
      if(command=="clear") continue;
      std::vector<std::string> interactive={command}; if(!rest.empty()) interactive.push_back(rest);
      execute(interactive); std::cout<<"\nAppuyez sur Entree pour revenir...";
      std::cin.get();
    }
    return 0;
  }

  const std::string c=a[0]; std::string o;
  try{
  if(c=="sha256") std::cout<<util::sha256(arg(a,1))<<"\n";
  else if(c=="b64") std::cout<<util::b64enc(arg(a,1))<<"\n";
  else if(c=="b64d"){ if(!util::b64dec(arg(a,1),o)){ std::cerr<<"invalid base64\n"; return 2;} std::cout<<o<<"\n"; }
  else if(c=="hex") std::cout<<util::hexenc(arg(a,1))<<"\n";
  else if(c=="hexd"){ if(!util::hexdec(arg(a,1),o)){ std::cerr<<"invalid hex\n"; return 2;} std::cout<<o<<"\n"; }
  else if(c=="url") std::cout<<util::urlenc(arg(a,1))<<"\n"; else if(c=="urld") std::cout<<util::urldec(arg(a,1))<<"\n";
  else if(c=="json"){ bool v=util::json_valid(arg(a,1)); std::cout<<(v?"valid":"invalid")<<"\n"; return v?0:2; }
  else if(c=="regex") for(auto&m:util::regex_test(arg(a,1),arg(a,2))) std::cout<<m<<"\n";
  else if(c=="count"){ auto k=util::count(arg(a,1)); std::cout<<"chars="<<k.chars<<" words="<<k.words<<" lines="<<k.lines<<"\n"; }
  else if(c=="pass") std::cout<<util::password(a.size()>1?std::stoul(a[1]):20)<<"\n"; else if(c=="uuid") std::cout<<util::uuid4()<<"\n";
  else if(c=="ts") std::cout<<util::ts_to_iso(std::stoll(arg(a,1)))<<"\n";
  else if(c=="stats"){ auto s=sys::snapshot(); std::cout<<"cpu="<<s.cpu<<"% ram="<<s.ram<<"% disk="<<s.disk<<"% uptime="<<s.uptime<<"s ip="<<(s.ip.empty()?"n/a":s.ip)<<" gpu=unavailable(not implemented)\n"; }
  else if(c=="resolve"){ auto r=sys::resolve(arg(a,1)); if(r.empty()){ std::cerr<<"resolution failed\n"; return 2;} for(auto&x:r) std::cout<<x<<"\n"; h.log("network","resolve",arg(a,1)); }
  else if(c=="conns"){ auto cs=blue::tcp_connections(); for(auto&x:cs) std::cout<<x.local<<" -> :"<<x.rport<<" "<<x.state<<"\n"; for(auto&al:blue::alerts(cs)) std::cout<<"ALERT "<<al<<"\n"; }
  else if(c=="plugins") for(auto&p:core::discover("plugins")) std::cout<<p<<"\n";
  else if(c=="plugin"){ int rc=core::run("plugins",arg(a,1),arg(a,2),o); std::cout<<o; return rc==0?0:2; }
  else return usage();
  }catch(const std::exception&e){ std::cerr<<"error: "<<e.what()<<"\n"; return 2; }
  return 0; }
