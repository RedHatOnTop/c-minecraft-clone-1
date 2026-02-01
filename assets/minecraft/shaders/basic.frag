#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in float AO;
in float Light;

uniform sampler2D texture_atlas;
uniform vec3 fogColor;
uniform float fogNear;
uniform float fogFar;

void main() {
    vec4 texColor = texture(texture_atlas, TexCoord);
    if(texColor.a < 0.1)
        discard;
    
    // Apply AO
    float aoFactor = mix(0.5, 1.0, AO);
    vec3 color = texColor.rgb * aoFactor * clamp(Light, 0.05, 1.0);

    // Apply Fog
    float dist = gl_FragCoord.z / gl_FragCoord.w;
    float fogFactor = clamp((fogFar - dist) / (fogFar - fogNear), 0.0, 1.0);
    
    FragColor = vec4(mix(fogColor, color, fogFactor), texColor.a);
}
