#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{ 
    float near = 0.1;
    float far = 2000.0;
    float depth = texture(screenTexture, TexCoords).r;
    float distance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
    FragColor = vec4(distance / 50.0);
}
