
### Install Dependencies
```
sudo apt-get install zlib1g-dev libmsgpack-dev
```bash

### Building
```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```