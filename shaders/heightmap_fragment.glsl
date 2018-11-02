#version 410

in vec3 frag_position;
out vec4 fragmentColor;

void main() {
  fragmentColor = vec4(0.5 + frag_position.y / 32, 0.0, 0.0, 1.0);
}
