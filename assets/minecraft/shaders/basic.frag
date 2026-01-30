#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in float AO;

uniform sampler2D texture_atlas;

void main() {
    vec4 texColor = texture(texture_atlas, TexCoord);
    if(texColor.a < 0.1)
        discard;
    
    // Apply AO (simplified)
    float aoFactor = mix(0.5, 1.0, AO);
    FragColor = texColor * aoFactor;
}
