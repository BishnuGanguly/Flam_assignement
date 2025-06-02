#version 330 core
out vec4 FragColor; // Output color for the fragment

uniform vec4 ourColor; // We'll set this variable from the C++ program

void main() {
        FragColor = ourColor; // Use the uniform color
}