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

layout(set = 0, binding = 1) uniform sampler2D tex;

layout(push_constant) uniform Push {
    mat4 model;
    vec4 tint;
    vec4 params;
} pc;

const float bayer[16] = float[16](
    0.0,  8.0,  2.0, 10.0,
   12.0,  4.0, 14.0,  6.0,
    3.0, 11.0,  1.0,  9.0,
   15.0,  7.0, 13.0,  5.0
);

void main() {
    int bx = int(gl_FragCoord.x) & 3;
    int by = int(gl_FragCoord.y) & 3;
    float threshold = (bayer[by * 4 + bx] + 0.5) / 16.0;
    if (pc.tint.a < threshold) discard;
    vec3 c = ubo.pal[vPal].rgb * vShade;
    c *= texture(tex, vUv * 2.0).rgb;
    fragColor = vec4(c * pc.tint.rgb, 1.0);
}
