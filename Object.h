//
// Created by qidian on 3/2/18.
//
#pragma once
#ifndef MODEL_VIEW_OBJECT_H
#define MODEL_VIEW_OBJECT_H

#endif //MODEL_VIEW_OBJECT_H

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <glm/vec3.hpp>	// glm::vec3
#include <glm/vec4.hpp>	// glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp>
// glm::translate, glm::rotate, glm::scale, glm::perspective

class Object
{
public:
    Object();
    Object(std::string fileName);
    void load(GLuint &program);
    void draw();
    void getObjInfo(std::string fileName);
    ~Object();

private:
    unsigned long numVert;
    unsigned long numNorm;
    unsigned long numFaces;
    unsigned long numIndex;

    std::vector<glm::vec4> vertList;
    std::vector<glm::vec4> tempVertList;
    std::vector<glm::vec4> normList;
    std::vector<GLuint> vertexIndex;
    std::vector<GLuint> normalsIndex;
    glm::vec4 *vert, *color, *normal;
    GLuint *index;
    GLuint buffer, IndexBuffer;

    std::string title;
};

