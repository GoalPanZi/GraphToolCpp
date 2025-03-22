#include <Utils/renderer.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

Renderer::Renderer(int width, int height): width(width), height(height) {
};

void Renderer::createShader(const std::string& vertexPath, const std::string& fragmentPath) {
    std::ifstream vertexFile(vertexPath);
    if (!vertexFile.is_open()) {
        std::cerr << "Failed to open vertex shader file!" << std::endl;
        return;
    }
    std::stringstream vertexBuffer;
    vertexBuffer << vertexFile.rdbuf();

    std::string vertexCode = vertexBuffer.str();

    std::ifstream fragmentFile(fragmentPath);
    if (!fragmentFile.is_open()) {
        std::cerr << "Failed to open fragment shader file!" << std::endl;
        return;
    }
    std::stringstream fragmentBuffer;
    fragmentBuffer << fragmentFile.rdbuf();

    std::string fragmentCode = fragmentBuffer.str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexCodeCstr = vertexCode.c_str();
    glShaderSource(vertexShader, 1, &vertexCodeCstr, NULL);
    glCompileShader(vertexShader);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "Failed to compile vertex shader!" << std::endl;
        std::cerr << infoLog << std::endl;
        return;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentCodeCstr = fragmentCode.c_str();
    glShaderSource(fragmentShader, 1, &fragmentCodeCstr, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "Failed to compile fragment shader!" << std::endl;
        std::cerr << infoLog << std::endl;
        return;
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Failed to link shader program!" << std::endl;
        std::cerr << infoLog << std::endl;
        return;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    transformLoc = glGetUniformLocation(shaderProgram, "transform");
    idLoc = glGetUniformLocation(shaderProgram, "id");
    colorLoc = glGetUniformLocation(shaderProgram, "color");
};

void Renderer::useShader(){
    glUseProgram(shaderProgram);
};

void Renderer::setTransform(glm::mat4& transform) {
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
    glUseProgram(0);
};

void Renderer::setID(int id) {
    glUniform1i(idLoc, id);
};

void Renderer::setColor(glm::vec3 color) {
    glUniform3fv(colorLoc, 1, glm::value_ptr(color));
};

void Renderer::updateTransform() {
    glm::mat4 transform = projection * view;
    for (int i =0; i<4; i++){
        for (int j = 0; j<4; j++){
            std::cout<<transform[i][j]<<" ";
        }
        std::cout<<std::endl;
    }
    setTransform(transform);
};

void Renderer::updateZoom(float multiplier) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    zoomlevel *= multiplier;
    projection = glm::ortho(-1.0f * zoomlevel, 1.0f * zoomlevel, -1.0f * zoomlevel, 1.0f * zoomlevel, -1.0f, 1.0f);
    updateTransform();
};

void Renderer::updateScreenOffset(float offsetX, float offsetY) {
    screenOffsetX = offsetX;
    screenOffsetY = offsetY;
    view = glm::translate(glm::mat4(1.0f), glm::vec3(screenOffsetX, screenOffsetY, 0.0f));
    updateTransform();
};

void Renderer::draw(const std::unordered_map<int, ObjectModel*>& objects) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    useShader();

    for (auto& object : objects) {
        GLint id = object.first;
        if (selected.find(id) != selected.end()) {
            if (object.second-> getDrawFlag() == GL_TRIANGLES) {
                setColor(glm::vec3(1.0f, 1.0f, 1.0f));
                object.second-> drawOutline();
            }
        }
        setID(id); // 객체 ID 설정
        setColor(object.second->getColor());
        object.second->draw();
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);

    glBlitFramebuffer(0, 0, width, height,
                     0, 0, width, height,
                    GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int Renderer::getObjectIDAt(int x, int y) {
    resolveBuffer();

    glBindFramebuffer(GL_READ_FRAMEBUFFER, resolveIDBuffer);
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    GLint id = 0;
    glReadPixels(x, height - y, 1, 1, GL_RED_INTEGER, GL_INT, &id);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    return id;
};

void Renderer::initializeFramebuffer(int width, int height) {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenRenderbuffers(1, &screenBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, screenBuffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, screenBuffer);

    glGenTextures(1, &idBuffer);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, idBuffer);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_R32I, width, height, GL_TRUE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D_MULTISAMPLE, idBuffer, 0);

    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr<< "Framebuffer is not complete" << std::endl;
    };

    glGenFramebuffers(1, &resolveFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, resolveFbo);

    glGenTextures(1, &resolveIDBuffer);
    glBindTexture(GL_TEXTURE_2D, resolveIDBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, width, height, 0, GL_RED_INTEGER, GL_INT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, resolveIDBuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Resolve Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
};

void Renderer::resizeFrameBuffer(int width, int height){
    this -> width = width;
    this -> height = height;

    glBindRenderbuffer(GL_RENDERBUFFER, screenBuffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, screenBuffer);

    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, idBuffer);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_R32I, width, height, GL_TRUE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D_MULTISAMPLE, idBuffer, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr<< "Framebuffer is not complete" << std::endl;
    };

    glBindFramebuffer(GL_FRAMEBUFFER, resolveFbo);

    glBindTexture(GL_TEXTURE_2D, resolveIDBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, width, height, 0, GL_RED_INTEGER, GL_INT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, resolveIDBuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Resolve Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
};

void Renderer::resolveBuffer() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);  // MSAA FBO
    glReadBuffer(GL_COLOR_ATTACHMENT1);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolveFbo);  // 비-MSAA FBO


    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, 
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
};

void Renderer::reBindFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
};

void Renderer::select(int id) {
    if (id>0){
        if (selected.find(id) != selected.end()) {
            selected.erase(id);
        }
        else {
            selected.clear();
            selected.insert(id);
        }
    } else {
        selected.clear();
    }
};