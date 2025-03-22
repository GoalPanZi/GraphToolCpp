#include <Utils/object_model.h>
#include <glm/gtc/constants.hpp>
#include <iostream>

ObjectModel::ObjectModel(GLenum drawflag, glm::vec3 color) {
    this -> drawflag = drawflag;
    this -> color = color;
    this -> id = id;
    if (drawflag==GL_TRIANGLES) {
        this-> name = "MESH";
    } else {
        this-> name = "LINE";
    }

    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO);
};

ObjectModel::ObjectModel(GLenum drawflag,
                        const std::vector<GLfloat>& vertices, 
                        const std::vector<GLuint>& indices, 
                        glm::vec3 color) {
    this -> drawflag = drawflag;
    this -> color = color;
    if (drawflag==GL_TRIANGLES) {
        this-> name = "MESH";
    } else {
        this-> name = "LINE";
    }

    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &outlineVAO);
    glGenBuffers(1, &outlineVBO);

    setVertices(vertices, indices);
};

ObjectModel::~ObjectModel() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
};

void ObjectModel::setVertices(const std::vector<GLfloat>& vertices, const std::vector<GLuint>& indices) {
    this -> vertices = vertices;
    this -> indices = indices;
    this -> outlineVertices = ObjectModel::getOutlineVertices(vertices, 0.01f);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(outlineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, outlineVBO);
    glBufferData(GL_ARRAY_BUFFER, outlineVertices.size() * sizeof(GLfloat), outlineVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
};

void ObjectModel::draw() const {
    glBindVertexArray(VAO);
    glDrawElements(drawflag, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
};

void ObjectModel::setColor(const glm::vec3& color) {
    this -> color = color;
};

void ObjectModel::setName(const std::string& name) {
    this -> name = name;
}

void ObjectModel::setId(const int& id) {
    this -> id = id;
}

const glm::vec3& ObjectModel::getColor() const {
    return color;
};

const GLenum ObjectModel::getDrawFlag() const {
    return drawflag;
};

const std::string& ObjectModel::getName() const {
    return name;
}

void ObjectModel::drawOutline() const {
    glBindVertexArray(outlineVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, outlineVertices.size()/2);
    glBindVertexArray(0);
};

std::vector<GLfloat> ObjectModel::getOutlineVertices(const std::vector<GLfloat>& vertices, float offset) {
    std::vector<GLfloat> outline;
    float pi = glm::pi<float>();
    glm::vec2 prev, current, next;
    int n = vertices.size();
    for (int i = 0 ; i< vertices.size(); i+=2) {
        prev = glm::vec2(vertices[i], vertices[i+1]);
        current = glm::vec2(vertices[(i+2)%n], vertices[(i+3)%n]);
        next = glm::vec2(vertices[(i+4)%n], vertices[(i+5)%n]);

        glm::vec2 dir1 = current-prev;
        glm::vec2 dir2 = next-current;
        float prevTheta = atan2(-dir1.x, dir1.y);
        float nextTheta = atan2(-dir2.x, dir2.y);
        float dTheta = nextTheta - prevTheta;
        if (dTheta<0) {
            dTheta+= 2*pi;
        }
        
        std::cout << prevTheta << ", " << nextTheta << ", " << dTheta << std::endl;

        float thetaStep = dTheta/10.0;
        for (int j = 0; j < 11; j++) {
            float theta = prevTheta + thetaStep*j;
            float px = current.x + offset*cos(theta);
            float py = current.y + offset*sin(theta);
            outline.push_back(px);
            outline.push_back(py);
        }
    }
    return outline;
};