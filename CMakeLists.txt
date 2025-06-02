# Minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# Project Name and Language
project(OpenGLSolarSystem CXX C) # CXX means C++

# Set C++ standard (e.g., C++17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# --- Library Paths ---
set(GLFW_DIR "C:/Users/bishn/Downloads/glfw-3.4.bin.WIN64/glfw-3.4.bin.WIN64")
set(GLAD_INCLUDE_DIR "C:/Users/bishn/Downloads/glad/include") # Path to GLAD's 'include' folder
set(GLM_INCLUDE_DIR "C:/Users/bishn/Downloads/glm-0.9.9.8/glm") # Path to the folder CONTAINING the 'glm' subfolder

# --- Finding GLFW ---
# For GLFW, we'll point to the include directory and the library file
include_directories(${GLFW_DIR}/include)

# --- Finding GLAD ---
# For GLAD, we just need its include directory (glad.c will be compiled with our sources)
include_directories(${GLAD_INCLUDE_DIR})

# --- Finding GLM ---
# For GLM (header-only), we just need its include directory
# The actual GLM headers are in a subfolder usually called 'glm', so if GLM_INCLUDE_DIR
# is C:/dev/libs, then your includes like #include <glm/glm.hpp> will work.
include_directories(${GLM_INCLUDE_DIR})


# --- OpenGL ---
# Find the OpenGL package (standard on most systems)
find_package(OpenGL REQUIRED)

# Explicitly set the language for glad.c
set_source_files_properties(src/glad.c PROPERTIES LANGUAGE C)

# --- Our Executable ---
# Define the executable name and list its source files
add_executable(SolarSystemApp 
    src/main.cpp
    src/glad.c  # We need to compile glad.c
)

# --- Linking Libraries ---
# Link GLFW and OpenGL to our application
# For MinGW, GLFW often needs to be linked with gdi32
target_link_libraries(SolarSystemApp PRIVATE 
    ${GLFW_DIR}/lib-mingw-w64/libglfw3dll.a  # Or libglfw3.a for static
    OpenGL::GL # Modern way to link OpenGL
    gdi32      # Windows GDI library, often needed by GLFW on MinGW
)

# --- Copy GLFW DLL (Optional, but helpful for running) ---
# This command will copy glfw3.dll to your build directory where the .exe is
# Ensure the source path to glfw3.dll is correct.
add_custom_command(TARGET SolarSystemApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${GLFW_DIR}/lib-mingw-w64/glfw3.dll" # Source DLL
    $<TARGET_FILE_DIR:SolarSystemApp>     # Destination: where the .exe is
    COMMENT "Copying GLFW DLL to build directory"
)

# Print a message to indicate where the include directories are (for debugging)
message(STATUS "GLFW include directory: ${GLFW_DIR}/include")
message(STATUS "GLAD include directory: ${GLAD_INCLUDE_DIR}")
message(STATUS "GLM include directory: ${GLM_INCLUDE_DIR}")
message(STATUS "GLFW library path: ${GLFW_DIR}/lib-mingw-w64/libglfw3dll.a")