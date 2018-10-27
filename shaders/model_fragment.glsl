#version 410

in vec2 frag_texture_coordinate;
in vec3 frag_normal;
in vec3 frag_position;

out vec4 fragmentColor;

uniform sampler2D model_texture;
uniform vec3 light_position;

void main() {
  vec3 diffuseColor = vec3(6.0, 6.0, 6.0);
  vec3 ambientColor = vec3(0.3, 0.3, 0.3);

  vec3 normal = normalize(frag_normal);
  vec3 lightDirection = normalize(light_position - frag_position);
  float diff = max(dot(normal, lightDirection), 0.0);
  vec3 diffuse = diff * diffuseColor;

  vec3 result = (diffuse + ambientColor) * texture(model_texture, frag_texture_coordinate).rgb;
  fragmentColor = vec4(result, 1.0);
}
