#version 450 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
//layout (location = 1) in vec3 aColor;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
  
out vec3 vertPos;
//out vec3 vertColor;
out vec3 vertNormal;
out vec2 TexCoord;

out vec4 fragPosLightSpace;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main() {
    gl_Position = projection * view * transform * vec4(aPos, 1.0); // see how we directly give a vec3 to vec4's constructor
    vertPos = vec3(transform * vec4(aPos, 1.0));
    //vertColor = aColor;
    vertNormal = mat3(transpose(inverse(transform))) * aNormal;
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
    fragPosLightSpace = lightSpaceMatrix * vec4(vertPos, 1.0);
}