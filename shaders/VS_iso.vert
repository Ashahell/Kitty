#version 450

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 bPos;
layout(location = 2) in uvec4 aCol;
layout(location = 3) in vec2 aUv;

layout(location = 0) flat out uint vPal;
layout(location = 1) out vec2 vUv;
layout(location = 2) out float vShade;
layout(location = 3) out float vUi;

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
    vec3 p = mix(aPos, bPos, pc.params.x);
    vec4 clip;
    if (pc.params.z > 0.5) {
        clip = vec4(p.xy, 0.0, 1.0);
    } else {
        clip = ubo.vp * (pc.model * vec4(p, 1.0));
    }
    gl_Position = clip;
    vPal = aCol.x & 15u;
    vShade = float(aCol.y) / 255.0;
    vUv = aUv;
    vUi = pc.params.z;
}
