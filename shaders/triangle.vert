#version 330 core
layout(location = 0) in vec3 position;

uniform mat4 MVP;
uniform mat4 R;
// todo: also apply rotation
void main()
{
    gl_Position = MVP * vec4(position, 1.0);
}