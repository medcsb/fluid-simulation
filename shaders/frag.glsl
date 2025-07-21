#version 450 core

out vec4 FragColor;

in vec3 vertColor;
in vec2 TexCoord;

uniform sampler2D texture1;

void main() {
    vec3 lightColor = vec3(1.0, 0.0, 0.0);
    FragColor = texture(texture1, TexCoord) * vec4(vertColor * lightColor, 1.0);
    //FragColor = vec4(vertColor, 1.0);
}