#ifndef APP_HPP
#define APP_HPP

#include "renderer.hpp"

class App {

private:
    Renderer renderer;

public:
    App();
    ~App();

    void init();
    void run();
};

#endif // APP_HPP