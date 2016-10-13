/*----------COPYRIGHT-------------------------
(c) Copyright Will Eccles, 2016
----------------------------------------------*/

/*----------LICENSE---------------------------
This software is licensed under the Apache license. See LICENSE file to learn more.
----------------------------------------------*/

/*----------DESCRIPTION-----------------------
This is a collection of my frequently used classes and methods.
----------------------------------------------*/

/* Common imports, or things required for the classes to work. */
#include <stdio.h>
#include <string>
#include <thread>
#include <chrono>

// used to detect things later on, IIRC. Just copied it from the old IOManager.h file, pretty sure it's used somewhere.
#ifdef _WIN32
#define ISWINDOWS 1
#else
#define ISWINDOWS 0
#endif

using namespace std::chrono;

// My namespace, which contains helpful methods and such.
namespace wrestd {
	/* Methods that work with strings. */
	namespace strings {
		int substr_count(std::string, std::string, bool);
		std::string substr_replace(std::string&, std::string, std::string, bool, bool);
		std::string to_lower(std::string);
		std::string to_upper(std::string);
	}
	
	/* Handles IO functions, such as console printing, colors, waiting, etc. and file existance checks. */
	namespace io {
		enum color_t {
			BLACK = 0,
			DARKBLUE = 1,
			DARKGREEN = 2,
			DARKCYAN = 3,
			DARKRED = 4,
			DARKMAGENTA = 5,
			DARKYELLOW = 6,
			DARKWHITE = 7,
			DARKGREY = 8,
			BLUE = 9,
			GREEN = 10,
			CYAN = 11,
			RED = 12,
			MAGENTA = 13,
			YELLOW = 14,
			WHITE = 15,
			DEFAULT = WHITE
		};
		void printc(std::string, color_t, color_t);
		void printlc(std::string, color_t, color_t);
		void clear();
		void wait(color_t, color_t);
		void wait(std::string, color_t, color_t);
		void setColor(color_t, color_t);
		bool fileExists(char[]);
	};
	
	/* Handles threading-related functions, including execution times and such. */
	namespace threading {
		template<class RType = double,
			class _Fn,
			class... _Args>
		RType executionTime(_Fn&& fn, _Args&&... args) {
			high_resolution_clock::time_point t1 = high_resolution_clock::now();
			fn(args...);
			high_resolution_clock::time_point t2 = high_resolution_clock::now();
			duration<RType> timespan = duration_cast<duration<RType> >(t2-t1);
			return (RType)timespan.count() * (RType)seconds::period::num/(RType)seconds::period::den;
		}
	};
}
