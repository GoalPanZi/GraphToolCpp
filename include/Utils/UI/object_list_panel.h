#pragma once

#include <unordered_map>
#include <string>
#include <imgui.h>

class ObjectListPanel {
private:
    std::unordered_map<int, std::string> objects;
    int selectedObject;
    ImGuiWindowFlags window_options;

public:
    ObjectListPanel();
    ~ObjectListPanel();

    void render();
    void addObject(int id, const std::string& name);
    void select(int id);
};