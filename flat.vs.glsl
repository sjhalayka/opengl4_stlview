#version 410 core

// Per-vertex inputs
layout (location = 0) in vec3 position;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform vec3 flat_colour;

out VS_OUT
{
    vec3 vertex_colour;
} vs_out;


void main(void)
{
    // Calculate view-space coordinate
    vec4 P = mv_matrix * vec4(position, 1.0);

    // Calculate the clip-space position of each vertex
    gl_Position = proj_matrix * P;

    vs_out.vertex_colour = flat_colour;
}
