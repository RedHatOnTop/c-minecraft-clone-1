#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in float AO;
in float Light;
in vec3 ViewPos;

uniform sampler2D texture_atlas;
uniform vec3 fogColor;
uniform float fogNear;
uniform float fogFar;

void main() {
    vec4 texColor = texture(texture_atlas, TexCoord);

    float aoFactor = mix(0.5, 1.0, AO);
    vec3 lit = texColor.rgb * aoFactor * clamp(Light, 0.05, 1.0);

    float dist = length(ViewPos);
    float fogFactor = clamp((fogFar - dist) / (fogFar - fogNear), 0.0, 1.0);

    FragColor = vec4(mix(fogColor, lit, fogFactor), 1.0);
}

