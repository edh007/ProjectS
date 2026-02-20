#!/usr/bin/env bash
# WSL에서 GDExtension 빌드 스크립트
# 사용법: wsl bash godot_extension/build.sh [debug|release]

set -e

CONFIG=${1:-debug}
BUILD_DIR=/tmp/projectS_build
PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"

echo "=== GDExtension Build (${CONFIG}) ==="

# 첫 빌드 또는 CMakeLists 변경 시에만 configure
if [ ! -f "${BUILD_DIR}/Makefile" ]; then
    echo "Configuring CMake..."
    cmake \
        -S "${PROJECT_ROOT}/godot_extension" \
        -B "${BUILD_DIR}" \
        -DCMAKE_BUILD_TYPE=$([ "$CONFIG" = "release" ] && echo "Release" || echo "Debug")
fi

echo "Building..."
cmake --build "${BUILD_DIR}" -j$(nproc)

echo "Done. Output: ${PROJECT_ROOT}/bin/"
ls -lh "${PROJECT_ROOT}/bin/"
