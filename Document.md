# Document
## Bmx Syntax
A `bmx` file can be formed with three parts: *Comments*, *BlockHeads* and *BlockBodies*.

It is important to note that the only recognizable symbol is the `[`, which is the beginning of the new block and the end of the previous block.

Here are some examples.

**NOTE: The first line in `file` cannot put any block head, or you may get unexcepted errors!**

- Comment

 You can write comments in two places: from the beginning of the file to the beginning of the first block, and on the same line after each block declaration. In these places, you can write any thing you want, except `[`.

```ini
"""
This is a comment
My first bmx file.
"""
[ Block1 ] # This is a comment | my first block.
```

- BlockHead & BlockBody

Use `[` to start block name, and `]` to end it. Brackets and the block name it contains are *BlockHead*.

```ini
[ BlockName ] # Here is BlockHead
BlockBody
```

- Full Example
```ini
# This is a BMX file!
This is a Comment!
"""
Comment 1
Comment 2
"""

[ Name ] # Author's name
AnpyD

[ Age ] # Author's age
0

[ Height ] # Author's height
-1
```

## Usage Examples
**Sample.bmx**
```ini
"""
Sample Bmx File
- date: 22/8
"""

[ CompanyName ]
GreatCompany

[ Location ]
SA
```

- Load from file
```c++
#include <iostream>
#include <fstream>
#include "bmx.hpp"
using namespace std;

int main()
{
    fstream f("Sample.bmx");
    Bmx::Type data = Bmx::loads(f);
    std::cout << data["Company"] << std::endl
              << data["Location"] << std::endl;

    f.close();
    return 0;
}
```

- Load from string
```c++
#include <iostream>
#include "bmx.hpp"
using namespace std;

int main()
{
    
    Bmx::Type data = Bmx::loads("[Company]\nGreateCompany\n[Location]\nLA");
    std::cout << data["Company"] << std::endl
              << data["Location"] << std::endl;

    return 0;
}
```

- Write data to files
```c++
#include <iostream>
#include <fstream>
#include "bmx.hpp"
using namespace std;

int main()
{
    
    Bmx::Type data = Bmx::loads("[Company]\nGreateCompany\n[Location]\nLA");
    std::string str = Bmx::dumps(data);

    ofstream f("Test.bmx");
    f << str;
    f.close();
    
    return 0;
}
```


## C++ API
All functions / types from Bmx library are wrapped into `Bmx` namespace.

### Bmx::Exception
- Prototype: `class Exception: public std::exception`
- Usage: Throw when there is a problem reading `BMX` data.

### Bmx::Type
- Prototype: `class Type`
- Usage: Wrapper of `BMX` data.

### Bmx::Type::update
- Prototype: `void Type::update(const std::string& key, const std::string& value)`
- Usage: Add block / Change block's value.

### Bmx::Type::remove
- Prototype: `void Type::remove(const std::string& key)`
- Usage: Remove block.

### Bmx::Type::has_block
- Prototype: `bool Type::has_block(const std::string& key) const`
- Usgae: Check is block exisited.

### Bmx::Type::get_keys
- Prototype: `std::vector<std::string> Type::get_keys() const`
- Usgae: Get all blocks' keys.

### Bmx::load
- Prototype: `Type load(const std::string& str)`
- Usage: Load `Bmx` data from string.

### Bmx::loads
- Prototype: `Type loads(const std::fstream& file)`
- Usage: Load `Bmx` data from file.

### Bmx::dumps
- Prototype: `std::string dumps(Type& data)`
- Usage: Generate string from `Bmx` data for writing to files.