#include <iostream> 
#include<fstream>
#include<sstream>
#include<string>
#include <cmath>
#include<vector>

using namespace std;
#include <glad/glad.h> 

#include <GLFW/glfw3.h>

// NEW: GLM Headers
#include <glm/glm.hpp> // Core GLM stuff (vectors, matrices)
#include <glm/gtc/matrix_transform.hpp> // For operations like translate, rotate, scale
#include <glm/gtc/type_ptr.hpp> // For converting GLM types to pointers for OpenGL


// Window dimensions
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


float cameraDistance = 15.0f; // Initial distance from the origin
float cameraYaw = -90.0f;     // Horizontal angle (degrees), -90 looks towards +Z if using typical setup
float cameraPitch = 20.0f;    // Vertical angle (degrees), 20 degrees looking slightly down from above
glm::vec3 cameraTargetPos = glm::vec3(0.0f, 0.0f, 0.0f); // Camera always looks at the origin
glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

// Mouse state for dragging
bool firstMouse = true;
bool mouseDragActive = false;
double lastMouseX = SCR_WIDTH / 2.0; // Initialize to center of screen
double lastMouseY = SCR_HEIGHT / 2.0;
const float mouseSensitivity = 0.1f;
const float zoomSensitivity = 1.0f;


struct Sphere {
    std::vector<float> vertices; // Will contain x,y,z, nx,ny,nz, s,t
    std::vector<unsigned int> indices;
    int vertexCount = 0;
    int indexCount = 0;
    int stride = 8; // 3 for pos, 3 for normal, 2 for texcoord (x,y,z, nx,ny,nz, s,t)
};
//fun for sphere
Sphere createSphere(float radius, int sectorCount, int stackCount) {
    Sphere sphere;
    sphere.vertexCount = (sectorCount + 1) * (stackCount + 1);
    sphere.indexCount = stackCount * sectorCount * 6;
    sphere.vertices.resize(sphere.vertexCount * sphere.stride);
    sphere.indices.resize(sphere.indexCount);

    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
    float s, t;                                     // vertex texCoord

    float sectorStep = 2 * M_PI / sectorCount;
    float stackStep = M_PI / stackCount;
    float sectorAngle, stackAngle;

    int vertexArrayIndex = 0;
    for (int i = 0; i <= stackCount; ++i) {
        stackAngle = M_PI / 2 - i * stackStep;      // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        for (int j = 0; j <= sectorCount; ++j) {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            sphere.vertices[vertexArrayIndex++] = x;
            sphere.vertices[vertexArrayIndex++] = y;
            sphere.vertices[vertexArrayIndex++] = z;

            // normalized vertex normal (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            sphere.vertices[vertexArrayIndex++] = nx;
            sphere.vertices[vertexArrayIndex++] = ny;
            sphere.vertices[vertexArrayIndex++] = nz;

            // vertex tex coord (s, t) range between [0, 1]
            s = (float)j / sectorCount;
            t = (float)i / stackCount;
            sphere.vertices[vertexArrayIndex++] = s;
            sphere.vertices[vertexArrayIndex++] = t;
        }
    }

    int k1, k2;
    int indexArrayIndex = 0;
    for (int i = 0; i < stackCount; ++i) {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            // 2 triangles per sector excluding first and last stacks
            if (i != 0) {
                sphere.indices[indexArrayIndex++] = k1;
                sphere.indices[indexArrayIndex++] = k2;
                sphere.indices[indexArrayIndex++] = k1 + 1;
            }

            if (i != (stackCount - 1)) {
                sphere.indices[indexArrayIndex++] = k1 + 1;
                sphere.indices[indexArrayIndex++] = k2;
                sphere.indices[indexArrayIndex++] = k2 + 1;
            }
        }
    }
    return sphere;
}


