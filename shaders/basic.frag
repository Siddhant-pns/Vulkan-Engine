#version 450

layout(location = 0) in vec2 fragUV;
layout(location = 1) in vec3 fragColor;

layout(set = 0, binding = 0) uniform sampler2D tex;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 texColor = texture(tex, fragUV);
    outColor = texColor * vec4(fragColor, 1.0);
}
