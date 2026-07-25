cmake_minimum_required(VERSION 3.10)
project(Verlet-Physics)

find_package(SFML 3 REQUIRED COMPONENTS
    Graphics
    Window
    System
)

add_executable(Verlet-Physics
    src/main.cpp
    src/render/render.cpp
    src/engine/solver.cpp
)

add_custom_command(TARGET Verlet-Physics POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:Verlet-Physics>/assets
)

target_include_directories(Verlet-Physics PRIVATE
    include
)


target_link_libraries(Verlet-Physics PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)