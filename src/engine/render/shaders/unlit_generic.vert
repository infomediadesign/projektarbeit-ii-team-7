#version 460

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 uv;

layout (location = 0) out vec2 frag_uv;

layout (push_constant, std430) uniform constants {
  mat4 transform_matrix;
  mat4 camera_matrix;
} PushConstants;

void main() {
  gl_Position = PushConstants.camera_matrix * PushConstants.transform_matrix * position;
  frag_uv = uv;
}
