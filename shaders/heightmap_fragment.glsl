#version 410

in vec3 frag_position;
out vec4 fragmentColor;

void main() {
  fragmentColor = vec4(frag_position.y / 256, 0.0, 0.0, 1.0);
}
