#version 410

layout(location = 0) in vec4 v_position;
layout(location = 1) in vec4 v_color;
layout(location = 2) in vec4 v_normal;
layout(location = 3) in vec2 v_tex_coords;

out vec4 f_color;
out vec2 f_tex_coords;
out vec3 f_pos;
out vec3 f_normal;

#define PI 3.1415
#define scaling_factor 2

mat4 generate_frustum() {
    float near = 1.0;
    float far = 12.0;
    float left = -1;
    float right = 1;
    float top = 9.0 / 16.0;
    float bottom = -9.0 / 16.0;

    mat4 frustum = mat4(
        2.0 * near / (right - left),        0.0,                                0.0,                                0.0,
        0.0,                                2.0 * near / (top - bottom),        0.0,                                0.0,
        (right + left) / (right - left),    (top + bottom) / (top - bottom),    -(far + near) / (far - near),       -1.0,
        0.0,                                0.0,                                -2.0 * near * far / (far - near),   0.0
    );

    return frustum;
}

void main()
{
    vec4 trans = vec4(0, 0, -2.0, 0.0);
    mat4 frustum = generate_frustum();

    mat4 rot_x = mat4(
        1, 0, 0, 0,
        0, cos(PI), -sin(PI), 0,
        0, sin(PI), cos(PI), 0,
        0, 0, 0, 1
    );

    mat4 scale = mat4(
        scaling_factor * .5, 0, 0, 0,
        0, scaling_factor, 0, 0,
        0, 0, scaling_factor, 0,
        0, 0, 0, 1
    );

    vec3 pos = ((scale * v_position) + trans).xyz;

    gl_Position = frustum * vec4(pos, 1.0);

    f_color = v_color;
    f_tex_coords = v_tex_coords;
    f_pos = pos;
    f_normal = v_normal.xyz;
}
