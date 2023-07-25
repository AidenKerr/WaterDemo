#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform float time;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

void main()
{
    vec3 pos = aPos;
    pos.y += sin(pos.x + time);
    /*vec3 tangent = vec3(1.0, 0, cos(pos.x + time)); // tangent on x axis
    vec3 binormal = vec3(0.0, 1.0, 0.0); // tangent on z axis*/
    //vec3 norm = cross(tangent, binormal);
    gl_Position = proj * view * model * vec4(pos, 1.0);
    FragPos = vec3(model * vec4(pos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    
}