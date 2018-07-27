# CppGuid
Easy to use cross platform C++ GUID library. Supports CMake and Mama build.

# Getting started
This repository supports both CMake and Mama build tool: https://github.com/RedFox20/Mama

1. Install mama `pip install mama`
2. Add this repository to your project mamafile.py
```py
import mama
class MyProject(mama.BuildTarget):
    def dependencies(self):
        self.add_git('CppGuid', 'https://github.com/RedFox20/CppGuid.git')
```
3. Build your project `mama build`
4. Configure your cmake to include mama products
```cmake
include(mama.cmake)
include_directories(${MAMA_INCLUDES})

target_link_libraries(MyProject ${MAMA_LIBS})
```
5. Include and use the guid
```cpp
#include <cppguid/guid.h>
using cppguid::Guid;

void test()
{
    Guid guid = Guid::create();
    std::cout << guid << "\n";
}
```
