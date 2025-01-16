cmake_minimum_required(VERSION 3.10)

project(yolo11_detect)

# EXPLANATION
if(POLICY CMP0146)
  cmake_policy(SET CMP0146 OLD)
endif()

add_definitions(-std=c++11)
add_definitions(-DAPI_EXPORTS)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE release)

# include and link dirs of cuda and tensorrt, you need adapt them if yours are different
# ============= cuda ============
set(CMAKE_CUDA_ARCHITECTURES 86)
set(CMAKE_CUDA_COMPILER "/usr/local/cuda-12.6/bin/nvcc")
find_package(CUDA REQUIRED)
include_directories(/usr/local/cuda-12.6/include)
link_directories(/usr/local/cuda-12.6/lib64)
include_directories(/home/valmorx/DeepLearningSource/cudnn-8.9.7/include)
link_directories(/home/valmorx/DeepLearningSource/cudnn-8.9.7/lib)

# ============= tensorrt ============
if (CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
  message("Embed_platform on")
  include_directories(/home/valmorx/DeepLearningSource/TensorRT-8.6.1.6/include)
  include_directories(/usr/include/aarch64-linux-gnu)
  include_directories(/home/valmorx/DeepLearningSource/TensorRT-8.6.1.6/targets/x86_64-linux-gnu/include)
  link_directories(/usr/lib/aarch64-linux-gnu)
  link_directories(/home/valmorx/DeepLearningSource/TensorRT-8.6.1.6/lib)
  link_directories(/home/valmorx/DeepLearningSource/TensorRT-8.6.1.6/targets/x86_64-linux-gnu/lib)
else()
  message("Embed_platform off")
  include_directories(/home/valmorx/DeepLearningSource/TensorRT-8.6.1.6/include)
  include_directories(/usr/include/x86_64-linux-gnu)
  include_directories(/home/valmorx/DeepLearningSource/TensorRT-8.6.1.6/targets/x86_64-linux-gnu/include)
  link_directories(/usr/lib/x86_64-linux-gnu)
  link_directories(/home/valmorx/DeepLearningSource/TensorRT-8.6.1.6/lib)
  link_directories(/home/valmorx/DeepLearningSource/TensorRT-8.6.1.6/targets/x86_64-linux-gnu/lib)

endif()

# ============ opencv ============
find_package(OpenCV)
include_directories(${OpenCV_INCLUDE_DIRS})

# ============ yolo infer shared lib ============
include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB_RECURSE SRCS ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.cu)
cuda_add_library(yolo_infer SHARED ${SRCS})
target_link_libraries(yolo_infer cudart nvinfer nvonnxparser ${OpenCV_LIBS})

# ======== main execute file ========
add_executable(main ${PROJECT_SOURCE_DIR}/main.cpp)
target_link_libraries(main yolo_infer)
