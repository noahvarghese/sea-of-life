#include "life.h"

#define STRIDE 4

static void print_grid(bool **grid, int rows, int columns) {
    char *line;

    // +1 for newline, +2 for the extra space and pipe -> "| "
    line = calloc(columns * 4 + 2 + 1, sizeof(char));

    if (line == NULL) {
        error("Unable to initialize memory for line\n");
    }

    for (int i = rows - 1; i >= 0; i--)
    {
        for (int j = 0; j < columns; j++)
        {
            int index = j * STRIDE;
            line[index] = '|';
            line[index + 1] = ' ';
            line[index + 2] = grid[i][j] ? 'X' : 'O';
            line[index + 3] = ' ';

            if (j == (columns - 1)) {
                line[index + 4] = '|';
                line[index + 5] = '\n';
            }
        }

        printf(line);
    }
}

static void print(life_t *self)
{
    print_grid(self->grid, self->rows, self->columns);
}

static void print_shadow(life_t *self) {
    print_grid(self->shadow_grid, self->rows, self->columns);
}

static bool gen_life() {
    float random_value = rand();
    float normalized_value = random_value / (float)RAND_MAX;

    if (normalized_value > 0.5f)
        return true;
    else
        return false;
}

static void swap(life_t *self) {
    bool **tmp_grid;
    tmp_grid = self->grid;
    self->grid = self->shadow_grid;
    self->shadow_grid = tmp_grid;

    int_tuple_list_t *tmp_last_changed;
    tmp_last_changed = self->last_changed;
    self->last_changed = self->shadow_last_changed;
    self->shadow_last_changed = tmp_last_changed;
}

static void seed(life_t *self) {
    self->last_changed->clear(self->last_changed);

    for (int i = 0; i < self->rows; i++)
    {
        for (int j = 0; j < self->columns; j++) {
            self->grid[i][j] = gen_life();
            self->last_changed->push(self->last_changed, (int[2]){i, j});
        }
    }
}

static bool get_alive(life_t *self, int x, int y) {
    return self->grid[x][y];
}

static void set_alive(life_t *self, int x, int y, bool alive) {
    self->shadow_grid[x][y] = alive;
    self->shadow_last_changed->push(self->shadow_last_changed, (int[2]){x,y});
}

static void init_grid(bool ***grid, int rows, int columns) {
    (*grid) = (bool **)calloc(rows, sizeof(bool *));

    for (int i = 0; i < rows; i++) {
        (*grid)[i] = (bool *)calloc(columns, sizeof(bool));
    }
}

static int get_num_alive_neighbors(life_t *self, int x, int y) {
    bool **grid = self->grid;

    int max_rows = self->rows - 1;
    int max_columns = self->columns - 1;

    int alive_neighbors = 0;

    for (int x_offset = -1; x_offset < 2; x_offset++)
    {
        int row = x + x_offset;
        
        if (row < 0 || row > max_rows)
            continue;

        for (int y_offset = -1; y_offset < 2; y_offset++)
        {
            int column = y + y_offset;

            // If the cell we are checking is the cell we want to get the neighbors for we need to skip it
            if (x_offset == 0 && y_offset == 0)
                continue;
            if (column < 0 || column > max_columns)
                continue;
            if (grid[row][column]) 
                alive_neighbors++;
        }
    }

    return alive_neighbors;
}

// static void live(life_t *self) {
//     self->shadow_last_changed->clear(self->shadow_last_changed);

//     for (int i = 0; i < self->rows; i++)
//     {
//         for (int j = 0; j < self->columns; j++) {
//             bool is_alive = self->get_alive(self, i, j);
//             int alive_neighbors = get_num_alive_neighbors(self, i, j);

//             if (is_alive && (alive_neighbors < 2 || alive_neighbors > 3)) {
//                 self->set_alive(self, i, j, false);
//                 continue;
//             } else if (alive_neighbors == 3) {
//                 self->set_alive(self, i, j, true);
//                 continue;
//             }

//             self->set_alive(self, i, j, is_alive);
//         }
//     }

//     self->swap(self);
// }

static void life_delegate(int coordinate[2], int index, void *context)
{
    life_t *self = (life_t *)context;
    bool is_alive = self->get_alive(self, coordinate[0], coordinate[1]);
    int alive_neighbors = get_num_alive_neighbors(context, coordinate[0], coordinate[1]);
    bool should_live = is_alive;

    if (is_alive && (alive_neighbors < 2 || alive_neighbors > 3))
        should_live = false;
    else if (alive_neighbors == 3)
        should_live = true;

    bool changed = is_alive == should_live;

    if (changed)
    {
        self->shadow_last_changed->push(self->shadow_last_changed, coordinate);
        self->set_alive(self, coordinate[0], coordinate[1], should_live);
    }
}

static void live2(life_t *self)
{
    self->shadow_last_changed->clear(self->shadow_last_changed);

    self->last_changed->for_each(self->last_changed, life_delegate, self);

    self->swap(self);
}

life_t *init_life(int rows, int columns) {
    srand(time(0));
    life_t *self;

    self = (life_t *)calloc(1, sizeof(life_t));
    if (self == NULL) {
        error("Unable to allocate memory for life.");
    }

    self->rows = rows;
    self->columns = columns;

    self->last_changed = init_int_tuple_list(rows * columns, NULL);
    self->shadow_last_changed = init_int_tuple_list(rows * columns, NULL);

    init_grid(&self->grid, self->rows, self->columns);
    init_grid(&self->shadow_grid, self->rows, self->columns);

    self->print = print;
    self->print_shadow = print_shadow;
    self->seed = seed;
    self->swap = swap;
    self->set_alive = set_alive;
    self->get_alive = get_alive;
    self->live = live2;
    // self->live = live;

    return self;
}

void destroy_life(life_t *self) {
    destroy_int_tuple_list(self->last_changed);
    destroy_int_tuple_list(self->shadow_last_changed);
    for (int i = 0; i < self->rows; i++)
    {
        free(self->grid[i]);
    }
    free(self->grid);
    free(self);
}