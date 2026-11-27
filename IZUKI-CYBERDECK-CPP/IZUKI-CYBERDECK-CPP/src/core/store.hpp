#pragma once
#include <fstream>
#include <string>
#include <ctime>
namespace izuki::core {
// Flat append-only history file (no SQLite at build time).
class History{ std::string path; public: explicit History(std::string p):path(std::move(p)){}
  static std::string esc(std::string s){ for(auto&c:s) if(c=='\n'||c=='\t') c=' '; return s; }
  void log(const std::string& mod,const std::string& act,const std::string& detail=""){ std::ofstream f(path,std::ios::app); f<<std::time(nullptr)<<'\t'<<esc(mod)<<'\t'<<esc(act)<<'\t'<<esc(detail)<<'\n'; } };
}
