#include "primitives.h"


vec3 zero(void)
{
	vec3 out;
	out.x = out.y = out.z = 0;

	return out;
}

vec3 rotate_x(const vec3 input, const float& radians)
{
	vec3 out = input;

	out.y = input.y * cos(radians) + input.z * sin(radians);
	out.z = input.y * -sin(radians) + input.z * cos(radians);

	return out;
}

vec3 rotate_y(const vec3 input, const float& radians)
{
	vec3 out = input;

	out.x = input.x * cos(radians) + input.z * -sin(radians);
	out.z = input.x * sin(radians) + input.z * cos(radians);

	return out;
}