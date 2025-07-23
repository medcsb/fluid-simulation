#version 450 core

out vec4 FragColor;

//in vec3 vertColor;

uniform vec3 lightColor;
uniform float opaqueVal;

void main() {
    FragColor = vec4(lightColor, opaqueVal);
}