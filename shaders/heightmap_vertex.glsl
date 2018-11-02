#version 410

in vec3 vertex_position;

uniform mat4 mvp;

out vec3 frag_position;

void main() {
  frag_position = vertex_position;
  gl_Position = mvp * vec4(vertex_position, 1.0);
}
