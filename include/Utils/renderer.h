#pragma once

#include <vector>
#include <map>
#include <Utils/object_model.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>
#include <string>
#include <set>

class Renderer{
    private:
        GLuint shaderProgram;
        GLuint transformLoc;
        GLuint idLoc;
        GLuint colorLoc;
        GLuint fbo, screenBuffer, idBuffer, depthBuffer, resolveFbo,resolveIDBuffer;
        std::set<int> selected;

        int width, height;

        float zoomlevel = 1.0f;
        float screenOffsetX = 0.0f;
        float screenOffsetY = 0.0f;

        glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::mat4(1.0f);

    public:

        Renderer(int width, int height);

        void createShader(const std::string& vertexPath, const std::string& fragmentPath);
        void initializeFramebuffer(int width, int height);
        void resizeFrameBuffer(int width, int height);
        void resolveBuffer();


        void draw(const std::unordered_map<int, ObjectModel*>& objects);

        void useShader();

        void select(int id);
        void setTransform(glm::mat4& transform);
        int getObjectIDAt(int x, int y);
        void setID(int id);
        void setColor(glm::vec3 color);
        void updateZoom(float multiplier);
        void updateScreenOffset(float offsetX, float offsetY);
        void updateTransform();
        void reBindFramebuffer();

};