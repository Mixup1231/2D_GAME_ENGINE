#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_uvs;

out vec2 uvs;

uniform mat4 projection;
uniform mat4 model;
uniform vec4 uvs_shift;

void main() {
	if (a_uvs.x == 1)
		uvs.x = uvs_shift.z;
	else
		uvs.x = uvs_shift.x;
	
	if (a_uvs.y == 1)
		uvs.y = uvs_shift.w;
	else
		uvs.y = uvs_shift.y;

	gl_Position = projection * model * vec4(a_pos, 1.0);
}