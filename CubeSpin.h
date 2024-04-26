#pragma once

#include <lvgl/lvgl.h>
#include "displayapp/apps/Apps.h"
#include "displayapp/screens/Screen.h"
#include "displayapp/Controllers.h"
#include "Symbols.h"
#include <components/motion/MotionController.h>
#include "systemtask/SystemTask.h"

#include <vector>

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class CubeSpin : public Screen {
      public:
        CubeSpin(Controllers::MotionController& motionController);
        ~CubeSpin() override;
        void Refresh() override;
        bool OnTouchEvent(TouchEvents event) override;
      private:
        Controllers::MotionController& motionController;
        float coords_original[8][3] {
            {-1, -1, -1},
            {-1,  1, -1},
            { 1,  1, -1},
            { 1, -1, -1},
            {-1, -1,  1},
            {-1,  1,  1},
            { 1,  1,  1},
            { 1, -1,  1}
        };
        float coords[8][3] {
            {-1, -1, -1},
            {-1,  1, -1},
            { 1,  1, -1},
            { 1, -1, -1},
            {-1, -1,  1},
            {-1,  1,  1},
            { 1,  1,  1},
            { 1, -1,  1}
        };
        lv_point_t display_points[8] {
            {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}
        };

        int indici[12][2] {
            {0,1},
            {1,2},
            {2,3},
            {3,0},
            {4,5},
            {5,6},
            {6,7},
            {7,4},
            {0,4},
            {1,5},
            {2,6},
            {3,7},
        };
        lv_obj_t *linee[12];
        lv_point_t punti_linee[12][2];
        lv_style_t stile_linea;

        lv_task_t *taskRefresh;

        bool is_free_rotation = true;

        double sin1 = _lv_trigo_sin(3)/32767.0; 
        double cos1 = _lv_trigo_sin(93)/32767.0; 
        double sin2 = _lv_trigo_sin(5)/32767.0; 
        double cos2 = _lv_trigo_sin(95)/32767.0; 
      };
    }
    
    template <>
    struct AppTraits<Apps::CubeSpin> {
      static constexpr Apps app = Apps::CubeSpin;
      static constexpr const char* icon = Screens::Symbols::cube;
      static Screens::Screen* Create(AppControllers& controllers) {
        return new Screens::CubeSpin(controllers.motionController);
      }
    };
  }
}
