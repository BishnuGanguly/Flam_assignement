#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;       // NEW: View matrix
uniform mat4 projection; // NEW: Projection matrix

void main() {
    // Order is important: Model -> View -> Projection
    gl_Position = projection * view * model * vec4(aPos, 1.0); 
}