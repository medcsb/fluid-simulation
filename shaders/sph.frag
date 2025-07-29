#version 450 core

in vec3 vertColor;
in vec3 vertNormal;
in vec3 vertPos;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
//uniform vec3 viewPos;
uniform float ambientStrength;
//uniform float specularStrength;
//uniform int specularPower;
uniform float attenuationFactor;
uniform bool showDepth;

uniform float gamma;

float far = 100.0;
float near = 0.1;

float LogDepth(float depth) {
    float z = depth * 2.0 - 1.0;  // back to NDC
    float linear = (2.0 * near * far) / (far + near - z * (far - near));
    float logDepth = log(linear + 1.0) / log(far + 1.0); // normalized [0,1]
    return logDepth;
}

void main() {
    
    vec3 norm = normalize(vertNormal);
    vec3 directionToLight = lightPos - vertPos;
    float dist2 = dot(directionToLight, directionToLight);
    float attenuation = attenuationFactor / dist2;
    vec3 lightDir = normalize(directionToLight);
    float diff = max(dot(norm, lightDir), 0.1);

    // specular

    // ambient
    vec3 ambient = ambientStrength * lightColor;
    // diffuse
    vec3 diffuse = diff * lightColor;
    // specular

    if (attenuationFactor != 0) {
        diffuse = diff * lightColor * attenuation;
        ambient = ambientStrength * lightColor * attenuation;
    }

    vec3 result = (ambient + (diffuse)) * vertColor;

    if (showDepth) {
        float depth = LogDepth(gl_FragCoord.z);
        FragColor = vec4(vec3(depth), 1.0);
    } else {
        FragColor = vec4(result, 1.0);
    }

    //FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}