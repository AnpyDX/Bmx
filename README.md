# Bmx
## About
**Block Mixture Language** is a lightweight data storage format. It provide you a easy way to store data or codes! For example, you can put the *vertex shader* and *fragment shader* at one `bmx` file, so that you can easily manage your code.

This library is a `Bmx` parser for C++, but you can do some work to bind with other programming languages.

## How to use
You can read and parse `Bmx` by including only a singel header file.

Here is a simple example.

- sample.bmx
```ini
"""
This is a commit   
"""

[ Block1 ]
Here is BlockBody

[ Block2 ]
Hello World
123456

[ MyCode ]
printf("Hello World!");

```
- main.cpp
```c++
#include <iostream>
#include "bmx.hpp"
using namespace std;

int main()
{
    Bmx::Type data;
    data = Bmx::loads("sample.bmx");

    std::cout << data["Block1"] << std::endl;
    return 0;
}
```
You can see more details in the [Document](https://github.com/AnpyDX/Bmx/blob/main/Document.md)


## Document
See [Document](https://github.com/AnpyDX/Bmx/blob/main/Document.md).

## License
Licensed under the MIT license, see `LICENSE` for details.
