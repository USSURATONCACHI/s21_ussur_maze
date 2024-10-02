#version 330 core

out vec4 out_color;
in vec2 f_tex_pos;

void main() {
    out_color = vec4(f_tex_pos, 0.0, 1.0);
}