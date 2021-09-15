#version 410

in lowp vec4 f_color;
in vec2 f_tex_coords;
in vec3 f_pos;
in vec3 f_normal;

out vec4 out_color;

uniform sampler2D texture_;

void main()
{
    // Combine everything:
    out_color = texture(texture_, f_tex_coords);
}
