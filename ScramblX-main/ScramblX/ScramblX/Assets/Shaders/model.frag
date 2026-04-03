#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform vec3 baseColor;
uniform bool hasTexture;

uniform vec3 lightPos;
uniform vec3 viewPos; // Now synced with camera
uniform vec3 lightColor;

void main() {
    // Ambient
    float ambientStrength = 1.0;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    float diffuseStrength = 1.0;
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diff * lightColor;

    // Specular - FIXED: uses viewPos
    float specularStrength = 0.8;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 512);
    vec3 specular = specularStrength * spec * lightColor;

    vec4 objectColor;
    if(hasTexture) {
        objectColor = texture(texture_diffuse1, TexCoords);
    } else {
        objectColor = vec4(baseColor, 1.0);
    }

    // Apply lighting to RGB, keep texture Alpha
    vec3 result = (ambient + diffuse + specular) * objectColor.rgb;
    FragColor = vec4(result, objectColor.a);
}