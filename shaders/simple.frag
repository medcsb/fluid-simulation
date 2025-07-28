#version 450 core

out vec4 FragColor;

in vec3 vertPos;
//in vec3 vertColor;
in vec3 vertNormal;
in vec2 TexCoord;

in vec4 fragPosLightSpace;

uniform sampler2D texture1;
uniform sampler2D shadowMap;

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
uniform bool shadowsOn;

uniform float gamma;

float far = 100.0;
float near = 0.1;

float LogDepth(float depth) {
    float z = depth * 2.0 - 1.0;  // back to NDC
    float linear = (2.0 * near * far) / (far + near - z * (far - near));
    float logDepth = log(linear + 1.0) / log(far + 1.0); // normalized [0,1]
    return logDepth;
}

float ShadowCalculation(vec4 posSpaceLight) {
    // Perform perspective divide
    vec3 projCoords = posSpaceLight.xyz / posSpaceLight.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    // Get closest depth value from light's perspective
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    vec3 normal = normalize(vertNormal);
    vec3 lightDir = normalize(lightPos - vertPos);
    
    // Add bias to prevent shadow acne
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.001);  
    bias = 0.0005;
    float shadow = currentDepth - bias> closestDepth ? 1.0 : 0.0;
    
    return shadow;
}

void main() {
    // ambient
    // diffuse
    vec3 norm = normalize(vertNormal);
    vec3 directionToLight = lightPos - vertPos;
    float dist2 = dot(directionToLight, directionToLight);
    float attenuation = attenuationFactor / dist2;
    vec3 lightDir = normalize(directionToLight);
    float diff = max(dot(norm, lightDir), 0.0);

    // specular
    vec3 viewDir = normalize(viewPos - vertPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularPower);

    vec3 ambient = ambientStrength * lightColor;
    vec3 diffuse = diff * lightColor;
    vec3 specular = specularStrength * spec * lightColor;

    if (attenuationFactor != 0) {
        diffuse = diff * lightColor * attenuation;
        ambient = ambientStrength * lightColor * attenuation;
        specular = specularStrength * spec * lightColor * attenuation;
    }

    // calculate shadows
    float shadow = 0.0;
    if (shadowsOn) shadow = ShadowCalculation(fragPosLightSpace);

    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    vec4 texColor = useTexture ? texture(texture1, TexCoord) : vec4(1.0);

    if (showDepth) {
        float depth = LogDepth(gl_FragCoord.z);
        FragColor = vec4(vec3(depth), 1.0);
    } else {
        FragColor = vec4(result, 1.0) * texColor; // maybe needs to change
    }

    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}