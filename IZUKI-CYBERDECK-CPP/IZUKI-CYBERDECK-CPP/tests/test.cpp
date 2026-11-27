#undef NDEBUG
#include <cassert>
#include <iostream>
#include "../src/modules/utilities.hpp"
#include "../src/modules/blue_team.hpp"
#include "../src/system/monitor.hpp"
#include "../src/core/plugins.hpp"
using namespace izuki;
int main(){ std::string o;
  assert(util::sha256("abc")=="ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
  assert(util::sha256("")=="e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
  assert(util::b64enc("foobar")=="Zm9vYmFy"&&util::b64enc("fo")=="Zm8="); assert(util::b64dec("Zm8=",o)&&o=="fo"); assert(!util::b64dec("@@@@",o));
  assert(util::hexenc("hi")=="6869"&&util::hexdec("6869",o)&&o=="hi"&&!util::hexdec("zz",o));
  assert(util::urlenc("a b&")=="a%20b%26"&&util::urldec("a%20b%26")=="a b&");
  assert(util::json_valid("{\"a\":[1,2.5,true,null]}")&&!util::json_valid("{\"a\":}")&&!util::json_valid("{"));
  assert(util::password(24).size()==24&&util::uuid4().size()==36&&util::ts_to_iso(0)=="1970-01-01T00:00:00Z");
  assert(util::regex_test("[0-9]+","a12b3").size()==2&&util::count("a b\nc").words==3);
  auto s=sys::snapshot(50); assert(s.cpu>=0&&s.cpu<=100&&s.ram>0&&s.uptime>0);
  assert(!blue::alerts({{"x",5,4444,"ESTABLISHED"}}).empty()&&blue::alerts({{"x",5,443,"ESTABLISHED"}}).empty());
  assert(core::discover("plugins").size()==1&&core::run("plugins","example_echo","hi",o)==0&&o=="echo: hi\n"&&core::run("plugins","../x","a",o)==-1);
  std::cout<<"ALL TESTS PASSED\n"; }
