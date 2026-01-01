# Building for release
release:
	CMAKE_BUILD_PARALLEL_LEVEL=$(nproc) npx cmake-js build

# Building for debug
debug:
	CMAKE_BUILD_PARALLEL_LEVEL=$(nproc) npx cmake-js build --debug
