#version 330 core
layout (location = 0) in vec3 aPos;

struct Wave
{
    vec3 dir;    // direction
    float amp;   // amplitude
    float speed; 
};

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform float time;

out vec3 FragPos;
out vec3 Normal;

float wave(vec3 pos, Wave wave);
float waveDerivative(vec3 pos, Wave wave);

void main()
{
    Wave wave = Wave(vec3(1.0, 0.0, 0.3), 1.0, 0.0);

    vec3 pos = aPos;
    pos.y += wave(pos, wave);
    vec3 tangent = vec3(1.0, wave.dir.x * waveDerivative(pos, wave), 0.0); // tangent on x axis
    vec3 binormal = vec3(0.0, wave.dir.z * waveDerivative(pos, wave), 1.0); // tangent on z axis
    Normal = cross(binormal, tangent); // right hand rule, z cross x, z is towards camera
    gl_Position = proj * view * model * vec4(pos, 1.0);
    FragPos = vec3(model * vec4(pos, 1.0));
}

float wave(vec3 pos, Wave w)
{
    return w.amp * sin(dot(w.dir, pos) + time * w.speed);
}

float waveDerivative(vec3 pos, Wave w)
{
    return w.amp * cos(dot(w.dir, pos) + time * w.speed);
}