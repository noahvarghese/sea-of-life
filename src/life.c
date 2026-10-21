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

static void seed_grid(bool **grid, int rows, int columns) {
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++) {
            grid[i][j] = gen_life();
        }
    }
}

static void swap(life_t *self) {
    bool **tmp;
    tmp = self->grid;
    self->grid = self->shadow_grid;
    self->shadow_grid = tmp;
}

static void seed(life_t *self) {
    seed_grid(self->grid, self->rows, self->columns);
}

static bool get_alive(life_t *self, int x, int y) {
    return self->grid[x][y];
}

static void set_alive(life_t *self, int x, int y, bool alive) {
    self->shadow_grid[x][y] = alive;
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

static void cell_life(life_t *life, int x, int y) {
    bool is_alive = life->get_alive(life, x, y);
    int alive_neighbors = get_num_alive_neighbors(life, x, y);

    if (is_alive && (alive_neighbors < 2 || alive_neighbors > 3)) {
        life->set_alive(life, x, y, false);
    } else if (alive_neighbors == 3) {
        life->set_alive(life, x, y, true);
    } else {
        life->set_alive(life, x, y, is_alive);
    }
}

typedef struct args_t {
    life_t *life;
    int starting_row;
    int ending_row;
    int starting_column;
    int ending_column;
} args_t;

static void *live_separate(void *args)
{
    struct args_t *life_args = args;
    printf("%p\n", &args);

    pthread_t self = pthread_self();
    printf("Running thread [%ld]\n", self);
    printf("\tstarting_col: %d, ending_col: %d, starting_row: %d, ending_row: %d\n", life_args->starting_column, life_args->ending_column, life_args->starting_row, life_args->ending_row);
    printf("\n");

    for (int i = life_args->starting_row; i < life_args->ending_row; i++) {
        for (int j = life_args->starting_column; i < life_args->ending_column; i++) {
            cell_life(life_args->life, i, j);
        }
    }

    return NULL;
}

static void live_concurrently(life_t *self) {
    int assigned_rows = 0;

    while (assigned_rows < self->rows) {
        args_t *args;
        args = calloc(1, sizeof(args_t));
        if (args == NULL) {
            error("Unable to allocate args");
        }

        args->starting_row = assigned_rows;
        assigned_rows += 100;

        args->ending_column = assigned_rows > self->rows ? self->rows : assigned_rows;
        args->starting_column = 0;
        args->ending_column = self->columns;
        args->life = self;

        self->__thread_pool__->do_work(self->__thread_pool__, live_separate, (void *)args);
    }

    self->__thread_pool__->wait(self->__thread_pool__);
    self->swap(self);
}

static void live(life_t *self) {
    if (self->__thread_pool__ == NULL) {
        for (int i = 0; i < self->rows; i++) {
            for (int j = 0; j < self->columns; j++) {
                cell_life(self, i, j);
            }
        }

        self->swap(self);
        return;
    }

    live_concurrently(self);
}

life_t *init_life(int rows, int columns, thread_pool_t *thread_pool) {
    srand(time(0));
    life_t *self;

    self = (life_t *)calloc(1, sizeof(life_t));
    if (self == NULL) {
        error("Unable to allocate memory for life.");
    }

    self->rows = rows;
    self->columns = columns;

    init_grid(&self->grid, self->rows, self->columns);
    init_grid(&self->shadow_grid, self->rows, self->columns);

    self->print = print;
    self->print_shadow = print_shadow;
    self->seed = seed;
    self->swap = swap;
    self->set_alive = set_alive;
    self->get_alive = get_alive;
    self->live = live;
    self->__thread_pool__ = thread_pool;

    return self;
}

void destroy_life(life_t *self) {
    for (int i = 0; i < self->rows; i++) {
        free(self->grid[i]);
    }
    free(self->grid);
    free(self);
}