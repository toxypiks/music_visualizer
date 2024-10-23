Change library search path for cmake

export LD_LIBRARY_PATH="./"

# build
- run:
  - `mkdir build`
  - `cd build`
  - for hot code reloading:
  - `cmake -DHOTRELOAD=true ..`
  - without hot code relaoding: `cmake -DHOTRELOAD=true ..`
  - `make`
