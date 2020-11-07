#include "mesh.h"

void get_vertices_and_normals_from_triangles(vector<triangle> &t, vector<vec3> &fn, vector<vec3> &v, vector<vec3> &vn)
{
	fn.clear();
	v.clear();
	vn.clear();
	
	if(0 == t.size())
		return;

	cout << "Triangles: " << t.size() << endl;
	
    cout << "Welding vertices" << endl;
 
    // Insert unique vertices into set.
    set<indexed_vertex_3> vertex_set;
 
    for(vector<triangle>::const_iterator i = t.begin(); i != t.end(); i++)
    {
        vertex_set.insert(i->vertex[0]);
        vertex_set.insert(i->vertex[1]);
        vertex_set.insert(i->vertex[2]);
    }
 
    cout << "Vertices: " << vertex_set.size() << endl;

    cout << "Generating vertex indices" << endl;
 
	vector<indexed_vertex_3> vv;

    // Add indices to the vertices.
    for(set<indexed_vertex_3>::const_iterator i = vertex_set.begin(); i != vertex_set.end(); i++)
    {
        size_t index = vv.size();
        vv.push_back(*i);
        vv[index].index = index;
    }

	for (size_t i = 0; i < vv.size(); i++)
	{
		vec3 vv_element(vv[i].x, vv[i].y, vv[i].z);
		v.push_back(vv_element);
	}
 
    vertex_set.clear();

	// Re-insert modifies vertices into set.
    for(vector<indexed_vertex_3>::const_iterator i = vv.begin(); i != vv.end(); i++)
        vertex_set.insert(*i);
 
    cout << "Assigning vertex indices to triangles" << endl;
   
    // Find the three vertices for each triangle, by index.
    set<indexed_vertex_3>::iterator find_iter;
 
    for(vector<triangle>::iterator i = t.begin(); i != t.end(); i++)
    {
        find_iter = vertex_set.find(i->vertex[0]);
        i->vertex[0].index = find_iter->index;
 
        find_iter = vertex_set.find(i->vertex[1]);
        i->vertex[1].index = find_iter->index;
 
        find_iter = vertex_set.find(i->vertex[2]);
        i->vertex[2].index = find_iter->index;
    }

	vertex_set.clear();

	cout << "Calculating normals" << endl;
	fn.resize(t.size());
	vn.resize(v.size());

	for(size_t i = 0; i < t.size(); i++)
	{
		vec3 v0;// = t[i].vertex[1] - t[i].vertex[0];
		v0.x = t[i].vertex[1].x - t[i].vertex[0].x;
		v0.y = t[i].vertex[1].y - t[i].vertex[0].y;
		v0.z = t[i].vertex[1].z - t[i].vertex[0].z;

		vec3 v1;// = t[i].vertex[2] - t[i].vertex[0];
		v1.x = t[i].vertex[2].x - t[i].vertex[0].x;
		v1.y = t[i].vertex[2].y - t[i].vertex[0].y;
		v1.z = t[i].vertex[2].z - t[i].vertex[0].z;

		fn[i] = cross(v0, v1);
		fn[i] = normalize(fn[i]);

		vn[t[i].vertex[0].index] = vn[t[i].vertex[0].index] + fn[i];
		vn[t[i].vertex[1].index] = vn[t[i].vertex[1].index] + fn[i];
		vn[t[i].vertex[2].index] = vn[t[i].vertex[2].index] + fn[i];
	}

	for (size_t i = 0; i < vn.size(); i++)
		vn[i] = normalize(vn[i]);
}


