# cmake

Collection of CMake find modules and utilities

## CMake basics
CMake is most useful for building out-of-source.  
In many cases, if not all, you should create a build directory in your project root, which will contain all staging files.

```bash
# clone a project
git clone <my_project_url> my_project
# create a build directory ...
mkdit my_project/build
# ... and navigate to it
cd my_project/build
# instruct cmake to create Makefiles (on linux, that is), by letting it look for a CMakelists.txt in 1 directory up (..)
cmake .. 
# build the project
make
```


