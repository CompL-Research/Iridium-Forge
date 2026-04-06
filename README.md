### Build Instructions

This section covers the necessary steps to build the project, including installing dependencies, initial setup, and building for both debug and release configurations.

#### Install Dependencies

```bash
apt install zlib2g-dev libmsgpack-dev build-essential cmake pkg-config
```

#### Install dependencies
```bash
npm i
```

#### Building for debug
```bash
CMAKE_BUILD_PARALLEL_LEVEL=$(nproc) npx cmake-js build --debug
```

#### Building for release
```bash
CMAKE_BUILD_PARALLEL_LEVEL=$(nproc) npx cmake-js build
```

#### Clean build
```bash
npx cmake-js clean
```

- If you encounter build errors, ensure all dependencies are installed correctly.
- Check that your Node.js version is compatible with the project requirements.
- For troubleshooting, consider checking the project's issue tracker or documentation for known compatibility issues.
