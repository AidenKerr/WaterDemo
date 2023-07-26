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

float sinWave(vec3 pos, Wave wave);
float sinWaveDerivative(vec3 pos, Wave wave);
float wave(vec3 pos, Wave wave);
float waveDerivative(vec3 pos, Wave wave);

Wave waves[] = Wave[](Wave(normalize(vec3(1.0, 0.0, 0.3)), 0.5, 1.0),
                      Wave(normalize(vec3(0.3, 0.0, -0.9)), 0.4, 2.0),
                      Wave(normalize(vec3(-0.9, 0.0, 0.2)), 0.3, 3.0),
                      Wave(normalize(vec3(-0.5, 0.0, -0.8)), 0.2, 4.0));
const int wavesCount = waves.length();

void main()
{
    vec3 pos = aPos;

    Normal = vec3(0.0);
    for (int i = 0; i < wavesCount; i++)
    {
        Wave w = waves[i];
        pos.y += wave(pos, w);
        vec3 tangent = vec3(1.0, w.dir.x * waveDerivative(pos, w), 0.0); // tangent on x axis
        vec3 binormal = vec3(0.0, w.dir.z * waveDerivative(pos, w), 1.0); // tangent on z axis
        Normal += cross(binormal, tangent); // right hand rule, z cross x, z is towards camera
    }
    gl_Position = proj * view * model * vec4(pos, 1.0);
    FragPos = vec3(model * vec4(pos, 1.0));
}

float sinWave(vec3 pos, Wave w)
{
    return w.amp * sin(dot(w.dir, pos) + time * w.speed);
}

float sinWaveDerivative(vec3 pos, Wave w)
{
    return w.amp * cos(dot(w.dir, pos) + time * w.speed);
}

float wave(vec3 pos, Wave w)
{
    return w.amp * exp(sin(dot(w.dir, pos) + time * w.speed));
}

float waveDerivative(vec3 pos, Wave w)
{
    return w.amp * exp(sin(dot(w.dir, pos) + time * w.speed)) * cos(dot(w.dir, pos) + time * w.speed);
}