#version 450

layout(location = 0) flat in uint vPal;
layout(location = 1) in vec2 vUv;
layout(location = 2) in float vShade;
layout(location = 3) in float vUi;

layout(location = 0) out vec4 fragColor;

layout(set = 0, binding = 0) uniform Ubo {
    mat4 vp;
    vec4 pal[16];
    vec4 params;
} ubo;

layout(push_constant) uniform Push {
    mat4 model;
    vec4 tint;
    vec4 params;
} pc;

void main() {
    vec3 c = ubo.pal[vPal].rgb * vShade * pc.tint.rgb;
    fragColor = vec4(c, 1.0);
}
