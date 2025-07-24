#version 450 core

out vec4 FragColor;

in vec3 vertPos;
//in vec3 vertColor;
in vec3 vertNormal;
in vec2 TexCoord;

uniform sampler2D texture1;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform vec3 color;
uniform float ambientStrength;
uniform float specularStrength;
uniform int specularPower;
uniform float attenuationFactor;
uniform bool useTexture;
uniform bool showDepth;

float far = 100.0;
float near = 0.1;

float LogDepth(float depth) {
    float z = depth * 2.0 - 1.0;  // back to NDC
    float linear = (2.0 * near * far) / (far + near - z * (far - near));
    float logDepth = log(linear + 1.0) / log(far + 1.0); // normalized [0,1]
    return logDepth;
}

void main() {
    // ambient
    // diffuse
    vec3 norm = normalize(vertNormal);
    vec3 directionToLight = lightPos - vertPos;
    float dist2 = dot(directionToLight, directionToLight);
    float attenuation = attenuationFactor / sqrt(dist2);
    vec3 lightDir = normalize(directionToLight);
    float diff = max(dot(norm, lightDir), 0.0);

    // specular
    vec3 viewDir = normalize(viewPos - vertPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularPower);

    vec3 ambient = vec3(1.0, 1.0, 1.0) * lightColor * ambientStrength * attenuationFactor;
    vec3 diffuse = vec3(1.0, 1.0, 1.0) * lightPos * specularStrength;
    vec3 specular = vec3(1.0, 1.0, 1.0) * viewPos * specularPower;

    if (attenuationFactor == 0) {
        diffuse = diff * lightColor;
        ambient = ambientStrength * lightColor;
        specular = specularStrength * spec * lightColor;
    } else {
        diffuse = diff * lightColor * attenuation;
        ambient = ambientStrength * lightColor * attenuation;
        specular = specularStrength * spec * lightColor * attenuation;
    }

    vec3 result = (ambient + diffuse + specular) * color;

    vec4 texColor = useTexture ? texture(texture1, TexCoord) : vec4(1.0);

    if (showDepth) {
        float depth = LogDepth(gl_FragCoord.z);
        FragColor = vec4(vec3(depth), 1.0);
    } else {
        FragColor = vec4(result, 1.0) * texColor; // maybe needs to change
    }
}