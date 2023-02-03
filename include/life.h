#ifndef LIFE_H

#define LIFE_H

#include "utils/std_utils.h"
#include "utils/string_utils.h"
#include <stdbool.h>
#include <time.h>

typedef struct life_t
{
    int rows;
    int columns;
    bool **grid;
    bool **shadow_grid;
    void (*print)(struct life_t *self);
    void (*print_shadow)(struct life_t *self);
    void (*seed)(struct life_t *self);
    void (*swap)(struct life_t *self);
    void (*live)(struct life_t *self);
    void (*set_alive)(struct life_t *self, int x, int y, bool alive);
    bool (*get_alive)(struct life_t *self, int x, int y);
    int (*get_num_alive_neighbors)(struct life_t *self, int x, int y);
} life_t;

life_t *init_life(int rows, int columns);
void destroy_life(life_t *self);

#endif