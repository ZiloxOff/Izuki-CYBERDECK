#pragma once
#include <string>
#include <vector>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <filesystem>
namespace izuki::core {
// Plugin = dir with manifest.json + executable "run", started as a separate process (not a sandbox).
inline bool manifest_ok(const std::string& m, std::string& why){ for(auto k:{"id","name","version","description","category","dependencies","permissions","platforms","entry"}) if(m.find(std::string("\"")+k+"\"")==std::string::npos){ why=std::string("missing ")+k; return false; } return true; }
inline std::vector<std::string> discover(const std::string& dir){ std::vector<std::string> ok; namespace fs=std::filesystem; if(!fs::exists(dir)) return ok;
  for(auto&e:fs::directory_iterator(dir)){ auto mf=e.path()/"manifest.json"; if(!fs::exists(mf)) continue; std::ifstream f(mf); std::stringstream s; s<<f.rdbuf(); std::string why; if(manifest_ok(s.str(),why)) ok.push_back(e.path().filename().string()); } return ok; }
inline int run(const std::string& dir,const std::string& id,const std::string& arg,std::string& out){ for(char c:id) if(!(isalnum((unsigned char)c)||c=='_')) return -1; for(char c:arg) if(c=='\''||c=='\\') return -1;
#ifdef _WIN32
  std::string cmd="\""+dir+"/"+id+"/run.exe\" \""+arg+"\" 2>&1";
  FILE*p=_popen(cmd.c_str(),"r"); if(!p) return -1; char b[256]; out.clear(); while(fgets(b,sizeof b,p)) out+=b; return _pclose(p); }
#else
  std::string cmd="timeout 10 '"+dir+"/"+id+"/run' '"+arg+"' 2>&1"; FILE*p=popen(cmd.c_str(),"r"); if(!p) return -1; char b[256]; out.clear(); while(fgets(b,sizeof b,p)) out+=b; return pclose(p); }
#endif
}
