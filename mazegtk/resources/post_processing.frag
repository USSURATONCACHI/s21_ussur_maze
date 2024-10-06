#version 330 core

out vec4 out_color;
in vec2  f_tex_pos;      // Fragment position in world coordinates

uniform sampler2D u_read_texture;

void main() {
    out_color = texture(u_read_texture, f_tex_pos);
}