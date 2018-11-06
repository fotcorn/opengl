#version 410

in vec3 frag_position;
out vec4 fragmentColor;

void main() {
  float color = 0.3 + frag_position.y / 32;
  fragmentColor = vec4(color, color, color, 1.0);
}
