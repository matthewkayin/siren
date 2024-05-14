#include "geometry.h"

#include <glad/glad.h>
#include <cstddef>

siren::Geometry siren::geometry_create_cube(vec3 extents) {
	float cube_vertices[] = {
		// back face
		-extents.x, -extents.y, -extents.z,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-left
		 extents.x,  extents.y, -extents.z,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, 0.0f, // top-right
		 extents.x, -extents.y, -extents.z,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, 0.0f, // bottom-right
		 extents.x,  extents.y, -extents.z,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, 0.0f, // top-right
		-extents.x, -extents.y, -extents.z,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-left
		-extents.x,  extents.y, -extents.z,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // top-left
		// front face
		-extents.x, -extents.y,  extents.z,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 0.0f, // bottom-left
		 extents.x, -extents.y,  extents.z,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, 0.0f, // bottom-right
		 extents.x,  extents.y,  extents.z,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, 0.0f, // top-right
		 extents.x,  extents.y,  extents.z,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, 0.0f, // top-right
		-extents.x,  extents.y,  extents.z,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, 0.0f, // top-left
		-extents.x, -extents.y,  extents.z,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 0.0f, // bottom-left
		// left face
		-extents.x,  extents.y,  extents.z, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f, // top-right
		-extents.x,  extents.y, -extents.z, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, 0.0f, // top-left
		-extents.x, -extents.y, -extents.z, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 0.0f, // bottom-left
		-extents.x, -extents.y, -extents.z, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 0.0f, // bottom-left
		-extents.x, -extents.y,  extents.z, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f, // bottom-right
		-extents.x,  extents.y,  extents.z, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f, // top-right
		// right face
		 extents.x,  extents.y,  extents.z,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f, // top-left
		 extents.x, -extents.y, -extents.z,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 0.0f, // bottom-right
		 extents.x,  extents.y, -extents.z,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, 0.0f, // top-right
		 extents.x, -extents.y, -extents.z,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 0.0f, // bottom-right
		 extents.x,  extents.y,  extents.z,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.0f, // top-left
		 extents.x, -extents.y,  extents.z,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
		 // bottom face
		 -extents.x, -extents.y, -extents.z,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, 0.0f, // top-right
		  extents.x, -extents.y, -extents.z,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, 0.0f, // top-left
		  extents.x, -extents.y,  extents.z,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
		  extents.x, -extents.y,  extents.z,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
		 -extents.x, -extents.y,  extents.z,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f, // bottom-right
		 -extents.x, -extents.y, -extents.z,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, 0.0f, // top-right
		 // top face
		 -extents.x,  extents.y, -extents.z,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, 0.0f, // top-left
		  extents.x,  extents.y , extents.z,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, 0.0f, // bottom-right
		  extents.x,  extents.y, -extents.z,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, 0.0f, // top-right
		  extents.x,  extents.y,  extents.z,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, 0.0f, // bottom-right
		 -extents.x,  extents.y, -extents.z,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, 0.0f, // top-left
		 -extents.x,  extents.y,  extents.z,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 0.0f  // bottom-left
	};

    Geometry geometry;
    glGenVertexArrays(1, &geometry.vao);
    glBindVertexArray(geometry.vao);
    glGenBuffers(1, &geometry.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), &cube_vertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(vec3), (void*)offsetof(Geometry::VertexData, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(vec3), (void*)offsetof(Geometry::VertexData, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(vec3), (void*)offsetof(Geometry::VertexData, tex_coord));

    geometry.vertex_count = 36;
    geometry.material_albedo = 0;

    return geometry;
}
