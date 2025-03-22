#include <Utils/graph_app.h>
#include <iostream>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

int main() {
    // GLFW 초기화

    GraphApp* app = new GraphApp(WINDOW_WIDTH, WINDOW_HEIGHT, "Graph Tool");
    
    app->initialize();

    app->run();

    return 0;
}