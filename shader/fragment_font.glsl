#version 410

in lowp vec4 f_color;
in vec2 f_tex_coords;
in vec3 f_pos;
in vec3 f_normal;

out vec4 out_color;

uniform sampler2D digit;

void main()
{
    if (texture(digit, f_tex_coords).xyz == vec3(0, 0, 0)) discard;

    // Combine everything:
    out_color = texture(digit, f_tex_coords);
}
