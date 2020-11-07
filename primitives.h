// Source code by Shawn Halayka
// Source code is in the public domain

#ifndef PRIMITIVES_H
#define PRIMITIVES_H



#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
using namespace glm;


#include <cmath>
#include <cstddef> // g++ chokes on size_t without this

class indexed_vertex_3
{
public:

	inline indexed_vertex_3(void) : x(0.0f), y(0.0f), z(0.0f), index(0) { /*default constructor*/ }
	inline indexed_vertex_3(const float src_x, const float src_y, const float src_z, const size_t src_index) : x(src_x), y(src_y), z(src_z), index(src_index) { /* custom constructor */ }
	inline indexed_vertex_3(const float src_x, const float src_y, const float src_z) : x(src_x), y(src_y), z(src_z), index(0) { /* custom constructor */ }
	
	inline const void normalize(void)
	{
		float len = length();

		if (0.0f != len)
		{
			x /= len;
			y /= len;
			z /= len;
		}
	}

	inline const float self_dot(void)
	{
		return x * x + y * y + z * z;
	}

	inline const float length(void)
	{
		return std::sqrtf(self_dot());
	}

	inline const indexed_vertex_3& cross(const indexed_vertex_3& right) const
	{
		static indexed_vertex_3 temp;

		temp.x = y * right.z - z * right.y;
		temp.y = z * right.x - x * right.z;
		temp.z = x * right.y - y * right.x;

		return temp;
	}

	inline const indexed_vertex_3& operator-(const indexed_vertex_3& right) const
	{
		static indexed_vertex_3 temp;

		temp.x = this->x - right.x;
		temp.y = this->y - right.y;
		temp.z = this->z - right.z;

		return temp;
	}
	
	inline const indexed_vertex_3& operator+(const indexed_vertex_3& right) const
	{
		static indexed_vertex_3 temp;

		temp.x = this->x + right.x;
		temp.y = this->y + right.y;
		temp.z = this->z + right.z;

		return temp;
	}

	inline bool operator<(const indexed_vertex_3& right) const
	{
		if (right.x > x)
			return true;
		else if (right.x < x)
			return false;

		if (right.y > y)
			return true;
		else if (right.y < y)
			return false;

		if (right.z > z)
			return true;
		else if (right.z < z)
			return false;

		return false;
	}

	inline void zero(void)
	{
		x = y = z = 0;
	}

	inline void rotate_x(const float& radians)
	{
		float t_y = y;

		y = t_y * cos(radians) + z * sin(radians);
		z = t_y * -sin(radians) + z * cos(radians);
	}

	inline void rotate_y(const float& radians)
	{
		float t_x = x;

		x = t_x * cos(radians) + z * -sin(radians);
		z = t_x * sin(radians) + z * cos(radians);
	}

	inline void rotate_z(const float& radians)
	{
		float t_x = x;

		x = t_x * cos(radians) + y * sin(radians);
		y = t_x * -sin(radians) + y * cos(radians);
	}

	float x, y, z, w;
	size_t index;
};


vec3 zero(void);

vec3 rotate_x(vec3 input, const float& radians);

vec3 rotate_y(vec3 input, const float& radians);



class triangle
{
public:
	indexed_vertex_3 vertex[3];
};





#endif