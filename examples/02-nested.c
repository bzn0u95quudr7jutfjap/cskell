
    typedef struct { int a; int b; } pair;
    typedef struct { int s; int a[12]; } array;
    typedef struct { array a; pair b[12]; } nester;

    pair a = { .a = 0, .b = 12, };
    array b = {.s = 9, .a = {0,1,2,3,4,5,6,7,8}};
    array b = {.s = rand((pair){.a = 0, .b = 9}), .a = {0,1,2,3,4,5,6,7,8}};
    nester b = {
      .a = {
        .s = rand((pair){.a = 0, .b = 12}),
        .a = {},
      },
      .b = {
        {.a = 0, .b = 0},
        {.a = 1, .b = 1},
        {.a = 2, .b = 2},
        {.a = 3, .b = 3},
      },
    };
