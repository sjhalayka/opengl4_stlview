#pragma once

#include "primitives.h"


#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
using namespace glm;

#include <iostream>
using std::cout;
using std::endl;

#include <fstream>
using std::ifstream;
using std::ofstream;

#include <ios>
using std::ios_base;
using std::ios;

#include <set>
using std::set;

#include <vector>
using std::vector;

#include <limits>
using std::numeric_limits;

#include <cstring> // for memcpy()



void get_vertices_and_normals_from_triangles(vector<triangle> &t, vector<vec3> &fn, vector<vec3> &v, vector<vec3> &vn);
bool read_triangles_from_binary_stereo_lithography_file(vector<triangle> &triangles, const char *const file_name);
void scale_mesh(vector<triangle> &triangles, float max_extent);


