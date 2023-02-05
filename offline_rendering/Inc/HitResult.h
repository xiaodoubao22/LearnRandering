#ifndef HIT_RESULT_H
#define HIT_RESULT_H

#include "Utils.h"
#include "Material.h"

class HitResult
{
public:
    int isHit = 0;             // �Ƿ�����(1Ϊ���У�����ֵδ����)
    float distance = FLT_MAX;         // �뽻��ľ���
    glm::vec3 hitPoint = glm::vec3(0.0f, 0.0f, 0.0f);  // �������е�
    Material* material = nullptr;              // ���е�ı������
    glm::vec3 normal;                // ������
};

#endif // !HIT_RESULT


