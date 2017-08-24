#pragma once

#include <list>
#include <memory>
#include "nuklear/nuklear.h"
#include "GUI/INKWindow.hpp"

class GUIManager {
public:
        std::list<std::shared_ptr<INKWindow>> windows;

        void add(std::shared_ptr<INKWindow> win) {
                windows.push_back(win);
        }

        void draw(nk_context* ctx) {
                for (auto& win : windows) {
                        win->draw(ctx);
                }
        }

        static GUIManager& get() {
                static GUIManager instance;
                return instance;
        }
};
