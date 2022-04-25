#version 460

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 uv;

layout (location = 0) out vec2 frag_uv;

void main() {
  gl_Position = position;
  frag_uv = uv;
}
