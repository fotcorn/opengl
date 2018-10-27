#version 410

in vec3 vertex_position;
in vec2 texture_coordinate;
in vec3 normal;

out vec2 frag_texture_coordinate;
out vec3 frag_normal;
out vec3 frag_position;

uniform mat4 mvp;
uniform mat4 model;

void main() {
  frag_texture_coordinate = texture_coordinate;
  frag_normal = mat3(transpose(inverse(model))) * normal;

  frag_position = vec3(model * vec4(vertex_position, 1.0));
  gl_Position = mvp * vec4(vertex_position, 1.0);
}
