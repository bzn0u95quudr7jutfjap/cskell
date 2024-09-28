#include <fumo.h>

u32 deltasigma(i32 a, i32 b) {
  i32 sum = a + b;
  return 0 < sum ? sum : 0;
}
