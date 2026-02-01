#pragma once

#include <glm/glm.hpp>
#include <vector>

struct Plane {
    glm::vec3 normal;
    float distance;

    Plane() : normal(0, 1, 0), distance(0) {}
    Plane(const glm::vec3& p1, const glm::vec3& norm) : normal(glm::normalize(norm)), distance(glm::dot(normal, p1)) {}

    float getSignedDistanceToPlane(const glm::vec3& point) const {
        return glm::dot(normal, point) - distance;
    }
};

class Frustum {
public:
    Plane topFace;
    Plane bottomFace;
    Plane rightFace;
    Plane leftFace;
    Plane farFace;
    Plane nearFace;

    void update(const glm::mat4& viewProjection) {
        // Implementation of frustum extraction from view-projection matrix
        // Standard method: rows of the matrix represent the planes
        glm::mat4 m = glm::transpose(viewProjection);
        leftFace.normal   = glm::vec3(m[3] + m[0]);
        leftFace.distance = -(m[3].w + m[0].w);
        
        rightFace.normal   = glm::vec3(m[3] - m[0]);
        rightFace.distance = -(m[3].w - m[0].w);
        
        bottomFace.normal   = glm::vec3(m[3] + m[1]);
        bottomFace.distance = -(m[3].w + m[1].w);
        
        topFace.normal   = glm::vec3(m[3] - m[1]);
        topFace.distance = -(m[3].w - m[1].w);
        
        nearFace.normal   = glm::vec3(m[3] + m[2]);
        nearFace.distance = -(m[3].w + m[2].w);
        
        farFace.normal   = glm::vec3(m[3] - m[2]);
        farFace.distance = -(m[3].w - m[2].w);

        // Normalize planes
        normalizePlane(leftFace);
        normalizePlane(rightFace);
        normalizePlane(bottomFace);
        normalizePlane(topFace);
        normalizePlane(nearFace);
        normalizePlane(farFace);
    }

    bool isBoxInFrustum(const glm::vec3& min, const glm::vec3& max) const {
        return isPlaneInFrustum(leftFace, min, max) &&
               isPlaneInFrustum(rightFace, min, max) &&
               isPlaneInFrustum(topFace, min, max) &&
               isPlaneInFrustum(bottomFace, min, max) &&
               isPlaneInFrustum(nearFace, min, max) &&
               isPlaneInFrustum(farFace, min, max);
    }

private:
    void normalizePlane(Plane& plane) {
        float length = glm::length(plane.normal);
        plane.normal /= length;
        plane.distance /= length;
    }

    bool isPlaneInFrustum(const Plane& plane, const glm::vec3& min, const glm::vec3& max) const {
        glm::vec3 p = min;
        if (plane.normal.x >= 0) p.x = max.x;
        if (plane.normal.y >= 0) p.y = max.y;
        if (plane.normal.z >= 0) p.z = max.z;

        if (plane.getSignedDistanceToPlane(p) < 0) return false;
        return true;
    }
};
