#include <string>
#include <vector>
#include <complex>
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

// parameters of type int
int func1(int a, int b){
	console_log(__PRETTY_FUNCTION__);
	return std::max(a,b);
}

// parameters of type double
double func2(double a, double b){
	console_log(__PRETTY_FUNCTION__);
	return a*b;
}

// parameter of type string (C++)
int func3(const std::string & s){
	console_log(__PRETTY_FUNCTION__);
	return s.size()/2;
}

// arg is assumed to be an array in javascript
int func4(emscripten::val arg){
	console_log(__PRETTY_FUNCTION__);
	std::vector<int> v=vecFromJSArray<int>(arg); // arg=a JS array
		// v is a C++ object (vector<int>) filled with the values of arg
	int mx=0;
	if(v.size()>0){
		mx=v[0];
		for(int x:v)
			mx=std::max(mx,x);
	}
	return mx; // returns the max value of arg (an array in JS)
}

// returns an array in JS
emscripten::val func5(int s){
	console_log(__PRETTY_FUNCTION__);
	int v=s;
	std::vector<int> vres(s);
	//v=[s,s-1,...,1]
	for(int & x:vres){
		x=v; v--;
	}
	// creates an object from a C++ vector
	emscripten::val array = emscripten::val::array(vres.begin(),vres.end());
	// returns an object that will be used as a JS array
	return array;
}

/**********************************************************/
/**********************************************************/
/**********************************************************/
// arg is assumed to be a JS array with strings
std::string func6(emscripten::val arg){
	// a vector<string> (C++) is filled
	std::vector<std::string> v = vecFromJSArray<std::string>(arg);
	std::string sres="";
	for(std::string s : v)
		sres += s;
	// all strings are concatenated and returned in one string
	return sres;
}

// arg = JS object (attributes x/y) [see JS/HTML]
int func7(emscripten::val arg){
	//reads the attribute x of the object arg (JS object)
	int x = arg["x"].as<int>();
	//reads the attribute y of the object
	int y = arg["y"].as<int>();
	return x + y;
}

// creates a JS object of type point (see JS/HTML)
emscripten::val func8(int x,int y){
	emscripten::val point = emscripten::val::global("point").new_(x,y);
	return point;
}

/**********************************************************/
/**********************************************************/
/**********************************************************/
// arg = is assumed to be a JS object of class point (attributes x/y)
int func9(emscripten::val arg){
	int x=arg["x"].as<int>();
	int y=arg["y"].as<int>();
	return x+y;
}

emscripten::val func10(int x,int y){
	// creates a JS object
	emscripten::val point=emscripten::val::object();
	point.set("x",x);
	point.set("y",y); // creates onthefly two attributes
	return point;
}

/**********************************************************/
/**********************************************************/
/**********************************************************/
/* In the JS part of the Web page, a complex type has to be defined
 with two attributes. For example, this one: (see the html file)
	function complex(r,i)
	{
		this.r=r;
		this.i=i;
		this.plus=function(c){return Module.cplus(this,c);}
		this.times=function(c){ return Module.ctimes(this,c);}
		this.toString=function(){return Module.cstring(this);}
	}
*/


// **** Two functions to wrap the mirrored types : complex JS <-> complex C++ **** //
// JS to C++(WASM)
// JS object complex (emscripten::val) -> std::complex (C++ object)
std::complex<double> valToComplex(emscripten::val vC) // vC represents a complex in JS
{
	double r=vC["r"].as<double>(); // attribute r from vC -> double ;
	double i=vC["i"].as<double>(); // attribute i from vC -> double;
	std::complex<double> objectCPP(r,i); // initialize a C++ object
	return objectCPP;
}
// C++(WASM) to JS
// std::complex -> emscripten::val (JS object complex)
emscripten::val complexToVal(const std::complex<double> & c)
{
	emscripten::val C=emscripten::val::global("complex").new_(c.real(),c.imag()); // instantiates a JS variable
	return C;
}
// **** EXPOSED functions to compute plus and times operations **** //
// vC1,vC2 assumed to be complex JS objects
emscripten::val cplus(emscripten::val vC1,emscripten::val vC2) {
	std::complex<double> c1=valToComplex(vC1); // c1 is a std::complex C++ object
	std::complex<double>c2=valToComplex(vC2); // c2 is a std::complex C++ object
	std::complex<double> cRes=c1+c2; // c1+c2 is a std::complex -> cRes is std::complex
	return complexToVal(cRes); // from std::complex to a variable in JS
}
// vC1,vC2 assumed to be complex JS objects
emscripten::val ctimes(emscripten::val vC1,emscripten::val vC2) {
	std::complex<double> c1=valToComplex(vC1); // c1 is a std::complex C++ object
	std::complex<double> c2=valToComplex(vC2); // c2 is a std::complex C++ object
	std::complex<double> cRes=c1*c2; // c1*c2 is a std::complex -> cRes is std::complex
	return complexToVal(cRes);
}

// vC1 assumed to be complex JS objects returns a string
std::string cplxToString(emscripten::val vC1) // to give a string output of a complex (JS)
{
	std::complex<double> c1=valToComplex(vC1); // JS to C++
	std::stringstream ss;
	ss << c1.real() << "+(" << c1.imag() << ")i";
	return ss.str(); // returns a string
}

EMSCRIPTEN_BINDINGS(module) {
	//called from JS as Module.modify(...)
	emscripten::function("modifyStr", &modifyStr);
	// Module.f1(...) (JS) will call func1(...) (C++)
	emscripten::function("f1", &func1);
	emscripten::function("f2", &func2);
	emscripten::function("f3", &func3);
	emscripten::function("f4", &func4);
	emscripten::function("f5", &func5);

	/*********************************/
	/*********************************/
	emscripten::function("f6", &func6);
	emscripten::function("f7", &func7);
	emscripten::function("f8", &func8); 
 
	/*********************************/
	/*********************************/
	emscripten::function("f9", &func9);
	emscripten::function("f10", &func10); 

	/*********************************/
	/*********************************/
	emscripten::function("cplus", &cplus); // can be called from JS Module.cplus(__,__)
	emscripten::function("ctimes", &ctimes);
	emscripten::function("cstring", &cplxToString);
}
}	//extern "C" 
