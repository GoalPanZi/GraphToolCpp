#pragma once

#include <vector>
#include <unordered_map>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

class ObjectModel {
private:
    GLuint VAO, VBO, EBO;
    GLuint outlineVAO, outlineVBO;
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> outlineVertices;
    std::vector<GLuint> indices;
    glm::vec3 color;
    int id;
    GLenum drawflag;
    std::string name;

public:
    ObjectModel(GLenum drawflag, glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f));
    ObjectModel(GLenum drawflag,
        const std::vector<GLfloat>& vertices, 
        const std::vector<GLuint>& indices, 
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f));
    ~ObjectModel();
    

    static std::vector<GLfloat> getOutlineVertices(const std::vector<GLfloat>& vertices, float offset);

    void setVertices(const std::vector<GLfloat>& vertices, const std::vector<GLuint>& indices);
    void setColor(const glm::vec3& color);
    void setName(const std::string& name);
    void setId(const int& id);

    const glm::vec3& getColor() const;
    const GLenum getDrawFlag() const;
    const std::string& getName() const;

    void draw() const;
    void drawOutline() const;

};