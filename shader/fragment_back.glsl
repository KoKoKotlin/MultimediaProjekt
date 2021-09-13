#version 410
precision mediump float;

in lowp vec4 f_color;
in vec2 f_tex_coords;
in vec3 f_pos;
in vec3 f_normal;

out vec4 out_color;

uniform sampler2D background;

void main()
{
    // Combine everything:
    out_color = texture(background, f_tex_coords); // vec4(1, 0, 0, 1);
}