// Function to load shader source from a file
string loadShaderSourceFromFile(const string& filePath) {
    ifstream shaderFile;
    stringstream shaderStream;
    // Ensure ifstream objects can throw exceptions:
    shaderFile.exceptions(ifstream::failbit | ifstream::badbit);
    try {
        // Open file
        shaderFile.open(filePath);
        // Read file's buffer contents into streams
        shaderStream << shaderFile.rdbuf();
        // Close file handlers
        shaderFile.close();
        // Convert stream into string
        return shaderStream.str();
    } catch (ifstream::failure& e) {
        cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << filePath << "\n" << e.what() << endl;
        return "";
    }
}




// Function to be called when mouse buttons are pressed/released
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            mouseDragActive = true;
            glfwGetCursorPos(window, &lastMouseX, &lastMouseY); // Get current pos when drag starts
            firstMouse = true; // Reset firstMouse for new drag sequence
            // Optional: glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Hide/grab cursor
        } else if (action == GLFW_RELEASE) {
            mouseDragActive = false;
            // Optional: glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // Show cursor
        }
    }
}

// Function to be called when the mouse cursor moves
void mouse_cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (!mouseDragActive) {
        return; // Only process if dragging
    }

    if (firstMouse) {
        lastMouseX = xpos;
        lastMouseY = ypos;
        firstMouse = false;
    }

    double xoffset = xpos - lastMouseX;
    double yoffset = lastMouseY - ypos; // Reversed since y-coordinates go from top to bottom

    lastMouseX = xpos;
    lastMouseY = ypos;

    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    cameraYaw += (float)xoffset;
    cameraPitch += (float)yoffset;

    // Constrain pitch to avoid flipping
    if (cameraPitch > 89.0f)
        cameraPitch = 89.0f;
    if (cameraPitch < -89.0f)
        cameraPitch = -89.0f;
}

// Function to be called when the mouse scroll wheel is used
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    cameraDistance -= (float)yoffset * zoomSensitivity; // yoffset is usually vertical scroll
    
    // Clamp zoom distance
    if (cameraDistance < 2.0f)
        cameraDistance = 2.0f;
    if (cameraDistance > 50.0f) // Adjust max zoom as needed
        cameraDistance = 50.0f;
}


// Function prototypes (we'll define these below main or in other files later)
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);


//celestial bodies parameters
const float SUN_SCALE = 2.0f;

const float PLANET1_ORBIT_RADIUS = 5.0f;
const float PLANET1_SCALE = 0.5f;
const float PLANET1_ORBIT_SPEED = 0.5f;      // Radians per second (adjust for desired speed)
const float PLANET1_SELF_ROTATION_SPEED = 1.5f; // Radians per second

// NEW: Parameters for Planet 2
const float PLANET2_ORBIT_RADIUS = 8.0f; // Further out
const float PLANET2_SCALE = 0.7f;      // A bit larger than Planet 1
const float PLANET2_ORBIT_SPEED = 0.3f;      // Slower orbit
const float PLANET2_SELF_ROTATION_SPEED = 1.0f;

// NEW: Parameters for the Moon (orbiting Planet 2)
const float MOON_ORBIT_RADIUS_FROM_PLANET = 1.5f; // Distance from its parent planet
const float MOON_SCALE = 0.2f;
const float MOON_ORBIT_SPEED_AROUND_PLANET = 2.0f;
const float MOON_SELF_ROTATION_SPEED = 2.5f;





