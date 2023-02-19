#include "Utils.h"

float Utils::GetUniformRandom(float min, float max) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_real_distribution<float> dist(min, max); // distribution in range [min, max]

    return dist(rng);
}

// ��λ���ڵ��������
glm::vec3 Utils::randomVec3()
{
    float z = GetUniformRandom(-1.0f, 1.0f);
    float phi = GetUniformRandom(0.0f, 2 * Consts::M_PI);
    float r = std::sqrt(1.0f - z * z);
    return glm::vec3(r * std::cos(phi), r * std::sin(phi), z);
}

// ��������������
glm::vec3 Utils::randomDirection(glm::vec3 n)
{
    glm::vec3 randVec = randomVec3();
    float dotNToRandVec = dot(n, randVec);
    if (dotNToRandVec < 0) {
        randVec = randVec + dotNToRandVec * n;
    }
    return randVec;
}

glm::vec3 Utils::RandomDirectionFromLDS(glm::vec3 n, float x, float y)
{
    float z = 2.0 * (x - 0.5f);
    float phi = y * 2 * Consts::M_PI;
    float r = std::sqrt(1.0f - z * z);
    glm::vec3 randVec = glm::vec3(r * std::cos(phi), r * std::sin(phi), z);

    float dotNToRandVec = dot(n, randVec);
    if (dotNToRandVec < 0) {
        randVec = randVec + dotNToRandVec * n;
    }
    return randVec;
}
