#version 410 core

// Output
layout (location = 0) out vec4 colour;
layout (location = 1) out vec4 normal_depth;

// Input from geometry shader

in vec3 N;
in vec3 L;
in vec3 V;
in vec3 vertex_colour;

// Material properties
vec3 specular_albedo = vec3(1.0, 1.0, 1.0);
float specular_power = 8.0;

void main(void)
{
    // Normalize the incoming N, L and V vectors
    vec3 N_ = normalize(N);
    vec3 L_ = normalize(L);
    vec3 V_ = normalize(V);

    // Calculate R locally
    vec3 R_ = reflect(-L_, N_);

    // Compute the diffuse and specular components for each fragment
    vec3 diffuse = max(dot(N_, L_), 0.0) * vertex_colour;
    diffuse *= diffuse;

    vec3 specular = pow(max(dot(R_, V_), 0.0), specular_power) * specular_albedo;

    vec3 ambient = vec3(0, 0, 0);

    // Write final color to the framebuffer
    colour = vec4(diffuse + specular + ambient, 1.0);
    normal_depth = vec4(N_, V.z);
}
