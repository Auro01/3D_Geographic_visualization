#ifndef SHAPES_HPP
#define SHAPES_HPP

#include <vector>
#include <queue>
#include <cmath>

#include "glm/glm.hpp"

#define PI   3.14159265
#define PI_4 0.78539816

using namespace std;

struct Object {
    vector<glm::dvec3> vertices;
    vector<glm::dvec2> tex;
    glm::dmat4 modelTrans;
    void draw(const glm::dmat4 & camera);
};

void Object::draw(const glm::dmat4 & camera) {
    glm::dmat4 modelView = camera * this->modelTrans;
    glLoadMatrixd(&modelView[0][0]);
    glBegin(GL_TRIANGLES);
        auto tex = this->tex.begin();
        for(auto & vertex : this->vertices) {
            if(tex != this->tex.end()) {
                glTexCoord2f(tex->x, tex->y);
                ++tex;
            }
            glVertex3d(vertex.x, vertex.y, vertex.z);
        }
    glEnd();
}

void sphere(glm::dvec3 center, double radius, int n, vector<Object> & objects) {
    // Octahedron
    auto top = center + glm::dvec3(0, radius, 0);
    auto nearRight = center + glm::dvec3(radius * sin(PI_4), 0, radius * cos(PI_4));
    auto farRight = center + glm::dvec3(radius * sin(3 * PI_4), 0, radius * cos(3 * PI_4));  
    auto farLeft = center + glm::dvec3(radius * sin(5 * PI_4), 0, radius * cos(5 * PI_4));  
    auto nearLeft = center + glm::dvec3(radius * sin(7 * PI_4), 0, radius * cos(7 * PI_4));
    auto bottom = center + glm::dvec3(0, -radius, 0);

    queue<glm::dvec3> sphere;
    queue<glm::dvec2> texCoor;

    // Top front
    sphere.push(top); sphere.push(nearLeft); sphere.push(nearRight);
    // Top left
    sphere.push(top); sphere.push(farLeft); sphere.push(nearLeft);
    // Top back
    sphere.push(top); sphere.push(farRight); sphere.push(farLeft);
    // Top right
    sphere.push(top); sphere.push(nearRight); sphere.push(farRight);
    // Bottom front
    sphere.push(bottom); sphere.push(nearRight); sphere.push(nearLeft);
    // Bottom left
    sphere.push(bottom); sphere.push(nearLeft); sphere.push(farLeft);
    // Bottom back
    sphere.push(bottom); sphere.push(farLeft); sphere.push(farRight);
    // Bottom right
    sphere.push(bottom); sphere.push(farRight); sphere.push(nearRight);

    texCoor.emplace(0.499630, 0.748775); texCoor.emplace(0.251069, 0.500144);
    texCoor.emplace(0.499490, 0.252187); texCoor.emplace(0.499699, 0.749069);
    texCoor.emplace(0.251179, 0.996520); texCoor.emplace(0.251173, 0.500367);
    texCoor.emplace(0.499563, 0.749294); texCoor.emplace(0.748507, 0.501088);
    texCoor.emplace(0.749492, 0.998144); texCoor.emplace(0.500569, 0.748330);
    texCoor.emplace(0.499944, 0.252037); texCoor.emplace(0.749697, 0.500795);
    texCoor.emplace(0.250712, 0.003605); texCoor.emplace(0.499628, 0.252806);
    texCoor.emplace(0.251354, 0.501213); texCoor.emplace(0.001723, 0.748779);
    texCoor.emplace(0.250503, 0.500652); texCoor.emplace(0.251034, 0.996953);
    texCoor.emplace(0.997805, 0.749238); texCoor.emplace(0.749430, 0.997199);
    texCoor.emplace(0.749169, 0.499891); texCoor.emplace(0.749293, 0.003298);
    texCoor.emplace(0.748966, 0.500445); texCoor.emplace(0.499575, 0.252136);

    if(n > 0) {
        unsigned iterations = 8 * pow(4, n - 1);

        // Subdivide
        for(unsigned i = 0; i < iterations; i++) {
            auto a = sphere.front(); sphere.pop();
            auto b = sphere.front(); sphere.pop();
            auto c = sphere.front(); sphere.pop();

            auto texA = texCoor.front(); texCoor.pop();
            auto texB = texCoor.front(); texCoor.pop();
            auto texC = texCoor.front(); texCoor.pop();

            auto d = (c + b) / 2.0;
            auto e = (a + c) / 2.0;
            auto f = (b + a) / 2.0;

            auto texD = (texC + texB) / 2.0;
            auto texE = (texA + texC) / 2.0;
            auto texF = (texB + texA) / 2.0;

            d = (d - center) * radius / glm::distance(center, d);
            e = (e - center) * radius / glm::distance(center, e);
            f = (f - center) * radius / glm::distance(center, f);

            sphere.push(a); sphere.push(f); sphere.push(e);
            sphere.push(f); sphere.push(b); sphere.push(d);
            sphere.push(d); sphere.push(e); sphere.push(f);
            sphere.push(e); sphere.push(d); sphere.push(c);

            texCoor.push(texA); texCoor.push(texF); texCoor.push(texE);
            texCoor.push(texF); texCoor.push(texB); texCoor.push(texD);
            texCoor.push(texD); texCoor.push(texE); texCoor.push(texF);
            texCoor.push(texE); texCoor.push(texD); texCoor.push(texC);
        }
    }

    objects.emplace_back();
    Object & object = objects.back();

    while(!sphere.empty()) {
        object.vertices.push_back(sphere.front());
        object.tex.push_back(texCoor.front());
        sphere.pop();
        texCoor.pop();
    }
}

