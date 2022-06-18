#version 460

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 uv;

layout (location = 0) out vec2 v_uv;
layout (location = 1) out vec4 v_color;

layout (push_constant, std430) uniform constants {
  mat4 camera_matrix;
  vec4 pos;
  vec4 color;
  vec2 scale;
  vec2 uvoffset;
} PushConstants;

void main() {
  mat4 trans_mat = mat4(
    PushConstants.scale.x, 0, 0, 0,
    0, PushConstants.scale.y, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
  );

  trans_mat[3] = PushConstants.pos;

  gl_Position = PushConstants.camera_matrix * trans_mat * position;

  v_uv = uv + PushConstants.uvoffset;
  v_color = PushConstants.color;
}
