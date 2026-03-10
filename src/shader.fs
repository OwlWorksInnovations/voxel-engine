#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture1;

void main()
{
    // Light settings
    vec3 lightDir = normalize(vec3(-0.5, 1.0, 0.3));
    vec3 lightColor = vec3(1.0, 1.0, 0.9);          
    float ambientStrength = 0.4;
    
    // Ambient
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Result
    vec3 result = (ambient + diffuse) * texture(texture1, TexCoord).rgb;
    FragColor = vec4(result, 1.0);
}
