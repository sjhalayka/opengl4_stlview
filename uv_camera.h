#ifndef uv_camera_h
#define uv_camera_h

#include <cstdlib>


#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;


#include "primitives.h"




// UV camera
//
// latitude:     | longitude:    | radius:       |
//       *_*_    |        ___    |        ___    |
//      */   \   |       /   \   |       /   \   |
// u:  *|  x  |  |  v:  |**x**|  |  w:  |  x**|  |
//      *\___/   |       \___/   |       \___/   |
//       * *     |               |               |
// 




const float uv_pi = 4.0f * atan(1.0f);
const float uv_pi_half = 0.5f * uv_pi;
const float uv_pi_2 = 2.0f * uv_pi;
const float uv_epsilon = 1e-6f;


class uv_camera
{
public:
	// Use as read-only
	float u, v, w, fov;
	int win_x, win_y;
	vec3 eye, look_at, up;
	float near_plane;
	float far_plane;

	mat4 model_mat;
	mat4 view_mat;
	mat4 projection_mat;

public:
	uv_camera(void);
	void calculate_camera_matrices(const int width_px, const int height_px);

protected:
	void transform(void);
	void reset(void);
	void rotate(void);
	void translate(void);
};


#endif
