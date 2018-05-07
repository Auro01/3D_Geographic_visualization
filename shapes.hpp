#ifndef SHAPES_HPP
#define SHAPES_HPP

#include <vector>
#include <queue>

#include "glm/vec3.hpp"
#include "glm/matrix.hpp"

using namespace std;

struct Object {
    vector<glm::dvec3> verteces;
    glm::dmat4 modelTrans;
};

void sphere(glm::dvec3 center, double radius, int n, vector<Object> & objects) {
    // Octahedron
    auto top = center + glm::dvec3(0, radius, 0);
    auto farRight = center + glm::dvec3(radius, 0, -radius);
    auto farLeft = center + glm::dvec3(-radius, 0, -radius);    
    auto nearRight = center + glm::dvec3(radius, 0, radius);
    auto nearLeft = center + glm::dvec3(-radius, 0, radius);
    auto bottom = center + glm::dvec3(0, -radius, 0);

    queue<glm::dvec3> sphere;

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

    unsigned iterations = 8 * pow(4, n - 1);

    // Subdivide
    for(unsigned i = 0; i < iterations; i++) {
        auto a = sphere.front(); sphere.pop();
        auto b = sphere.front(); sphere.pop();
        auto c = sphere.front(); sphere.pop();

        auto d = (c + b) / 2.0;
        auto e = (a + c) / 2.0;
        auto f = (b + a) / 2.0;

        d = (d - center) * radius / glm::distance(a, b);
        e = (e - center) * radius / glm::distance(a, b);
        f = (f - center) * radius / glm::distance(a, b);

        sphere.push(a); sphere.push(f); sphere.push(e);
        sphere.push(f); sphere.push(b); sphere.push(d);
        sphere.push(d); sphere.push(e); sphere.push(f);
        sphere.push(e); sphere.push(d); sphere.push(c);
    }

    objects.emplace_back();
    Object & object = objects.back();

    while(!sphere.empty()) {
        object.verteces.push_back(sphere.front());
        sphere.pop();
    }
}

#endif