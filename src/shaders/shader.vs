#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_uv;
layout (location = 2) in vec3 a_instancePos;

out vec2 v_uv;

uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    v_uv = a_uv;
    vec3 worldPos = a_instancePos + a_position;
    gl_Position = u_projection * u_view * vec4(worldPos, 1.0);
}
