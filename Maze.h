#pragma once

#include "displayapp/apps/Apps.h"
#include "displayapp/screens/Screen.h"
#include "displayapp/Controllers.h"
#include "Symbols.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class Maze : public Screen {
      public:
        Maze();
        ~Maze() override;
      };
    }
    
    template <>
    struct AppTraits<Apps::Maze> {
      static constexpr Apps app = Apps::Maze;
      static constexpr const char* icon = Screens::Symbols::l_icon;
      static Screens::Screen* Create(AppControllers& controllers) {
        return new Screens::Maze();
      }
    };
  }
}
