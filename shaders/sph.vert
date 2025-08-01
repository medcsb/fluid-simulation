#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 instancePos;
layout(location = 3) in vec3 instanceColor;

out vec3 vertColor;
out vec3 vertNormal;
out vec3 vertPos;

uniform mat4 view;
uniform mat4 projection;
uniform float radius;

void main() {
    vec3 worldPos = aPos * radius + instancePos;
    gl_Position = projection * view * vec4(worldPos, 1.0);
    vertPos = worldPos;
    vertColor = vec3(0.0f, 0.0f, 1.0f); // Default color
    float speed = sqrt(dot(instanceColor, instanceColor));
    speed = speed / 1000.0f;
    vertColor.x = speed; // Use speed to set the red channel
    vertColor.y = 0.0f;
    vertColor.z = 1.0f - speed; // Use speed to set the blue channel
    vertNormal = normalize(aNormal); 
    
}