int main() {
    // 1. Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    cout << "GLFW initialized successfully." << endl;

    // 2. Configure GLFW (Set OpenGL version and profile)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.x
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // We want OpenGL x.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We want the Core profile (no deprecated functions)
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on macOS, optional on others

    // 3. Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Solar System - by Bishnu", NULL, NULL);
    if (window == NULL) {
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate(); // Clean up GLFW
        return -1;
    }
    cout << "GLFW window created successfully." << endl;
    glfwMakeContextCurrent(window); // Make the window's context the current one on this thread

    // Register our framebuffer_size_callback function with GLFW
    // This function will be called whenever the window is resized.
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // NEW: Register mouse callbacks
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Optional: to see the cursor normally unless dragging
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    


    // 4. Initialize GLAD (Load OpenGL function pointers)
    // GLAD needs a function to get the address of OpenGL functions (provided by GLFW here)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cerr << "Failed to initialize GLAD" << endl;
        glfwTerminate();
        return -1;
    }
    cout << "GLAD initialized successfully." << endl;

        
    // NEW: Enable Depth Testing 
    glEnable(GL_DEPTH_TEST); 


    // The viewport tells OpenGL how to map its normalized device coordinates (-1 to 1)
    // to window coordinates (pixels). We usually want it to match the window size.
    // The first two parameters set the location of the lower-left corner of the window.
    // The third and fourth parameter set the width and height of the rendering window in pixels.
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
   


    string vertexShaderCode = loadShaderSourceFromFile("../shaders/triangle.vert"); // Path relative to build dir
    string fragmentShaderCode = loadShaderSourceFromFile("../shaders/triangle.frag"); // Path relative to build dir


    if (vertexShaderCode.empty() || fragmentShaderCode.empty()) {
    std::cerr << "Failed to load shader files. Exiting." << std::endl;
    glfwTerminate();
    return -1;
   }

   const char *vertexShaderSource = vertexShaderCode.c_str();
   const char *fragmentShaderSource = fragmentShaderCode.c_str();


    //compile and link shaders
    //vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1,&vertexShaderSource,NULL);
    glCompileShader(vertexShader);

    // Check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
         cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
     } else {
         cout << "Vertex shader compiled successfully." << endl;
     }


    // Fragment Shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // Check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
    } else {
        cout << "Fragment shader compiled successfully." << endl;
    }

    
    //Link shaders into  shader program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram,vertexShader);
    glAttachShader(shaderProgram,fragmentShader);
    glLinkProgram(shaderProgram);
    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    } else {
        std::cout << "Shader program linked successfully." << std::endl;
    }
    // Shaders are now linked into the program, so we don't need the individual shader objects anymore.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
     // Get the location of the 'ourColor' uniform in the shader program
     int ourColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
     if (ourColorLocation == -1) {
         cerr << "Warning: uniform 'ourColor' not found in shader!" << endl;
     } else {
         cout << "'ourColor' uniform location: " << ourColorLocation << endl;
     }
     
    // NEW: Get the location of the 'model' matrix uniform
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    if (modelLoc == -1) {
        std::cerr << "Warning: uniform 'model' not found in shader!" << std::endl;
    } else {
        std::cout << "'model' uniform location: " << modelLoc << std::endl;
    }
    


     // NEW: Get locations for view and projection matrices
     int viewLoc = glGetUniformLocation(shaderProgram, "view");
     if (viewLoc == -1) {
         std::cerr << "Warning: uniform 'view' not found in shader!" << std::endl;
     } else {
         std::cout << "'view' uniform location: " << viewLoc << std::endl;
     }
 
     int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
     if (projectionLoc == -1) {
         std::cerr << "Warning: uniform 'projection' not found in shader!" << std::endl;
     } else {
         std::cout << "'projection' uniform location: " << projectionLoc << std::endl;
     }





    // --- Generate Sphere Data ---
    Sphere mySphere = createSphere(1.0f, 36, 18); // Radius 1, 36 sectors, 18 stacks

    
    // Set up vertex data (and buffer(s)) and configure vertex attributes
    unsigned int VBO,VAO,EBO;
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1, &EBO); 

    // 1. Bind Vertex Array Object (VAO)
    glBindVertexArray(VAO);

    // Bind VBO and upload vertex data (for sphere)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mySphere.vertices.size() * sizeof(float), mySphere.vertices.data(), GL_STATIC_DRAW);



    // 2. Copy our vertices array into a vertex buffer for OpenGL to use
    //glBindBuffer(GL_ARRAY_BUFFER, VBO);
   // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // GL_STATIC_DRAW: the data will most likely not change or change very rarely.
    // GL_DYNAMIC_DRAW: the data is likely to change a lot.
    // GL_STREAM_DRAW: the data will change every time it is drawn.

    // Bind EBO and upload index data for sphere
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // NEW: Bind EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mySphere.indices.size() * sizeof(unsigned int), mySphere.indices.data(), GL_STATIC_DRAW); // NEW: Upload indices
    
    // 3. Set the vertex attributes pointers
    // Attribute location 0 (aPos in vertex shader), 3 components (vec3), type float,
    // not normalized, stride (bytes between consecutive vertices), offset (where data begins in buffer)
    // Stride is now 8 floats (x,y,z, nx,ny,nz, s,t)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, mySphere.stride * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
  
    // Normal attribute (location = 1) - We'll set this up but might not use it in the shader yet
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, mySphere.stride * sizeof(float), (void*)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);
  
    // Texture coordinate attribute (location = 2) - We'll set this up but might not use it yet
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, mySphere.stride * sizeof(float), (void*)(6 * sizeof(float)));
    // glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind the VAO so we don't accidentally modify it.

    // 5. Render Loop (The "Game Loop")

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // Clear color and depth buffers
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Slightly darker background
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear depth buffer too

        glUseProgram(shaderProgram);

        // Update 'ourColor' uniform (pulsating green)
        float timeValue = (float)glfwGetTime();
       // float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
      //  if (ourColorLocation != -1) {
       //     glUniform4f(ourColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
       // }



        // --- View Matrix (Camera Position/Orientation) ---
        // Position the camera a bit back from the origin to see the sphere
        // --- Calculate Camera Position for Orbital Camera ---
       glm::vec3 cameraPos;
       cameraPos.x = cameraDistance * cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
       cameraPos.y = cameraDistance * sin(glm::radians(cameraPitch));
       cameraPos.z = cameraDistance * sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f); // Looks at the origin
        glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f); // Head is up (set to 0,-1,0 to look upside-down)
        
        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
        if (viewLoc != -1) {
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        }

        // --- Projection Matrix (Perspective) ---
        // Parameters: Field of View (FOV), Aspect Ratio, Near plane, Far plane
        float fov = glm::radians(45.0f); // 45 degree field of view
        float aspectRatio = (float)SCR_WIDTH / (float)SCR_HEIGHT; // Calculate from window size
        float nearPlane = 0.1f;
        float farPlane = 100.0f;

        glm::mat4 projection = glm::perspective(fov, aspectRatio, nearPlane, farPlane);
        if (projectionLoc != -1) {
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        }

    
        glBindVertexArray(VAO); // Bind sphere VAO once, as both Sun and Planet use it
        
         // Draw the Sun 
         glm::mat4 sunModel = glm::mat4(1.0f);
         sunModel = glm::scale(sunModel, glm::vec3(SUN_SCALE)); // Scale the Sun
    
         // sunModel = glm::rotate(sunModel, timeValue * glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //rotation of the sun optional
         if (modelLoc != -1) {
             glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(sunModel));
         }
         // Set Sun's color (e.g., yellow)
         if (ourColorLocation != -1) {
             glUniform4f(ourColorLocation, 1.0f, 1.0f, 0.0f, 1.0f); // Yellow
         }
         glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mySphere.indices.size()), GL_UNSIGNED_INT, 0);
 
 
         // --- Draw Planet 1 ---
         glm::mat4 planet1Model = glm::mat4(1.0f);
         
         // 1. Orbit: Rotate around the Sun (which is at the origin)
         float planet1OrbitAngle = timeValue * PLANET1_ORBIT_SPEED;
         planet1Model = glm::rotate(planet1Model, planet1OrbitAngle, glm::vec3(0.0f, 1.0f, 0.0f)); // Orbit around Y-axis
         
         // 2. Translate out to its orbital distance
         planet1Model = glm::translate(planet1Model, glm::vec3(PLANET1_ORBIT_RADIUS, 0.0f, 0.0f)); // Translate along new X-axis
         
         // 3. Self-rotation (around its own Y-axis)
         float planet1SelfRotationAngle = timeValue * PLANET1_SELF_ROTATION_SPEED;
         planet1Model = glm::rotate(planet1Model, planet1SelfRotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
         
         // 4. Scale the planet
         planet1Model = glm::scale(planet1Model, glm::vec3(PLANET1_SCALE));
 
         if (modelLoc != -1) {
             glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(planet1Model));
         }
         // Set Planet's color (e.g., blue)
         if (ourColorLocation != -1) {
             glUniform4f(ourColorLocation, 0.2f, 0.3f, 1.0f, 1.0f); // Blueish
         }
         glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mySphere.indices.size()), GL_UNSIGNED_INT, 0);
 

        // --- Draw Planet 2 ---
        glm::mat4 planet2BaseTransform = glm::mat4(1.0f); // For planet's center position
        float planet2OrbitAngle = timeValue * PLANET2_ORBIT_SPEED;
        planet2BaseTransform = glm::rotate(planet2BaseTransform, planet2OrbitAngle, glm::vec3(0.0f, 1.0f, 0.0f)); // Orbit around Sun
        planet2BaseTransform = glm::translate(planet2BaseTransform, glm::vec3(PLANET2_ORBIT_RADIUS, 0.0f, 0.0f)); // Translate to orbit

        glm::mat4 planet2Model = planet2BaseTransform; // Start with base orbital position
        float planet2SelfRotationAngle = timeValue * PLANET2_SELF_ROTATION_SPEED;
        planet2Model = glm::rotate(planet2Model, planet2SelfRotationAngle, glm::vec3(0.0f, 1.0f, 0.0f)); // Self-rotation
        planet2Model = glm::scale(planet2Model, glm::vec3(PLANET2_SCALE)); // Scale

        if (modelLoc != -1) glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(planet2Model));
        if (ourColorLocation != -1) glUniform4f(ourColorLocation, 1.0f, 0.3f, 0.2f, 1.0f); // Reddish
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mySphere.indices.size()), GL_UNSIGNED_INT, 0);

        // --- Draw Moon (orbiting Planet 2) ---
        glm::mat4 moonModel = planet2BaseTransform; // Start from Planet 2's center position (before its self-rotation/scale)

        // 1. Moon's orbit around Planet 2
        float moonOrbitAngle = timeValue * MOON_ORBIT_SPEED_AROUND_PLANET;
        moonModel = glm::rotate(moonModel, moonOrbitAngle, glm::vec3(0.0f, 1.0f, 0.0f)); // Orbit around Planet 2's Y-axis
        moonModel = glm::translate(moonModel, glm::vec3(MOON_ORBIT_RADIUS_FROM_PLANET, 0.0f, 0.0f));

        // 2. Moon's self-rotation
        float moonSelfRotationAngle = timeValue * MOON_SELF_ROTATION_SPEED;
        moonModel = glm::rotate(moonModel, moonSelfRotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        
        // 3. Moon's scale
        moonModel = glm::scale(moonModel, glm::vec3(MOON_SCALE));

        if (modelLoc != -1) glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(moonModel));
        if (ourColorLocation != -1) glUniform4f(ourColorLocation, 0.7f, 0.7f, 0.7f, 1.0f); // Grey
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mySphere.indices.size()), GL_UNSIGNED_INT, 0);

         // glBindVertexArray(0); // Unbind VAO (optional if it's the last thing or only thing using this VAO)

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // NEW SECTION: De-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO); // NEW: Delete EBO
    glDeleteProgram(shaderProgram);

    // 6. Terminate GLFW (Clean up resources)
    cout << "Terminating GLFW." << endl;
    glfwTerminate();
    return 0; 
}

// This callback function is called whenever the window is resized.
// It adjusts the OpenGL viewport to match the new window dimensions.
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // Make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    cout << "Window resized to: " << width << "x" << height << endl;
    glViewport(0, 0, width, height);
}

// Process all input: query GLFW whether relevant keys are pressed/released
// this frame and react accordingly
void processInput(GLFWwindow *window) {
    // Check if the ESC key was pressed. If so, set the WindowShouldClose property to true.
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}