#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <Utils/UI/object_list_panel.h>
#include <Utils/renderer.h>

class GraphApp {
    private:
        int width, height;
        const char* title;
        int objectID = 0;
        GLFWwindow* window;

        Renderer* renderer;
        std::unordered_map<int, ObjectModel*> objects;


        static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        static void windowSizeCallback(GLFWwindow* window, int width, int height);
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

        void setupDockSpace();

        ObjectListPanel objectListPanel;

    public:

        GraphApp(const int width, const int height, const char* title);
        ~GraphApp();

        void addObject(ObjectModel* object);

        int initialize();

        int run();

        void setSize(int &width, int &height);

};