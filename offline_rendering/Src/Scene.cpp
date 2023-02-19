#include "Scene.h"

Scene::Scene() {
}

Scene::~Scene() {
    for (auto shape : mShapes) {
        if (shape != nullptr) {
            delete shape;
        }
    }

    for (auto material : mMaterials) {
        if (material != nullptr) {
            delete material;
        }
    }

    for (auto texure : mTexures) {
        if (texure != nullptr) {
            delete texure;
        }
    }
}

bool Scene::PushShape(Shape* shape) {
	if (shape == nullptr) {
		return false;
	}

	mShapes.push_back(shape);
	return true;
}

bool Scene::PushMaterial(Material* material) {
    if (material == nullptr) return false;

    mMaterials.push_back(material);
    return true;
}

bool Scene::PushTexure(TexureSampler* texure) {
    if (texure == nullptr) return false;

    mTexures.push_back(texure);
    return true;
}

HitResult Scene::GetIntersect(Ray ray) {
    HitResult res, r;
    res.distance = FLT_MAX;

    // ��������ͼ�Σ����������
    for (auto& shape : mShapes)
    {
        r = shape->Intersect(ray);
        if (r.isHit == 1 && r.distance < res.distance) res = r;  // ��¼����������󽻽��
    }

    return res;
}

bool Scene::SampleLight(float& pdf, HitResult& res) {
    // ���㷢������ܺ�
    float sumArea = 0;
    for (auto& shape : mShapes) {
        if (shape->HasEmission()) {
            sumArea += shape->GetArea();
        }
    }

    // ȡ�����
    float r = Utils::GetUniformRandom(Consts::EPS, sumArea - Consts::EPS);

    float currentArea = 0.0f;
    for (auto& shape : mShapes) {
        if (shape->HasEmission()) {
            currentArea += shape->GetArea();
            if (currentArea >= r) {
                shape->UniformSample(pdf, res);
                pdf = 1.0f / sumArea;
                return true;
            }
        }
    }

    return false;
}


std::vector<Mesh*> Scene::LoadModel(std::string modelPath, Material* material) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(modelPath, aiProcess_Triangulate);

    std::vector<Mesh*> result(0);

    // ���
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ASSIMP IMPORT FAILED" << importer.GetErrorString() << std::endl;
        return result;
    }

    // �������в���
    for (int i = 0; i < scene->mNumMaterials; i++) {

    }

    // ��������mesh
    for (int i = 0; i < scene->mNumMeshes; i++) {
        Mesh* mesh = new Mesh(scene->mMeshes[i], material);
        result.push_back(mesh);
    }

    //delete scene;
    return result;
}
