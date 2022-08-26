# Bmx
## About
**Block Mixture Language** is a lightweight data storage format. Its reserved word is only one pair of brackets. This provide you a simple way to write and read **Bmx** file.

This library is a **Bmx** parser for C++, but you can do some work to bind with other programming languages.

## How to use
**Bmx** file can be formed with three structures: *Commit*, *BlockHeads*, *BlockBodys*.
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
    data = Bmx::read("sample.bmx");

    std::cout << data["Block1"] << std::endl;
    return 0;
}
```
You can see more details in the [Document](https://github.com/AnpyDX/Bmx/blob/main/Document.md)

## License
Licensed under the MIT license, see `LICENSE` for details.