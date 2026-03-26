#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_uv;

out vec2 v_uv;

uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    v_uv = a_uv;
    gl_Position = u_projection * u_view * vec4(a_position, 1.0);
}
