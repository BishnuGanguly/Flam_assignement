#version 330 core
out vec4 FragColor;

uniform float time; // To control the pulsing

void main() {
    float pulse = 0.75 + 0.25 * sin(time * 5.0); // Adjust speed of pulse with (time * N)

    // Base color (e.g., bright yellow/orange)
    vec3 sunColor = vec3(1.0, 0.8, 0.2); 

    FragColor = vec4(sunColor * pulse, 1.0);
}