# wrestd

My standard C++ library. This basically is just a collection of my most-used classes/methods. Some classes may have better documentation in comments than the others do. This is simply because I don't plan on other people using this, and I already know what the things all do. In any case, most or all methods have short comments above their definitions that will tell you what they do.

## Usage
In order to use this, you just have to include [`wrestd.h`](Code/wrestd.h).

After that, you can use things like this:

```c++
#include "wrestd.h"

using namespace wrestd;
using namespace wrestd::io;
using wrestd::io::clear();
```

Of course, you can always just use the fully qualified names, which does still require the inclusion of `wrestd.h`, obviously.

```c++
#include "wrestd.h"

int main() {
	wrestd::io::clear();
	std::cout << "'ha' is in 'hahaha' " << wrestd::strings::substr_count("hahaha", "ha") << " times." << std::endl;
	
	return 0;
}
```

## Namespaces:
| Namespace | Definitions | Function |
| --- | --- | --- |
| `wrestd::io` | `io.cpp` | Contains methods to handle various IO-related things, both console and file IO. Adapts to multiple platforms. Supports colored console output on Windows. |
| `wrestd::strings` | `strings.cpp` | Contains methods that work with strings. |
| `wrestd::threading` | N/A for now | Contains thread-related methods. |

## Cross-platform
This should be cross-platform, at least as much as possible. For example, in `wrestd::io::setcolor()`, it will only do that if on Windows, and if you aren't, it will simply print it in normal colors.

**Note:** I have not tested cross-platform compatibility much. If something does not work, feel free to make a pull request with a fix and (obviously) create an issue - I'll get there when I can.
