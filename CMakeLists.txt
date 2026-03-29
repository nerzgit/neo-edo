cmake_minimum_required(VERSION 3.15)

set(APP_NAME neo-edo)
project(${APP_NAME} LANGUAGES CXX)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenAL CONFIG REQUIRED)
find_package(Stb REQUIRED)

# src内のすべての.cppファイルを取得
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(${APP_NAME} ${SOURCES})

target_compile_definitions(${APP_NAME}
    PRIVATE
        ASSETS_DIR="${CMAKE_SOURCE_DIR}/assets"
        ENGINE_ASSETS_DIR="${CMAKE_SOURCE_DIR}/src/engine/resource/assets"
)

target_include_directories(${APP_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include
        ${Stb_INCLUDE_DIR}
)

target_link_libraries(${APP_NAME}
  PRIVATE
    glfw
    glad::glad
    glm::glm
    nlohmann_json::nlohmann_json
    OpenAL::OpenAL
)

set_target_properties(${APP_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
