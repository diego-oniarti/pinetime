#pragma once

#include "displayapp/apps/Apps.h"
#include "displayapp/screens/Screen.h"
#include "displayapp/Controllers.h"
#include "Symbols.h"
#include <components/motion/MotionController.h>

#define N_CELLE 7
#define BALL_SIZE 10

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class Maze : public Screen {
      public:
        Maze(Controllers::MotionController& motionController);
        ~Maze() override;
        void Refresh() override;
      private:
        Controllers::MotionController& motionController;
        
        lv_task_t *taskRefresh;
        lv_style_t stile_linea;
        
        int n_muri;
        // matrice di punti [n_muri][2];
        lv_point_t (*punti)[2];

        bool muri_oriz[N_CELLE*N_CELLE+N_CELLE];
        bool muri_vert[N_CELLE*N_CELLE+N_CELLE];

        lv_obj_t *ball;
        lv_style_t stile_ball;
        float pos[2];
        float vel[2];

        lv_obj_t *destination;
        lv_style_t stile_destinazione;
        int cella_destinazione;

        bool won;

        lv_obj_t *scritta;
      };
    }
    
    template <>
    struct AppTraits<Apps::Maze> {
      static constexpr Apps app = Apps::Maze;
      static constexpr const char* icon = Screens::Symbols::l_icon;
      static Screens::Screen* Create(AppControllers& controllers) {
        return new Screens::Maze(controllers.motionController);
      }
    };
  }
}
