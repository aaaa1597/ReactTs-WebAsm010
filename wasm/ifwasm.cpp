#include <string>
#include <emscripten.h>
#include <emscripten/bind.h>


#define LOG_OUTPUT 1
#if LOG_OUTPUT
EM_JS(int, console_log, (const char *logstr), {
  console.log('aaaaa ' + UTF8ToString(logstr));
  return 0;
});
#else
#define console_log(logstr)
#endif

extern "C" {
std::string modifyStr(const std::string & s){ // returns a copy of s whose the first/last characters are
	console_log(__PRETTY_FUNCTION__);
	std::string str(s); // replaced by 'X'
	if(str.size()>=3){
		str[0]='X';
		str[str.size()-1]='X';
	}
	return str;
}

EMSCRIPTEN_BINDINGS(module) {
	emscripten::function("modifyStr", &modifyStr); //called from JS as Module.modify(...)
}
}	//extern "C" 
