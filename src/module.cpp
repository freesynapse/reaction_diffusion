
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <raylib.h>
#include <raymath.h>

#include "module.h"

#include "log.h"
#include "raylib_utils.h"


#ifdef __cplusplus
extern "C" {
#endif

struct ab_t {
    union {
        struct {
            float a; 
            float b;
        };
        float f[2];
    };

};
ab_t *ab, *ab_back;
#define AB_SIZE sizeof(ab_t) * w * h

// system parameters
float Da = 1.0f;
float Db = 0.5f;
float f  = 0.055f;
float k  = 0.062f;

// called once on dll reload
void dll_init(renderer_state_t *state)
{
    timer_start();

    // copy state and parameters from hotloader
    _copy_state(state);
    SetExitKey(KEY_ESCAPE);
    SetTargetFPS(20);

    //
    CreatePixelBuffer(w, h);
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            int idx = j * w + i;
            PIXELS[idx] = px_t(255, 0, 0, 100);
        }
    }
    CreatePixelTexture(w, h);

    //
    ab = (ab_t *)malloc(AB_SIZE);
    ab_back = (ab_t *)malloc(AB_SIZE);
    memset(ab, 0, AB_SIZE);
    memset(ab_back, 0, AB_SIZE);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx = y * w + x;
            ab[idx].a = 1.0f;
            ab[idx].b = 0.0f;
        }
    }

    // seed with circles
    vec2 p;
    int splat_count = 40;
    int radius = 50;
    for (int i = 0; i < splat_count; i++) {
        p = vec2rand(50, w-50, 50, h-50);
        for (int y = p.y - radius; y < p.y + radius; y++) {
            for (int x = p.x - radius; x < p.x + radius; x++) {
                int idx = y * w + x;
                int r = sqrtf((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y));
                if (r < radius)
                    ab[idx].b = 1.0f;
            }
        }
    }

    //
    float t = timer_elapsed();
    TRACE_INFO("module.so initialized in %.2f ms.\n", t);
}

// called once every frame, from ../loader_src/main.cpp
float laplace(int sidx, int idx)
{
    // sidx : index into the ab_t struct -- a (0) or b (1)
    // idx  : index into the ab array
    float l = 0.0f;
    l += -1.0f * ab[(idx)].f[sidx];
    l +=  0.2f * ab[(idx)-1].f[sidx];
    l +=  0.2f * ab[(idx)+1].f[sidx];
    l +=  0.2f * ab[(idx)-w].f[sidx];
    l +=  0.2f * ab[(idx)+w].f[sidx];
    l += 0.05f * ab[(idx)-w-1].f[sidx];
    l += 0.05f * ab[(idx)-w+1].f[sidx];
    l += 0.05f * ab[(idx)+w-1].f[sidx];
    l += 0.05f * ab[(idx)+w+1].f[sidx];
    return l;
}

//
void dll_render_callback(float dt)
{
    BeginDrawing();

    ClearBackground(_bg_color);

    // update system
    timer_start();
    for (int y = 1; y < h-1; y++) {
        for (int x = 1; x < w-1; x++) {
            int idx = y * w + x;
            float a = ab[idx].a;
            float b = ab[idx].b;
            float abb = a * b * b;
            ab_back[idx].a = a + (Da * laplace(0, idx)) - abb + (f * (1.0f - a));
            ab_back[idx].b = b + (Db * laplace(1, idx)) + abb - (b * (k + f));
        }
    }

    // swap buffers
    ab_t *tmp = ab;
    ab = ab_back;
    ab_back = tmp;

    // update pixels
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx = y * w + x;
            uint8_t c = (uint8_t)(ab[idx].b * 255);
            PIXELS[idx] = px_t(c, c, c, 255);
        }
    }
    for (int x = 0; x < w; x++) {
        PIXELS[x] = PX_BLACK;
        PIXELS[w * (h - 1) + x] = PX_BLACK;
    }
    for (int y = 0; y < h; y++) {
        PIXELS[y * w] = PX_BLACK;
        PIXELS[y * w + w - 1] = PX_BLACK;
    }
    float ms0 = timer_elapsed();

    //
    UpdatePixelTexture(PIXELS);
    DrawPixelTexture();
    float ms1 = timer_elapsed();
    // printf("loops: %.2f ms, textures: %.2f ms\n", ms0, ms1);

    //
    DrawTextEx(state->font, _timer_text(dt), _v2_text_pos, state->font_size, 0.0f, WHITE);
    
    EndDrawing();

    _alive_time += dt;

}

// called on module shutdown
void dll_shutdown()
{
    ReleasePixelTexture();

}


#ifdef __cplusplus
}
#endif


