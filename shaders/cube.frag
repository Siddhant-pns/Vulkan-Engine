#version 450
layout(set=0,binding=0) uniform sampler2D uTex;

layout(location=0) in vec2 vUV;
layout(location=1) in vec3 vCol;
layout(location=0) out vec4 outColor;

void main(){
    outColor = texture(uTex, vUV) * vec4(vCol,1.0);
}
