#version 410

layout(location = 0) in vec4 v_position;
layout(location = 1) in vec4 v_color;
layout(location = 2) in vec4 v_normal;
layout(location = 3) in vec2 v_tex_coords;

out vec4 f_color;
out vec2 f_tex_coords;
out vec3 f_pos;
out vec3 f_normal;

uniform int block_id;
uniform vec2 pos;

flat out int block_id_;

#define PI 3.1415
#define scaling_factor .1

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

// source: https://stackoverflow.com/questions/15095909/from-rgb-to-hsv-in-opengl-glsl
vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
    block_id_ = block_id;
    vec4 trans = vec4(pos, -2.0, 0.0);

    mat4 frustum = generate_frustum();

    mat4 rot_x = mat4(
        1, 0, 0, 0,
        0, cos(3*PI/2), -sin(3*PI/2), 0,
        0, sin(3*PI/2), cos(3*PI/2), 0,
        0, 0, 0, 1
    );

    mat4 scale = mat4(
        scaling_factor, 0, 0, 0,
        0, scaling_factor, 0, 0,
        0, 0, scaling_factor, 0,
        0, 0, 0, 1
    );

    vec3 pos = ((rot_x * scale * v_position) + trans).xyz;

    gl_Position = frustum * vec4(pos, 1.0);

    f_color = vec4(hsv2rgb(vec3(block_id * 2.0 * PI, 1, 1)), 1.0);
    f_tex_coords = v_tex_coords;
    f_pos = pos;
    f_normal = v_normal.xyz;
}
