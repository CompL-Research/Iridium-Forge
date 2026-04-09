
### Install Dependencies

```
apt install zlib1g-dev libmsgpack-dev build-essential cmake pkg-config

npm install
```

### Building for debug
```
CMAKE_BUILD_PARALLEL_LEVEL=$(nproc) npx cmake-js build --debug
```

### Building fore release
```
CMAKE_BUILD_PARALLEL_LEVEL=$(nproc) npx cmake-js build
```