#version 410

in lowp vec4 f_color;
in vec2 f_tex_coords;
in vec3 f_pos;
in vec3 f_normal;

out vec4 out_color;

uniform float alpha;

uniform sampler2D image;

void main()
{
    // Combine everything:
    vec4 color = texture(image, f_tex_coords) + vec4(0, 0, 0, 1);
    color.a = alpha;
    out_color = color;
}
