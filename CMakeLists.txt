cmake_minimum_required(VERSION 3.28)
project(flashcard_app)

set(CMAKE_CXX_STANDARD 20)

add_executable(flashcard_app main.cpp)

# Include directories for Native File Dialog
include_directories("C:/Program Files/JetBrains/CLion 2024.3/bin/mingw/include/nativefiledialog-master")

# Link against the Native File Dialog library
set(NFD_LIB_PATH "C:/Program Files/JetBrains/CLion 2024.3/bin/mingw/include/nativefiledialog-master/nativefiledialog-master/cmake-build-debug/libnativefiledialog.a")

target_link_libraries(flashcard_app PRIVATE ${NFD_LIB_PATH})
