#version 450
layout(location=0) in vec3 inPos;
layout(location=1) in vec3 inNormal;
layout(location=2) in vec2 inUV;
layout(location=3) in vec3 inColor;

layout(push_constant) uniform Push { mat4 mvp; };

layout(location=0) out vec2 vUV;
layout(location=1) out vec3 vCol;

void main() {
    vUV  = inUV;
    vCol = inColor;
    gl_Position = mvp * vec4(inPos,1.0);
}
