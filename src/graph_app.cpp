#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <Utils/graph_app.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

GraphApp::GraphApp(const int width, const int height, const char* title): width(width), height(height), title(title) {

};

GraphApp::~GraphApp() {
    delete renderer;

    for (auto& object : objects) {
        delete object.second;
    }

    objects.clear();
};

int GraphApp::initialize() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    // OpenGL 버전 설정 (3.3 Core)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 창 생성
    window = glfwCreateWindow(width, height, title, NULL, NULL);
    
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
    glewExperimental = GL_TRUE;

    // GLEW 초기화
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW!" << std::endl;
        return -1;
    }

    // ImGui 초기화
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // 키보드 컨트롤 가능
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    glfwSetScrollCallback(window, scrollCallback);
    glfwSetWindowSizeCallback(window, windowSizeCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    // OpenGL 옵션 설정

    renderer = new Renderer(width, height);

    renderer -> createShader("shaders/vert.glsl", "shaders/frag.glsl");
    renderer -> initializeFramebuffer(width,height);
    renderer -> updateTransform();

        
    // 삼각형 추가
    std::vector<GLfloat> triangleVertices = {
        -0.5f,  0.5f,   // 상단
        -0.5f, 0.0f,   // 좌하단
        0.0f, 0.0f    // 우하단
    };

    std::vector<GLuint> triangleIndices = { 0, 1, 2 };

    // 사각형 추가 (두 개의 삼각형으로 구성)
    std::vector<GLfloat> squareVertices = {
        0.0f,  0.5f,  // 좌상단
        0.0f,  0.0f,  // 좌하단
        0.5f,  0.0f,  // 우하단
        0.5f,  0.5f   // 우상단
    };

    std::vector<GLuint> squareIndices = { 0, 1, 2, 0, 2, 3 };

    // ObjectModel 추가
    ObjectModel* triangle = new ObjectModel(GL_TRIANGLES, triangleVertices, triangleIndices, glm::vec3(1.0f, 0.0f, 0.0f));
    ObjectModel* square = new ObjectModel(GL_TRIANGLES, squareVertices, squareIndices, glm::vec3(0.0f, 1.0f, 0.0f));

    addObject(triangle);
    std::cout<<"triangle id : "<<objectID<<std::endl;
    addObject(square);
    std::cout<<"square id : "<<objectID<<std::endl;

    return 0;
};

int GraphApp::run() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();  // 이벤트 처리
        
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderer -> draw(objects);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground;
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Host", nullptr, window_flags |
                                              ImGuiWindowFlags_NoTitleBar |
                                              ImGuiWindowFlags_NoCollapse |
                                              ImGuiWindowFlags_NoResize |
                                              ImGuiWindowFlags_NoMove |
                                              ImGuiWindowFlags_NoBringToFrontOnFocus |
                                              ImGuiWindowFlags_NoNavFocus);
        ImGui::PopStyleVar(3);

        setupDockSpace();

        ImGui::End();

        // 오른쪽 패널 UI
        objectListPanel.render();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // 정리
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void GraphApp::setSize(int &width, int &height){
    this -> width = width;
    this -> height = height;
    renderer -> resizeFrameBuffer(width, height);
};

void GraphApp::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
    // ImGui 창이 이벤트를 캡처했는지 확인
    if (!ImGui::GetIO().WantCaptureMouse) {
        GraphApp* app = static_cast<GraphApp*>(glfwGetWindowUserPointer(window));
        if (app->renderer) {
            app->renderer->updateZoom(yoffset > 0 ? 1.1f : 0.9f);
        }
    }

};

void GraphApp::windowSizeCallback(GLFWwindow* window, int width, int height) {
    GraphApp* app = static_cast<GraphApp*>(glfwGetWindowUserPointer(window));
    app -> setSize(width, height);
};

void GraphApp::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

    if (ImGui::GetIO().WantCaptureMouse) return;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        int x = static_cast<int>(xpos);
        int y = static_cast<int>(ypos);

        GraphApp* app = static_cast<GraphApp*>(glfwGetWindowUserPointer(window));
        if (app && app->renderer) {
            int id = app->renderer->getObjectIDAt(x, y);
            app->renderer->select(id);
            app->objectListPanel.select(id);
        }
    }
}

void GraphApp::addObject(ObjectModel* object) {
    objectID++;
    object->setId(objectID);
    objects[objectID] = object;
    std::string name = "MESH " + std::to_string(objectID);
    object->setName(name);
    objectListPanel.addObject(objectID, object->getName());
}

void GraphApp::setupDockSpace() {
    ImGuiID dockspace_id = ImGui::GetID("Main Dock Space");

    static bool dockBuilt = false;
    if (!dockBuilt) {
        dockBuilt = true;

        ImGui::DockBuilderRemoveNode(dockspace_id); 
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

        ImGuiID dock_main = dockspace_id;
        ImGuiID dock_right;

        ImGui::DockBuilderSplitNode(dock_main, ImGuiDir_Right, 0.25f, &dock_right, &dock_main);

        ImGui::DockBuilderDockWindow("Object List", dock_right);

        ImGui::DockBuilderFinish(dockspace_id);
    }

    ImGuiDockNodeFlags dockspace_flags =
        ImGuiDockNodeFlags_PassthruCentralNode |
        ImGuiDockNodeFlags_NoDockingInCentralNode;

    ImGui::DockSpace(dockspace_id, ImVec2(0, 0), dockspace_flags);
}
