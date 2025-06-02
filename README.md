# Flam_assignement
# Mini Solar System Visualization in OpenGL C++

## 🚀 Description

This project is a 3D visualization of a mini solar system, created using C++ and modern OpenGL (targeting concepts similar to ES 2.0+/3.0). It serves as a demonstration of core computer graphics principles including 3D transformations, custom shaders, camera manipulation, and object hierarchy. The scene features a central Sun, multiple planets orbiting at different speeds and distances, and a moon orbiting one of the planets.

## ✨ Features Implemented

* **Central Sun:** A static (or slowly rotating) Sun at the center of the system.
* **Orbiting Planets:** Two planets orbiting the Sun, each with:
    * Different orbital radii and speeds.
    * Self-rotation on their own axes at different speeds.
* **Moon:** One planet has a moon that orbits it, demonstrating hierarchical transformations.
* **3D Spherical Objects:** All celestial bodies are rendered as spheres.
* **Custom Shaders (GLSL):**
    * Vertex shader for 3D transformations (model, view, projection).
    * Fragment shader for basic object coloring (colors set via uniforms).
* **Matrix Transformations:** Extensive use of the GLM (OpenGL Mathematics) library for:
    * **Model matrices:** To position, scale, and rotate each celestial body.
    * **View matrix:** To control the camera's position and orientation.
    * **Projection matrix:** To provide 3D perspective.
* **Interactive Camera:**
    * **Orbital Camera:** The camera orbits around the center of the solar system (the origin).
    * **Mouse Drag (Left Button):** Rotate the camera view (adjusting yaw and pitch).
    * **Mouse Scroll Wheel:** Zoom in and out by changing the camera's distance to the target.
* **Efficient Rendering:** Utilizes Vertex Buffer Objects (VBOs), Vertex Array Objects (VAOs), and Element Buffer Objects (EBOs) for managing and rendering sphere geometry.
* **Depth Testing:** Enabled for correct 3D object occlusion.

## 🛠️ Technologies Used

* **Language:** C++ (C++17 standard)
* **Graphics API:** OpenGL 3.3 Core Profile
* **Libraries:**
    * **GLFW:** For window creation, OpenGL context management, and input handling.
    * **GLAD:** For loading OpenGL function pointers.
    * **GLM (OpenGL Mathematics):** For vector and matrix operations.
* **Build System:** CMake (version 3.10 or higher)
* **Compiler:** MinGW (GCC for Windows) was used during development.

## ⚙️ Setup, Build, and Run Instructions

### Prerequisites

1.  A C++ compiler that supports C++17 (e.g., MinGW on Windows, GCC on Linux, Clang on macOS).
2.  CMake (version 3.10 or higher) installed and added to your system PATH.
3.  **Library Files:**
    * **GLFW:** Header files and pre-compiled library (`.a` for MinGW, `.dll`).
    * **GLAD:** Generated header (`glad.h`, `khrplatform.h`) and source (`glad.c`) files.
    * **GLM:** Header files.

    *Note: The `CMakeLists.txt` in this repository is configured with specific paths for these libraries (e.g., `C:/Users/bishn/Downloads/...`). If you clone this repository, you will likely need to:*
    * *Download these libraries yourself.*
    * *Place them in the locations expected by `CMakeLists.txt`, OR*
    * *Modify the `set(GLFW_DIR ...)`, `set(GLAD_INCLUDE_DIR ...)`, and `set(GLM_INCLUDE_DIR ...)` paths in `CMakeLists.txt` to point to where you have stored these libraries on your system.*

### Build Steps

1.  **Clone the repository (or download the source code):**
    ```bash
    git clone [https://github.com/BishnuGanguly/Flam_assignement.git](https://github.com/BishnuGanguly/Flam_assignement.git)
    cd Flam_assignement
    ```

2.  **Create a build directory and navigate into it:**
    ```bash
    mkdir build
    cd build
    ```

3.  **Run CMake to generate build files:**
    * For MinGW on Windows (from the `build` directory):
        ```bash
        cmake .. -G "MinGW Makefiles"
        ```
    * For other systems/compilers, adjust the generator if needed (e.g., just `cmake ..` on Linux often works).

4.  **Compile the project:**
    * If using MinGW Makefiles:
        ```bash
        mingw32-make
        ```
    * If using standard Makefiles (Linux/macOS):
        ```bash
        make
        ```

5.  **Locate the Executable:**
    The executable (e.g., `SolarSystemApp.exe` on Windows or `SolarSystemApp` on Linux/macOS) will be located in the `build` directory.

6.  **Runtime Dependency (Windows):**
    Ensure `glfw3.dll` is in the same directory as the executable, or in a directory listed in your system's PATH. The CMake script attempts to copy this DLL to the build directory.

### Running the Application

Navigate to the `build` directory in your terminal and run the executable:
* Windows: `.\SolarSystemApp.exe`
* Linux/macOS: `./SolarSystemApp`

## 🎮 Controls

* **Mouse Drag (Hold Left Button + Move Mouse):** Rotate the camera view around the solar system.
* **Mouse Scroll Wheel:** Zoom the camera in or out.
* **ESC Key:** Close and exit the application.