bool read_triangles_from_binary_stereo_lithography_file(vector<triangle> &triangles, const char *const file_name)
{
	triangles.clear();
	
    // Write to file.
    ifstream in(file_name, ios_base::binary);
 
    if(in.fail())
        return false;

	const size_t header_size = 80;
	vector<char> buffer(header_size, 0);
	unsigned int num_triangles = 0; // Must be 4-byte unsigned int.
	indexed_vertex_3 normal;

	// Read header.
	in.read(reinterpret_cast<char *>(&(buffer[0])), header_size);
	
	if(header_size != in.gcount())
		return false;

	// Read number of triangles.
	in.read(reinterpret_cast<char *>(&num_triangles), sizeof(unsigned int));
	
	if(sizeof(unsigned int) != in.gcount())
		return false;

	triangles.resize(num_triangles);

	// Enough bytes for twelve 4-byte floats plus one 2-byte integer, per triangle.
	const size_t data_size = (12*sizeof(float) + sizeof(short unsigned int)) * num_triangles;
	buffer.resize(data_size, 0);

	in.read(reinterpret_cast<char *>(&buffer[0]), data_size);

	if(data_size != in.gcount())
		return false;

	// Use a pointer to assist with the copying.
	// Should probably use std::copy() instead, but memcpy() does the trick, so whatever...
	char *cp = &buffer[0];

    for(vector<triangle>::iterator i = triangles.begin(); i != triangles.end(); i++)
    {
		// Skip face normal.
		cp += 3*sizeof(float);
		
		// Get vertices.
		memcpy(&i->vertex[0].x, cp, sizeof(float)); cp += sizeof(float);
		memcpy(&i->vertex[0].y, cp, sizeof(float)); cp += sizeof(float);
		memcpy(&i->vertex[0].z, cp, sizeof(float)); cp += sizeof(float);
		memcpy(&i->vertex[1].x, cp, sizeof(float)); cp += sizeof(float);
		memcpy(&i->vertex[1].y, cp, sizeof(float)); cp += sizeof(float);
		memcpy(&i->vertex[1].z, cp, sizeof(float)); cp += sizeof(float);
		memcpy(&i->vertex[2].x, cp, sizeof(float)); cp += sizeof(float);
		memcpy(&i->vertex[2].y, cp, sizeof(float)); cp += sizeof(float);
		memcpy(&i->vertex[2].z, cp, sizeof(float)); cp += sizeof(float);

		// Skip attribute.
		cp += sizeof(short unsigned int);
    }

	in.close();
 
    return true;
} 

void scale_mesh(vector<triangle> &triangles, float max_extent)
{
	float curr_x_min = numeric_limits<float>::max();
	float curr_y_min = numeric_limits<float>::max();
	float curr_z_min = numeric_limits<float>::max();
	float curr_x_max = numeric_limits<float>::min();
	float curr_y_max = numeric_limits<float>::min();
	float curr_z_max = numeric_limits<float>::min();
	
	for(size_t i = 0; i < triangles.size(); i++)
	{
		for(size_t j = 0; j < 3; j++)
		{
			if(triangles[i].vertex[j].x < curr_x_min)
				curr_x_min = triangles[i].vertex[j].x;
				
			if(triangles[i].vertex[j].x > curr_x_max)
				curr_x_max = triangles[i].vertex[j].x;

			if(triangles[i].vertex[j].y < curr_y_min)
				curr_y_min = triangles[i].vertex[j].y;
				
			if(triangles[i].vertex[j].y > curr_y_max)
				curr_y_max = triangles[i].vertex[j].y;

			if(triangles[i].vertex[j].z < curr_z_min)
				curr_z_min = triangles[i].vertex[j].z;
				
			if(triangles[i].vertex[j].z > curr_z_max)
				curr_z_max = triangles[i].vertex[j].z;
		}			
	}
	
	float curr_x_extent = fabsf(curr_x_min - curr_x_max);
	float curr_y_extent = fabsf(curr_y_min - curr_y_max);
	float curr_z_extent = fabsf(curr_z_min - curr_z_max);

	float curr_max_extent = curr_x_extent;
	
	if(curr_y_extent > curr_max_extent)
		curr_max_extent = curr_y_extent;
		
	if(curr_z_extent > curr_max_extent)
		curr_max_extent = curr_z_extent;
	
	float scale_value = max_extent / curr_max_extent;
	
	cout << "Original max extent: " << curr_max_extent << endl;
	cout << "Scaling all vertices by a factor of: " << scale_value << endl;
	cout << "New max extent: " << max_extent << endl;

	for(size_t i = 0; i < triangles.size(); i++)
	{
		for(size_t j = 0; j < 3; j++)
		{
			triangles[i].vertex[j].x *= scale_value;
			triangles[i].vertex[j].y *= scale_value;
			triangles[i].vertex[j].z *= scale_value;
		}			
	}
}
