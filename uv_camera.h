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



public:
	void Set_Large_Screenshot(size_t num_cams, size_t cam_index_x, size_t cam_index_y)
	{
		// No guarantees about the behaviour of this functionality. It wasn't tested a lot.

		//glMatrixMode(GL_PROJECTION);
		//glLoadIdentity();

		// Image plane reference:
		// http://www.songho.ca/opengl/gl_transform.html
		const float deg_to_rad = (1.0f / 360.0f) * 2.0f * glm::pi<float>();
		float aspect = float(win_x) / float(win_y);
		float tangent = tanf((fov / 2.0f) * deg_to_rad);
		float height = near_plane * tangent; // Half height of near_plane plane.
		float width = height * aspect; // Half width of near_plane plane.

		float cam_width = 2 * width / num_cams;
		float cam_height = 2 * height / num_cams;

		float left = -width + cam_index_x * cam_width;
		float right = -width + (cam_index_x + 1) * cam_width;
		float bottom = -height + cam_index_y * cam_height;
		float top = -height + (cam_index_y + 1) * cam_height;


		model_mat = glm::mat4(1.0f);

		projection_mat = frustum(left, right, bottom, top, near_plane, far_plane);

		view_mat = lookAt(
			eye,
			look_at,
			up
		);


		//mat4x4 f = 
		//// Instead of gluPerspective...
		//glFrustum(left, right, bottom, top, near_plane, far_plane);

		//gluLookAt(
		//	eye.x, eye.y, eye.z, // Eye position.
		//	eye.x + look_at.x, eye.y + look_at.y, eye.z + look_at.z, // Look at position (not direction).
		//	up.x, up.y, up.z); // Up direction vector.
	}




};


#endif
