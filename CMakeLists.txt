cmake_minimum_required(VERSION 3.5)
project(CMakeSFMLProject)
set(RESOURCE_DIR "${CMAKE_SOURCE_DIR}/resources")
file(COPY ${RESOURCE_DIR}/images DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${RESOURCE_DIR}/music DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${RESOURCE_DIR}/fonts DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY ${RESOURCE_DIR}/records DESTINATION ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)  # out/build/x64-Debug/bin
option(BUILD_SHARED_LIBS "Build shared libraries" OFF) 

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG "3.0.0"  
    GIT_SHALLOW ON  
) 
FetchContent_MakeAvailable(SFML) 
add_executable(CMakeSFMLProject src/main.cpp)
target_include_directories(CMakeSFMLProject PUBLIC include)

# Добавляем SFML_STATIC
target_compile_definitions(CMakeSFMLProject PUBLIC SFML_STATIC)

# Линкуем дополнительные библиотеки
target_link_libraries(CMakeSFMLProject PUBLIC 
    SFML::Graphics SFML::Audio SFML::Network
)