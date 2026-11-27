#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <cstring>
#include <random>
#include <regex>
#include <sstream>
#include <ctime>
#include <iomanip>
namespace izuki::util {
inline std::string sha256(const std::string& m) {
  static const uint32_t K[64]={0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2};
  uint32_t h[8]={0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19};
  std::string p=m; p+=char(0x80); while(p.size()%64!=56) p+=char(0);
  uint64_t bits=uint64_t(m.size())*8; for(int i=7;i>=0;--i) p+=char(bits>>(i*8));
  auto rr=[](uint32_t x,int n){return (x>>n)|(x<<(32-n));};
  for(size_t o=0;o<p.size();o+=64){ uint32_t w[64];
    for(int i=0;i<16;i++) w[i]=(uint32_t(uint8_t(p[o+i*4]))<<24)|(uint32_t(uint8_t(p[o+i*4+1]))<<16)|(uint32_t(uint8_t(p[o+i*4+2]))<<8)|uint8_t(p[o+i*4+3]);
    for(int i=16;i<64;i++){uint32_t s0=rr(w[i-15],7)^rr(w[i-15],18)^(w[i-15]>>3),s1=rr(w[i-2],17)^rr(w[i-2],19)^(w[i-2]>>10);w[i]=w[i-16]+s0+w[i-7]+s1;}
    uint32_t a=h[0],b=h[1],c=h[2],d=h[3],e=h[4],f=h[5],g=h[6],hh=h[7];
    for(int i=0;i<64;i++){uint32_t t1=hh+(rr(e,6)^rr(e,11)^rr(e,25))+((e&f)^(~e&g))+K[i]+w[i],t2=(rr(a,2)^rr(a,13)^rr(a,22))+((a&b)^(a&c)^(b&c));hh=g;g=f;f=e;e=d+t1;d=c;c=b;b=a;a=t1+t2;}
    h[0]+=a;h[1]+=b;h[2]+=c;h[3]+=d;h[4]+=e;h[5]+=f;h[6]+=g;h[7]+=hh; }
  std::ostringstream r; for(auto x:h) r<<std::hex<<std::setw(8)<<std::setfill('0')<<x; return r.str(); }
inline std::string b64enc(const std::string& s){ static const char*T="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; std::string o; size_t i=0;
  for(;i+2<s.size();i+=3){uint32_t v=(uint32_t(uint8_t(s[i]))<<16)|(uint32_t(uint8_t(s[i+1]))<<8)|uint8_t(s[i+2]);o+=T[v>>18];o+=T[(v>>12)&63];o+=T[(v>>6)&63];o+=T[v&63];}
  if(i+1==s.size()){uint32_t v=uint32_t(uint8_t(s[i]))<<16;o+=T[v>>18];o+=T[(v>>12)&63];o+="==";}
  else if(i+2==s.size()){uint32_t v=(uint32_t(uint8_t(s[i]))<<16)|(uint32_t(uint8_t(s[i+1]))<<8);o+=T[v>>18];o+=T[(v>>12)&63];o+=T[(v>>6)&63];o+='=';} return o; }
inline bool b64dec(const std::string& s, std::string& out){ static const std::string T="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; out.clear(); uint32_t v=0; int n=0;
  if(s.size()%4) return false;
  for(char c:s){ if(c=='=') break; auto p=T.find(c); if(p==std::string::npos) return false; v=(v<<6)|uint32_t(p); n+=6; if(n>=8){n-=8;out+=char((v>>n)&0xFF);} } return true; }
inline std::string hexenc(const std::string& s){ std::ostringstream r; for(unsigned char c:s) r<<std::hex<<std::setw(2)<<std::setfill('0')<<int(c); return r.str(); }
inline bool hexdec(const std::string& s, std::string& out){ out.clear(); if(s.size()%2) return false; for(size_t i=0;i<s.size();i+=2){ if(!isxdigit((unsigned char)s[i])||!isxdigit((unsigned char)s[i+1])) return false; out+=char(std::stoi(s.substr(i,2),nullptr,16)); } return true; }
inline std::string urlenc(const std::string& s){ std::ostringstream r; for(unsigned char c:s){ if(isalnum(c)||c=='-'||c=='_'||c=='.'||c=='~') r<<c; else r<<'%'<<std::uppercase<<std::hex<<std::setw(2)<<std::setfill('0')<<int(c);} return r.str(); }
inline std::string urldec(const std::string& s){ std::string o; for(size_t i=0;i<s.size();i++){ if(s[i]=='%'&&i+2<s.size()&&isxdigit((unsigned char)s[i+1])&&isxdigit((unsigned char)s[i+2])){o+=char(std::stoi(s.substr(i+1,2),nullptr,16));i+=2;} else o+=s[i]; } return o; }
inline std::string password(size_t n=20){ if(n<8) n=8; static const std::string A="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*-_=+"; std::random_device rd; std::uniform_int_distribution<size_t> d(0,A.size()-1); std::string o; while(o.size()<n) o+=A[d(rd)]; return o; }
inline std::string uuid4(){ std::random_device rd; uint8_t b[16]; for(auto&x:b) x=uint8_t(rd()); b[6]=(b[6]&0x0F)|0x40; b[8]=(b[8]&0x3F)|0x80; std::ostringstream r; for(int i=0;i<16;i++){ if(i==4||i==6||i==8||i==10) r<<'-'; r<<std::hex<<std::setw(2)<<std::setfill('0')<<int(b[i]); } return r.str(); }
inline std::string ts_to_iso(long long t){ time_t tt=t; char b[32]; std::strftime(b,sizeof b,"%Y-%m-%dT%H:%M:%SZ",std::gmtime(&tt)); return b; }
inline std::vector<std::string> regex_test(const std::string& pat,const std::string& text){ std::vector<std::string> r; std::regex re(pat); for(std::sregex_iterator i(text.begin(),text.end(),re),e;i!=e;++i) r.push_back(i->str()); return r; }
struct Count{size_t chars,words,lines;};
inline Count count(const std::string& t){ std::istringstream w(t); std::string x; size_t words=0; while(w>>x) words++; size_t lines=t.empty()?0:1; for(char c:t) if(c=='\n') lines++; if(!t.empty()&&t.back()=='\n') lines--; return {t.size(),words,lines}; }
struct J{ const std::string&s; size_t i=0;
  void ws(){ while(i<s.size()&&isspace((unsigned char)s[i])) i++; }
  bool lit(const char*l){ size_t n=strlen(l); if(s.compare(i,n,l)==0){i+=n;return true;} return false; }
  bool str(){ if(i>=s.size()||s[i]!='"') return false; i++; while(i<s.size()&&s[i]!='"'){ if(s[i]=='\\') i++; else if((unsigned char)s[i]<0x20) return false; i++; } if(i>=s.size()) return false; i++; return true; }
  bool dig(){ if(i>=s.size()||!isdigit((unsigned char)s[i])) return false; while(i<s.size()&&isdigit((unsigned char)s[i])) i++; return true; }
  bool num(){ if(i<s.size()&&s[i]=='-') i++; if(!dig()) return false; if(i<s.size()&&s[i]=='.'){i++; if(!dig()) return false;} if(i<s.size()&&(s[i]=='e'||s[i]=='E')){i++; if(i<s.size()&&(s[i]=='+'||s[i]=='-')) i++; if(!dig()) return false;} return true; }
  bool val(int d=0){ if(d>200) return false; ws(); if(i>=s.size()) return false; char c=s[i];
    if(c=='{'){ i++; ws(); if(i<s.size()&&s[i]=='}'){i++;return true;} for(;;){ ws(); if(!str()) return false; ws(); if(i>=s.size()||s[i++]!=':') return false; if(!val(d+1)) return false; ws(); if(i>=s.size()) return false; if(s[i]==','){i++;continue;} if(s[i]=='}'){i++;return true;} return false; } }
    if(c=='['){ i++; ws(); if(i<s.size()&&s[i]==']'){i++;return true;} for(;;){ if(!val(d+1)) return false; ws(); if(i>=s.size()) return false; if(s[i]==','){i++;continue;} if(s[i]==']'){i++;return true;} return false; } }
    if(c=='"') return str(); if(lit("true")||lit("false")||lit("null")) return true; return num(); } };
inline bool json_valid(const std::string& s){ J j{s}; if(!j.val()) return false; j.ws(); return j.i==s.size(); }
}
