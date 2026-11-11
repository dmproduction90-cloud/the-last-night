#include <pspkernel.h>
#include <pspctrl.h>
#include "gfx.h"
#include "world.h"
#include "player.h"
#include "camera.h"

PSP_MODULE_INFO("THE LAST NIGHT", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

static int exit_callback(int arg1, int arg2, void *common)
{
    sceKernelExitGame();
    return 0;
}

static int callback_thread(SceSize args, void *argp)
{
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

static void setup_callbacks(void)
{
    int thid = sceKernelCreateThread("update_thread", callback_thread, 0x11, 0xFA0, 0, 0);
    if (thid >= 0) sceKernelStartThread(thid, 0, 0);
}

int main(void)
{
    setup_callbacks();

    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    gfx_init();
    world_init();

    Player ethan;
    Camera cam;
    player_init(&ethan, 5.0f, 10.0f);
    camera_init(&cam, 3.14159265f); /* regarde vers le nord (-z) */

    int flashlight = 0;
    unsigned int prev = 0;
    const float dt = 1.0f / 60.0f;

    while (1) {
        SceCtrlData pad;
        sceCtrlReadBufferPositive(&pad, 1);

        float sx = ((float)pad.Lx - 128.0f) / 128.0f;
        float sy = ((float)pad.Ly - 128.0f) / 128.0f;

        float yaw_in = 0.0f;
        if (pad.Buttons & PSP_CTRL_LTRIGGER) yaw_in -= 1.0f;
        if (pad.Buttons & PSP_CTRL_RTRIGGER) yaw_in += 1.0f;

        unsigned int pressed = pad.Buttons & ~prev;
        prev = pad.Buttons;
        if (pressed & PSP_CTRL_SQUARE) flashlight = !flashlight;

        player_update(&ethan, sx, sy, cam.yaw, dt);
        camera_update(&cam, yaw_in, dt);

        /* lampe torche provisoire : repousse simplement le brouillard */
        gfx_begin_frame(flashlight ? 22.0f : 9.0f);
        camera_apply(&cam, ethan.x, ethan.z);
        world_draw();
        player_draw(&ethan);
        gfx_end_frame();
    }

    sceKernelExitGame();
    return 0;
}