void marker(glm::dvec3 center, double side, double height, glm::dvec3 direction, int n, vector<Object> & objects) {
    auto len = side / 2;
    auto lfr = center + glm::dvec3(len, 0, -len);
    auto lfl = center + glm::dvec3(-len, 0, -len);
    auto lnr = center + glm::dvec3(len, 0, len);
    auto lnl = center + glm::dvec3(-len, 0, len);
    auto ufr = center + direction * height + glm::dvec3(len, 0, -len);
    auto ufl = center + direction * height + glm::dvec3(-len, 0, -len);
    auto unr = center + direction * height + glm::dvec3(len, 0, len);
    auto unl = center + direction * height + glm::dvec3(-len, 0, len);

    objects.emplace_back();

    auto & object = objects.back();

    // Front
    object.vertices.push_back(unr); object.vertices.push_back(lnl); object.vertices.push_back(lnr);
    object.vertices.push_back(lnl); object.vertices.push_back(unr); object.vertices.push_back(unl);
    
    // Left
    object.vertices.push_back(unl); object.vertices.push_back(lfl); object.vertices.push_back(lnl);
    object.vertices.push_back(lfl); object.vertices.push_back(unl); object.vertices.push_back(ufl);
    
    // Back
    object.vertices.push_back(ufl); object.vertices.push_back(lfr); object.vertices.push_back(ufr);
    object.vertices.push_back(lfr); object.vertices.push_back(lfl); object.vertices.push_back(ufl);

    // Right
    object.vertices.push_back(unl); object.vertices.push_back(lfl); object.vertices.push_back(lnl);
    object.vertices.push_back(lfl); object.vertices.push_back(unl); object.vertices.push_back(lfl);

    // Top
    object.vertices.push_back(ufr); object.vertices.push_back(unl); object.vertices.push_back(unr);
    object.vertices.push_back(ufl); object.vertices.push_back(unl); object.vertices.push_back(ufr);

    // Bot
    object.vertices.push_back(lnr); object.vertices.push_back(lfl); object.vertices.push_back(lnl);
    object.vertices.push_back(lfr); object.vertices.push_back(lfl); object.vertices.push_back(lnr);
}

#endif