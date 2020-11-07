#version 430 core

layout (triangles) in;
layout (triangle_strip) out;
layout (max_vertices = 3) out;


in VS_OUT {
    vec3 N;
    vec3 L;
    vec3 V;
    vec3 vertex_colour;
} gs_in[]; 

out vec3 N;
out vec3 L;
out vec3 V;
out vec3 vertex_colour;

void main(void)
{
    for (int i = 0; i < gl_in.length(); i++)
    {
        gl_Position = gl_in[i].gl_Position;

        N = gs_in[i].N;
        L = gs_in[i].L;
        V = gs_in[i].V;
        vertex_colour = gs_in[i].vertex_colour;

        EmitVertex();
    }

    EndPrimitive();
}  