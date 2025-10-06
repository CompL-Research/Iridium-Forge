
### Install Dependencies

```
sudo apt-get install zlib1g-dev libmsgpack-dev
```

### Building for debug
```
CMAKE_BUILD_PARALLEL_LEVEL=NUMCPUS npx cmake-js build --debug
```

### Building fore release
```
CMAKE_BUILD_PARALLEL_LEVEL=NUMCPUS npx cmake-js build
```