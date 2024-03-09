#version 330 core
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;

out vec2 tex_coords;
out vec3 vert_pos;
out vec3 vert_norm;

uniform mat4 matView;
uniform mat4 matProjection;
uniform mat4 matModel;
uniform mat4 mvp;

void main() {
    tex_coords = vertexTexCoord;
    vert_pos = vec3(matModel * vec4(vertexPosition, 1.0));
    vert_norm = transpose(inverse(mat3(matModel))) * vertexNormal;

    gl_Position = mvp * vec4(vertexPosition, 1.0);
}