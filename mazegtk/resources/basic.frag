#version 430 core

layout(std430, binding = 0) buffer MazeBuffer {
    uint maze_data[];
};
layout(std430, binding = 1) buffer MazeBufferSize {
    uint width;
    uint height;
};


struct MazeCell {
    bool top_wall;
    bool left_wall;
};

MazeCell get_cell_at(uvec2 pos);

out vec4 out_color;
in vec2 f_tex_pos;

uniform vec2 u_screen_size;

vec4 render_pixel(vec2 in_maze_coords);
vec4 render_corner(vec2 in_maze_coords);
vec4 render_vertical_border(vec2 in_maze_coords);
vec4 render_horizontal_border(vec2 in_maze_coords);
vec4 render_bg(vec2 in_maze_coords);

void main() {
    vec2 correct_tex_pos = vec2(f_tex_pos.x, 1.0 - f_tex_pos.y);
    vec2 pix_pos = correct_tex_pos * u_screen_size;


    vec2 maze_pos_pix = vec2(50);
    vec2 cell_size_pix = vec2(70);

    vec2 wh = vec2(width, height);


    vec2 maze_coords = (pix_pos - maze_pos_pix) / cell_size_pix;

    bool x_in_range = maze_coords.x > 0.0 && maze_coords.x < wh.x;
    bool y_in_range = maze_coords.y > 0.0 && maze_coords.y < wh.y;

    bool x_near = maze_coords.x > (-0.5) && maze_coords.x < (wh.x + 0.5);
    bool y_near = maze_coords.y > (-0.5) && maze_coords.y < (wh.y + 0.5);

    if (x_in_range && y_in_range)
        out_color = render_pixel(maze_coords);
    else if (x_near && y_in_range)
        out_color = render_vertical_border(maze_coords);
    else if (y_near && x_in_range)
        out_color = render_horizontal_border(maze_coords);
    else if (x_near && y_near)
        out_color = render_corner(maze_coords);
    else
        out_color = render_bg(maze_coords);
}

#define WALL_CUTOFF 0.5
#define WALL_EXP 256         // More = more sharp wall
#define WALL_EXP_OFFSET 0.03 // More = wider wall
#define WALL_BRIGHTNESS(coords) (1.0 / (1.0 + exp(WALL_EXP * (abs((coords)) - WALL_EXP_OFFSET))) - WALL_CUTOFF) / (1.0 - WALL_CUTOFF)
#define WALL_COLOR vec3(1.0, 0.5, 0.0)
#define BG_COLOR vec3(0.0)

#define CLAMP(x, minval, maxval) min((maxval), max((minval), (x))) 
#define COLOR_PICK(amount, color0, color1) (CLAMP(amount, 0.0, 1.0) * color1 + (1.0 - CLAMP(amount, 0.0, 1.0)) * color0)

vec4 render_corner(vec2 in_maze_coords) {
    vec2 maze_coords_floor = floor(in_maze_coords);
    vec2 in_cell_coords = in_maze_coords - maze_coords_floor;
    in_cell_coords = 0.5 - abs(in_cell_coords - 0.5);
    
    float brightess = WALL_BRIGHTNESS(length(in_cell_coords.xy));
    vec3 color = COLOR_PICK(brightess, BG_COLOR, WALL_COLOR);
    return vec4(color, 1.0);
}

vec4 render_vertical_border(vec2 in_maze_coords) {
    vec2 maze_coords_floor = floor(in_maze_coords);
    vec2 in_cell_coords = in_maze_coords - maze_coords_floor;
    in_cell_coords = 0.5 - abs(in_cell_coords - 0.5);

    float vert_brightness = WALL_BRIGHTNESS(in_cell_coords.x);
    vec3 color = COLOR_PICK(vert_brightness, BG_COLOR, WALL_COLOR);
    return vec4(color, 1.0);
}

vec4 render_horizontal_border(vec2 in_maze_coords) {
    vec2 maze_coords_floor = floor(in_maze_coords);
    vec2 in_cell_coords = in_maze_coords - maze_coords_floor;
    in_cell_coords = 0.5 - abs(in_cell_coords - 0.5);

    float horz_brightess = WALL_BRIGHTNESS(in_cell_coords.y);

    vec3 color = COLOR_PICK(horz_brightess, BG_COLOR, WALL_COLOR);
    return vec4(color, 1.0);
}
vec4 render_bg(vec2 in_maze_coords) {
    return vec4(BG_COLOR, 1.0);
}

vec4 render_pixel(vec2 in_maze_coords) {
    vec2 maze_coords_floor = floor(in_maze_coords);
    vec2 in_cell_coords = in_maze_coords - maze_coords_floor;

    MazeCell cell = get_cell_at(uvec2(maze_coords_floor));
    MazeCell cell_bottom = get_cell_at(uvec2(maze_coords_floor) + uvec2(0, 1));
    MazeCell cell_right = get_cell_at(uvec2(maze_coords_floor) + uvec2(1, 0));

    bool can_go_left = !cell.left_wall;
    bool can_go_up = !cell.top_wall;
    bool can_go_right = !cell_right.left_wall;
    bool can_go_down = !cell_bottom.top_wall;


    // vec2 wall_coords = 0.5 - abs(in_cell_coords - 0.5);
    float vert_brightness = 0.0;
    if (in_cell_coords.x < 0.5) {
        vert_brightness = max(WALL_BRIGHTNESS(in_cell_coords.x), 0.0) * (!can_go_left ? 1.0 : 0.0);
    } else {
        vert_brightness = max(WALL_BRIGHTNESS(1.0 - in_cell_coords.x), 0.0) * (!can_go_right ? 1.0 : 0.0);
    }

    float horz_brightness = 0.0;
    if (in_cell_coords.y < 0.5) {
        horz_brightness = max(WALL_BRIGHTNESS(in_cell_coords.y), 0.0) * (!can_go_up ? 1.0 : 0.0);
    } else {
        horz_brightness = max(WALL_BRIGHTNESS(1.0 - in_cell_coords.y), 0.0) * (!can_go_down ? 1.0 : 0.0);
    }
    float total_wall = min(1.0, vert_brightness + horz_brightness);

    vec3 cell_color = vec3(0.0);
    float how_close_to_border = length(in_cell_coords - 0.5) / (0.5 * sqrt(2));
    float blend = how_close_to_border * 0.8 + 0.2;
    cell_color = COLOR_PICK(blend, cell_color, vec3(0.125));

    vec4 out_color = vec4(WALL_COLOR * total_wall + cell_color * (1.0 - total_wall), 1.0);
    return out_color;
}

#define SIZEOF_UINT 4
MazeCell get_cell_at(uvec2 pos) {
    if (pos.x < 0 || pos.y < 0 || pos.x >= width || pos.y >= height) {
        MazeCell ret;
        ret.top_wall = true;
        ret.left_wall = true;
        return ret;
    }

    uint pos_bits = (pos.y * width + pos.x) * 2;
    uint pos_index = pos_bits / (SIZEOF_UINT * 8);
    uint rem_bits = pos_bits % (SIZEOF_UINT * 8);

    uint raw_bits = (maze_data[pos_index] >> rem_bits) & 3;

    MazeCell cell;
    cell.top_wall = (raw_bits & uint(1)) > 0 ? true : false;
    cell.left_wall = (raw_bits & uint(2)) > 0 ? true : false;

    return cell;
}