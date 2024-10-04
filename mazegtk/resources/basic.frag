#version 430 core

layout(std430, binding = 0) buffer MazeBuffer {
    uint maze_data[];
};
layout(std430, binding = 1) buffer MazeBufferSize {
    uint width;
    uint height;
};

out vec4 out_color;
in vec2 f_tex_pos;

void main() {
    out_color = vec4(f_tex_pos, 1.0, 1.0);
}

#define SIZEOF_UINT 4


struct MazeCell {
    bool top_wall;
    bool left_wall;
};

MazeCell get_cell_at(uint x, uint y);

MazeCell get_cell_at(uint x, uint y) {
    uint pos_bits = (y * width + x) * 2;
    uint pos_index = pos_bits / (SIZEOF_UINT * 8);
    uint rem_bits = pos_bits % (SIZEOF_UINT * 8);

    uint raw_bits = (maze_data[pos_index] >> rem_bits) & 3;

    MazeCell cell;
    cell.top_wall = (raw_bits & uint(1)) > 0 ? true : false;
    cell.left_wall = (raw_bits & uint(2)) > 0 ? true : false;

    return cell;
}