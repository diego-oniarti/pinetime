#include "displayapp/screens/CubeSpin.h"
#include "displayapp/DisplayApp.h"
#include "displayapp/LittleVgl.h"
#include <lv_conf.h>

#include <algorithm> // std::fill

using namespace Pinetime::Applications::Screens;

#define W 240
#define H 240

CubeSpin::CubeSpin(Controllers::MotionController& motionController) : motionController{motionController} {
    lv_style_init(&stile_linea);
    lv_style_set_line_width(&stile_linea, LV_STATE_DEFAULT, 2);
    lv_style_set_line_color(&stile_linea, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_line_rounded(&stile_linea, LV_STATE_DEFAULT, true);

    for (int i=0; i<12; i++) {
        linee[i] = lv_line_create(lv_scr_act(), nullptr);
        lv_obj_add_style(linee[i], LV_LINE_PART_MAIN, &stile_linea);
        lv_obj_align(linee[i], nullptr, LV_ALIGN_IN_TOP_LEFT, 0,0);
    }

    taskRefresh = lv_task_create(RefreshTaskCallback, LV_DISP_DEF_REFR_PERIOD, LV_TASK_PRIO_MID, this);
}

CubeSpin::~CubeSpin() {
    lv_style_reset(&stile_linea);
    lv_obj_clean(lv_scr_act());
    lv_task_del(taskRefresh);
}

void CubeSpin::Refresh() {
    double s1=0,c1=0;
    double s2=0,c2=0;
    if (!is_free_rotation) {
        s1 = _lv_trigo_sin(-motionController.X()/1000.*90.)/32767.0;
        c1 = _lv_trigo_sin(90.-motionController.X()/1000.*90.)/32767.0;
        s2 = _lv_trigo_sin(motionController.Y()/1000.*90.)/32767.0;
        c2 = _lv_trigo_sin(90.+motionController.Y()/1000.*90.)/32767.0;
    }

    for (int i=0; i<8; i++) {
        if (is_free_rotation) {
            float old_x = coords[i][0];
            float old_z = coords[i][2];
            coords[i][0] = old_x*cos1 - old_z*sin1;
            coords[i][2] = old_x*sin1 + old_z*cos1;

            float old_y = coords[i][1];
            old_z = coords[i][2];
            coords[i][1] = old_y*cos2 - old_z*sin2;
            coords[i][2] = old_y*sin2 + old_z*cos2;
        }else{
            coords[i][1] = coords_original[i][1]*c2 - coords_original[i][2]*s2;
            coords[i][2] = coords_original[i][1]*s2 + coords_original[i][2]*c2;

            coords[i][0] = coords_original[i][0]*c1 - coords[i][2]*s1;
            coords[i][2] = coords_original[i][0]*s1 + coords[i][2]*c1;
        }

        display_points[i].x = coords[i][0]/3/(coords[i][2]+3)*W;
        display_points[i].y = coords[i][1]/3/(coords[i][2]+3)*H;

        display_points[i].x += W/2;
        display_points[i].y += H/2;
    }
    for (int i=0; i<12; i++) {
        punti_linee[i][0] = display_points[indici[i][0]];
        punti_linee[i][1] = display_points[indici[i][1]];
        lv_line_set_points(linee[i], punti_linee[i], 2);
    }
}

bool CubeSpin::OnTouchEvent(Pinetime::Applications::TouchEvents event) {
    if (event != Pinetime::Applications::TouchEvents::Tap) return true;
    is_free_rotation = !is_free_rotation;
    if (is_free_rotation) return true;
    for (int i=0; i<8; i++) {
        for (int j=0; j<3; j++) {
            coords[i][j] = coords_original[i][j];
        }
    }
    return true;
}
