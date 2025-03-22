#include <imgui.h>
#include <Utils/UI/object_list_panel.h>

ObjectListPanel::ObjectListPanel() {
    window_options = ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_DockNodeHost;
}

ObjectListPanel::~ObjectListPanel() {}

void ObjectListPanel::render() {
    static bool firstRender = true;

    if (firstRender) {
        ImGui::SetNextWindowSize(ImVec2(200, 0));  // width = 200, height = auto
        firstRender = false;
    }

    ImGui::Begin("Object List", nullptr, window_options);

    for (const auto& object: objects) {
        if (ImGui::Selectable(object.second.c_str(), selectedObject == object.first)) {
            selectedObject = object.first;
        }
    }

    ImGui::End();
}

void ObjectListPanel::addObject(int id, const std::string& name) {
    objects[id] = name;
}

void ObjectListPanel::select(int id) {
    if (id > 0 && objects.find(id) != objects.end() && id!=selectedObject) {
        selectedObject = id;
    } else {
        selectedObject = 0;
    }
}