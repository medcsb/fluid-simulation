#version 450 core

out vec4 FragColor;

in vec3 vertColor;
in vec2 TexCoord;

uniform sampler2D Texture;

void main() {
    FragColor = texture(Texture, TexCoord) * vec4(vertColor, 1.0);
}