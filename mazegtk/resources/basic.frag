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
struct Directions {
    bool can_go_left;
    bool can_go_up;
    bool can_go_right;
    bool can_go_down;
};

MazeCell get_cell_at(uvec2 pos);
Directions get_directions_at(uvec2 pos);
float wall_brightness(Directions d, vec2 in_cell_coords);

out vec4 out_color;
in vec2 f_tex_pos;

uniform vec2 u_screen_size;
uniform vec2 u_camera_pos;
uniform vec2 u_cell_size_pix;

vec4 render_pixel(vec2 in_maze_coords);
vec4 render_corner(vec2 in_maze_coords);
vec4 render_vertical_border(vec2 in_maze_coords);
vec4 render_horizontal_border(vec2 in_maze_coords);
vec4 render_bg(vec2 in_maze_coords);



#define WALL_BRIGHTNESS(x) ((x < (sqrt(2) / length(u_cell_size_pix)) * 0.5) ? 1.0 : 0.0) 
#define WALL_COLOR vec3(1.0, 0.5, 0.0)
#define BG_COLOR vec3(0.0)

#define CLAMP(x, minval, maxval) min((maxval), max((minval), (x))) 
#define COLOR_PICK(amount, color0, color1) (CLAMP(amount, 0.0, 1.0) * color1 + (1.0 - CLAMP(amount, 0.0, 1.0)) * color0)




void main() {
    vec2 correct_tex_pos = vec2(f_tex_pos.x, 1.0 - f_tex_pos.y);
    vec2 pix_pos = correct_tex_pos * u_screen_size;

    vec2 screen_center_pix = vec2(0.5, 0.5) * u_screen_size;

    vec2 wh = vec2(width, height);
    vec2 cell_size_pix = u_cell_size_pix;

    vec2 maze_origin = u_camera_pos;
    vec2 maze_pos_pix = screen_center_pix - maze_origin * cell_size_pix;

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

    Directions d = get_directions_at(uvec2(maze_coords_floor));
    float total_wall = wall_brightness(d, in_cell_coords);

    vec3 cell_color = vec3(0.0);
    float how_close_to_border = length(in_cell_coords - 0.5) / (0.5 * sqrt(2));
    float blend = how_close_to_border * 0.8 + 0.2;
    cell_color = COLOR_PICK(blend, cell_color, vec3(0.125));

    vec2 grid_step = vec2(1024, 1024);
    vec2 in_grid_tmp = mod(in_maze_coords + grid_step / 2.0, grid_step) - grid_step / 2.0;
    in_grid_tmp.x = abs(in_grid_tmp.x) < (1.0 / u_cell_size_pix.x) ? 0.5 : 0.0;
    in_grid_tmp.y = abs(in_grid_tmp.y) < (1.0 / u_cell_size_pix.y) ? 0.5 : 0.0;
    float total_grid = CLAMP(length(in_grid_tmp), 0.0, 1.0);

    // cell_color = vec3(length(in_grid_tmp));
    // vec3 c = WALL_COLOR * total_wall + cell_color * (1.0 - total_wall);
    vec3 c = (WALL_COLOR * total_wall + cell_color * (1.0 - total_wall)) * (1.0 - total_grid) + vec3(1.0) * total_grid;
    return vec4(c, 1.0);
}

// WALL BRIGHTNESS
float wall_brightness(Directions d, vec2 in_cell_coords) {
    float vert_brightness = 0.0;
    if (in_cell_coords.x < 0.5) {
        vert_brightness = max(WALL_BRIGHTNESS(in_cell_coords.x), 0.0) * (!d.can_go_left ? 1.0 : 0.0);
    } else {
        vert_brightness = max(WALL_BRIGHTNESS(1.0 - in_cell_coords.x), 0.0) * (!d.can_go_right ? 1.0 : 0.0);
    }

    float horz_brightness = 0.0;
    if (in_cell_coords.y < 0.5) {
        horz_brightness = max(WALL_BRIGHTNESS(in_cell_coords.y), 0.0) * (!d.can_go_up ? 1.0 : 0.0);
    } else {
        horz_brightness = max(WALL_BRIGHTNESS(1.0 - in_cell_coords.y), 0.0) * (!d.can_go_down ? 1.0 : 0.0);
    }
    float total_wall = min(1.0, vert_brightness + horz_brightness);
    return total_wall;
}


// DIRECTIONS
Directions get_directions_at(uvec2 pos) {
    vec2 maze_coords_floor = pos;

    MazeCell cell = get_cell_at(uvec2(maze_coords_floor));
    MazeCell cell_bottom = get_cell_at(uvec2(maze_coords_floor) + uvec2(0, 1));
    MazeCell cell_right = get_cell_at(uvec2(maze_coords_floor) + uvec2(1, 0));

    Directions d;
    d.can_go_left = !cell.left_wall;
    d.can_go_up = !cell.top_wall;
    d.can_go_right = !cell_right.left_wall;
    d.can_go_down = !cell_bottom.top_wall;

    return d;
}

// READING
#define SIZEOF_UINT 4
MazeCell read_uint_to_cell(uint data, uint rem_bits) {
    uint raw_bits = (data >> rem_bits) & 3;

    MazeCell cell;
    cell.top_wall = (raw_bits & uint(1)) > 0 ? true : false;
    cell.left_wall = (raw_bits & uint(2)) > 0 ? true : false;

    return cell;
}

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

    return read_uint_to_cell(maze_data[pos_index], rem_bits);
}