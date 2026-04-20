#ifndef __MODULE_H
#define __MODULE_H

#include <stdint.h>

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

char _tmp_buffer[128];
static double _alive_time = 0.0f;
Color _bg_color = { 0x20, 0x20, 0x20, 0xff };
Vector2 _v2_text_pos = { .x=0.0f, .y=0.0f };
//
renderer_state_t *state;
int w;  // window width and height, copied from _state
int h;

//
const char *_timer_text(float dt)
{
    memset(_tmp_buffer, 0, 128);
    sprintf(_tmp_buffer, "FPS: %d  dt: %.6f  alive_time: %.0lf s", GetFPS(), dt, _alive_time);
    return _tmp_buffer;
}

// initialize state
void _copy_state(renderer_state_t *_state)
{
    state = _state;
    w = (int)state->dim.x;
    h = (int)state->dim.y;
}

// functions called from hotloader ($HOME/souce/include/hotloaer_raylib/src/hotloader/main.cpp)
//
void dll_init(renderer_state_t *);  // called on module loading, once
void dll_render_callback(float dt); // called once per frame
void dll_shutdown();                // called on module shutdown


#ifdef __cplusplus
}
#endif


#endif //MODULE_H_


