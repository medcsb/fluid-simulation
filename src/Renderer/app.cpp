#include "app.hpp"


App::App() {}
App::~App() {}

void App::init() {
    renderer.init();
}

void App::run() {
    while (!renderer.shouldWindowClose()) {
        renderer.processInput();
        renderer.render();
    }
}