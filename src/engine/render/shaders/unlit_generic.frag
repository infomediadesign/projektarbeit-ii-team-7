#version 460

layout (location = 0) in vec2 uv;

layout (location = 0) out vec4 v_color;

layout (binding = 0) uniform sampler2D color_texture;

void main() {
  v_color = texture(color_texture, uv);
}
