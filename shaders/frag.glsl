#version 450 core

out vec4 FragColor;

in vec3 vertPos;
in vec3 vertColor;
in vec3 vertNormal;
in vec2 TexCoord;

uniform sampler2D texture1;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform float ambientStrength;
uniform float specularStrength;
uniform int specularPower;
uniform bool useTexture;

void main() {
    // ambient
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(vertNormal);
    vec3 lightDir = normalize(lightPos - vertPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    vec3 viewDir = normalize(viewPos - vertPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularPower);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular);

    vec4 texColor = useTexture ? texture(texture1, TexCoord) : vec4(1.0);

    FragColor = vec4(result, 1.0) * texColor;
}