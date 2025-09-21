#!/usr/bin/env bash
set -eo pipefail

CMAKE_FLAGS=" \
   -DCPACK_BINARY_DEB=ON \
   -DBUILD_EXAMPLES=OFF \
   -DBUILD_opencv_python2=OFF \
   -DBUILD_opencv_python3=ON \
   -DBUILD_opencv_java=OFF \
   -DCMAKE_BUILD_TYPE=RELEASE \
   -DCMAKE_INSTALL_PREFIX=/usr/local \
   -DOPENCV_EXTRA_MODULES_PATH=/OpenCV/opencv_contrib/modules \
   -DCUDA_FAST_MATH=ON \
   -DEIGEN_INCLUDE_PATH=/usr/include/eigen3 \
   -DWITH_EIGEN=ON \
   -DOPENCV_ENABLE_NONFREE=OFF \
   -DOPENCV_GENERATE_PKGCONFIG=ON \
   -DBUILD_PERF_TESTS=OFF \
   -DBUILD_TESTS=OFF"

OPENCV_VERSION=$1

mkdir /OpenCV && cd /OpenCV &&

git clone --depth 1 --branch ${OPENCV_VERSION} https://github.com/opencv/opencv.git
git clone --depth 1 --branch ${OPENCV_VERSION} https://github.com/opencv/opencv_contrib.git

cmake -S opencv -B build ${CMAKE_FLAGS} && \
cmake --build build --config Release -- -j$(nproc) && \
cmake --install build --config Release --prefix ./install