#include "ws2812.h"

static PIO pio = pio1;
static uint sm = 0;

static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio, sm, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            (uint32_t) (b);
}

void pattern_snakes(uint len, uint t) {
    for (uint i = 0; i < len; ++i) {
        uint x = (i + (t >> 1)) % 64;
        if (x < 10)
            put_pixel(urgb_u32(0x1f, 0, 0));
        else if (x >= 15 && x < 25)
            put_pixel(urgb_u32(0, 0x1f, 0));
        else if (x >= 30 && x < 40)
            put_pixel(urgb_u32(0, 0, 0x1f));
        else
            put_pixel(0);
    }
}

void pattern_random(uint len, uint t) {
    if (t % 8)
        return;
    for (int i = 0; i < len; ++i)
        put_pixel(rand());
}

void pattern_sparkle(uint len, uint t) {
    if (t % 8)
        return;
    for (int i = 0; i < len; ++i)
        put_pixel(rand() % 16 ? 0 : 0xffffffff);
}

void pattern_greys(uint len, uint t) {
    int max = 100; // let's not draw too much current!
    t %= max;
    for (int i = 0; i < len; ++i) {
        put_pixel(t * 0x10101);
        if (++t >= max) t = 0;
    }
}

typedef void (*pattern)(uint len, uint t);
const struct {
  pattern pat;
  const char *name;
} pattern_table[] = {
  {pattern_snakes,  "Snakes!"},
  {pattern_random,  "Random data"},
  {pattern_sparkle, "Sparkles"},
  {pattern_greys,   "Greys"},
};

void
c_ws2812_init(mrb_vm *vm, mrb_value *v, int argc)
{
  uint offset = pio_add_program(pio, &ws2812_program);
  ws2812_program_init(pio, sm, offset, GET_INT_ARG(1), 800000, true);
}

void
c_ws2812_resume(mrb_vm *vm, mrb_value *v, int argc)
{
  if (tcb_rgb == NULL) return;
  mrbc_resume_task(tcb_rgb);
}

void
c_ws2812_off(mrb_vm *vm, mrb_value *v, int argc)
{
  for (int i=0; i < 8; i++) put_pixel(0);
}

void
c_ws2812_snakes(mrb_vm *vm, mrb_value *v, int argc)
{
  int dir = (rand() >> 30) & 1 ? 1 : -1;
  pattern_snakes(150, dir);
}

void
c_ws2812_put_pixel(mrb_vm *vm, mrb_value *v, int argc)
{
  put_pixel(urgb_u32(GET_INT_ARG(1), GET_INT_ARG(2), GET_INT_ARG(3)));
}
