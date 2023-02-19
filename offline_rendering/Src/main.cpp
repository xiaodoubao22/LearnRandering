#include "Utils.h"
#include "Shape.h"
#include "Triangle.h"
#include "Sphere.h"
#include "Randerer.h"
#include "Scene.h"
#include "Material.h"
#include "MaterialDefuse.h"
#include "MaterialSpecular.h"
#include "MaterialTransparent.h"
#include "MaterialCookTorrance.h"
#include "MaterialFrostedGlass.h"
#include "HitResult.h"
#include "TexureSampler.h"

#include <algorithm>
#include <omp.h> 
#include <vector>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"


glm::vec3 RED(1.0f, 0.5f, 0.5f);
glm::vec3 BLUE(0.5f, 0.5f, 1.0f);
glm::vec3 WHITE(1.0f, 1.0f, 1.0f);
glm::vec3 CYAN(0.0f, 1.0f, 1.0f);
glm::vec3 GRAY(0.5f, 0.5f, 0.5f);
glm::vec3 YELLOW(1.0f, 1.0f, 0.0f);

// ��� SRC �����е����ݵ�ͼ��
inline void imshow(float* SRC)
{
    unsigned char* image = new unsigned char[Consts::WIDTH * Consts::HEIGHT * 3];// ͼ��buffer
    unsigned char* p = image;
    float* S = SRC;    // Դ����

    for (int i = 0; i < Consts::HEIGHT; i++)
    {
        for (int j = 0; j < Consts::WIDTH; j++)
        {
            *p++ = (unsigned char)std::clamp(powf(*S++, Consts::GAMA) * 255.0f, 0.0f, 255.0f);  // R ͨ��
            *p++ = (unsigned char)std::clamp(powf(*S++, Consts::GAMA) * 255.0f, 0.0f, 255.0f);  // G ͨ��
            *p++ = (unsigned char)std::clamp(powf(*S++, Consts::GAMA) * 255.0f, 0.0f, 255.0f);  // B ͨ��
        }
    }

    stbi_write_png("../result/image.png", Consts::WIDTH, Consts::HEIGHT, 3, image, 0);
}

#include "LdsGenerator.h"

int main() {

    Scene* scene = new Scene;

    int width, height, nrChannels;
    unsigned char* data;
    float* texBuffer;
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("D:/LxyLearn/CGnote/PathTracing/models/sphere/chessBoard.jpg", &width, &height, &nrChannels, 0);
    texBuffer = new float[width * height * nrChannels];
    for (int i = 0; i < width * height * nrChannels; i++) {
        texBuffer[i] = (float)data[i] / 255.0f;
    }
    delete data;
    TexureSampler3F* tBoxDefuse = new TexureSampler3F(texBuffer, width, height, nrChannels);
    scene->PushTexure(tBoxDefuse);

    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("D:/LxyLearn/CGnote/PathTracing/models/sphere/world-roughness.png", &width, &height, &nrChannels, 0);
    texBuffer = new float[width * height * nrChannels];
    for (int i = 0; i < width * height * nrChannels; i++) {
        texBuffer[i] = (float)data[i] / 255.0f;
    }
    delete data;
    TexureSampler1F* tWorldRoughness = new TexureSampler1F(texBuffer, width, height, nrChannels);
    scene->PushTexure(tWorldRoughness);

    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("D:/LxyLearn/CGnote/PathTracing/models/sphere/world-roughnessInv.png", &width, &height, &nrChannels, 0);
    texBuffer = new float[width * height * nrChannels];
    for (int i = 0; i < width * height * nrChannels; i++) {
        texBuffer[i] = (float)data[i] / 255.0f;
    }
    delete data;
    TexureSampler1F* tWorldRoughnessInv = new TexureSampler1F(texBuffer, width, height, nrChannels);
    scene->PushTexure(tWorldRoughnessInv);


    MaterialDefuse* whiteEmission = new MaterialDefuse(WHITE);
    whiteEmission->mHasEmission = true;
    whiteEmission->mEmissionRadiance = glm::vec3(25.0f);
    scene->PushMaterial(whiteEmission);

    MaterialSpecular* whiteSpecular = new MaterialSpecular();
    MaterialSpecular* cyanSpecular = new MaterialSpecular(CYAN);
    MaterialSpecular* redSpecular = new MaterialSpecular(RED);
    scene->PushMaterial(whiteSpecular);
    scene->PushMaterial(cyanSpecular);
    scene->PushMaterial(redSpecular);

    MaterialTransparent* whiteTransparent = new MaterialTransparent(1.5f);
    MaterialCookTorrance* GoldGlossy = new MaterialCookTorrance(0.1f, glm::vec3(1.0f, 0.86f, 0.57f));
    MaterialCookTorrance* WhiteGlossy = new MaterialCookTorrance(0.8f, glm::vec3(1.0f));
    MaterialFrostedGlass* FrostedGlass = new MaterialFrostedGlass(0.01f, 1.5f);
    scene->PushMaterial(whiteTransparent);
    scene->PushMaterial(GoldGlossy);
    scene->PushMaterial(WhiteGlossy);
    scene->PushMaterial(FrostedGlass);

    MaterialDefuse* whiteDefuse = new MaterialDefuse(WHITE);
    MaterialDefuse* blueDefuse = new MaterialDefuse(BLUE);
    MaterialDefuse* redDefuse = new MaterialDefuse(RED);
    MaterialDefuse* cyanDefuse = new MaterialDefuse(CYAN);
    MaterialDefuse* yellowDefuse = new MaterialDefuse(YELLOW);
    scene->PushMaterial(whiteDefuse);
    scene->PushMaterial(blueDefuse);
    scene->PushMaterial(redDefuse);
    scene->PushMaterial(cyanDefuse);
    scene->PushMaterial(yellowDefuse);

    
    MaterialDefuse* boxDefuse = new MaterialDefuse(tBoxDefuse);
    MaterialCookTorrance* worldMapGlossy = new MaterialCookTorrance(tWorldRoughness, glm::vec3(1.0f, 0.86f, 0.57f));
    MaterialFrostedGlass* worldMapGlass = new MaterialFrostedGlass(tWorldRoughnessInv, 1.5f);
    scene->PushMaterial(boxDefuse);
    scene->PushMaterial(worldMapGlossy);
    scene->PushMaterial(worldMapGlass);

    std::string pFile = "D:/LxyLearn/CGnote/PathTracing/models/sphere/sphere-cylcoords-1k.obj";
    std::vector<Mesh*> meshes = Scene::LoadModel(pFile, worldMapGlass);
    for (auto mesh : meshes) {
        scene->PushShape(mesh);
    }

    // ������
    scene->PushShape(new Triangle(glm::vec3(0.4, 0.99, 0.4)* Consts::SCALE, glm::vec3(-0.4, 0.99, 0.4)* Consts::SCALE, glm::vec3(-0.4, 0.99, -0.4)* Consts::SCALE, whiteEmission));
    scene->PushShape(new Triangle(glm::vec3(0.4, 0.99, 0.4)* Consts::SCALE, glm::vec3(-0.4, 0.99, -0.4)* Consts::SCALE, glm::vec3(0.4, 0.99, -0.4)* Consts::SCALE, whiteEmission));
    // ��������
    // bottom
    scene->PushShape(new Triangle(glm::vec3(1, -1, 1) * Consts::SCALE, glm::vec3(-1, -1, -1) * Consts::SCALE, glm::vec3(-1, -1, 1) * Consts::SCALE, whiteDefuse));
    scene->PushShape(new Triangle(glm::vec3(1, -1, 1) * Consts::SCALE, glm::vec3(1, -1, -1) * Consts::SCALE, glm::vec3(-1, -1, -1) * Consts::SCALE, whiteDefuse));
    // top
    scene->PushShape(new Triangle(glm::vec3(1, 1, 1) * Consts::SCALE, glm::vec3(-1, 1, 1) * Consts::SCALE, glm::vec3(-1, 1, -1) * Consts::SCALE, whiteDefuse));
    scene->PushShape(new Triangle(glm::vec3(1, 1, 1) * Consts::SCALE, glm::vec3(-1, 1, -1) * Consts::SCALE, glm::vec3(1, 1, -1) * Consts::SCALE, whiteDefuse));
    // back
    //scene->PushShape(new Triangle(glm::vec3(1, -1, -1) * Consts::SCALE, glm::vec3(-1, 1, -1) * Consts::SCALE, glm::vec3(-1, -1, -1) * Consts::SCALE, whiteDefuse));
    //scene->PushShape(new Triangle(glm::vec3(1, -1, -1) * Consts::SCALE, glm::vec3(1, 1, -1) * Consts::SCALE, glm::vec3(-1, 1, -1) * Consts::SCALE, whiteDefuse));
    scene->PushShape(new Triangle(glm::vec3(1, -1, -1) * Consts::SCALE, glm::vec3(-1, 1, -1) * Consts::SCALE, glm::vec3(-1, -1, -1) * Consts::SCALE, 
                                  glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f),
                                  glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f), boxDefuse));
    scene->PushShape(new Triangle(glm::vec3(1, -1, -1) * Consts::SCALE, glm::vec3(1, 1, -1) * Consts::SCALE, glm::vec3(-1, 1, -1) * Consts::SCALE,
                                  glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f),
                                  glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f), boxDefuse));
    // left
    scene->PushShape(new Triangle(glm::vec3(-1, -1, -1) * Consts::SCALE, glm::vec3(-1, 1, 1) * Consts::SCALE, glm::vec3(-1, -1, 1) * Consts::SCALE, blueDefuse));
    scene->PushShape(new Triangle(glm::vec3(-1, -1, -1) * Consts::SCALE, glm::vec3(-1, 1, -1) * Consts::SCALE, glm::vec3(-1, 1, 1) * Consts::SCALE, blueDefuse));
    // right
    scene->PushShape(new Triangle(glm::vec3(1, 1, 1) * Consts::SCALE, glm::vec3(1, -1, -1) * Consts::SCALE, glm::vec3(1, -1, 1) * Consts::SCALE, redDefuse));
    scene->PushShape(new Triangle(glm::vec3(1, -1, -1) * Consts::SCALE, glm::vec3(1, 1, 1) * Consts::SCALE, glm::vec3(1, 1, -1) * Consts::SCALE, redDefuse));
    // yellow
    //scene->PushShape(new Triangle(glm::vec3(0.5, -0.5, -0.99) * Consts::SCALE, glm::vec3(-0.5, 0.5, -0.99) * Consts::SCALE, glm::vec3(-0.5, -0.5, -0.99) * Consts::SCALE, cyanDefuse));
    //scene->PushShape(new Triangle(glm::vec3(0.5, -0.5, -0.99) * Consts::SCALE, glm::vec3(0.5, 0.5, -0.99) * Consts::SCALE, glm::vec3(-0.5, 0.5, -0.99) * Consts::SCALE, cyanDefuse));

    //scene->PushShape(new Triangle(glm::vec3(0.5, -0.5, 0.9) * Consts::SCALE, glm::vec3(-0.5, 0.5, 0.9) * Consts::SCALE, glm::vec3(-0.5, -0.5, 0.9) * Consts::SCALE, FrostedGlass));
    //scene->PushShape(new Triangle(glm::vec3(0.5, -0.5, 0.9) * Consts::SCALE, glm::vec3(0.5, 0.5, 0.9) * Consts::SCALE, glm::vec3(-0.5, 0.5, 0.9) * Consts::SCALE, FrostedGlass));
    //scene->PushShape(new Triangle(glm::vec3(0.5, -0.5, 0.85) * Consts::SCALE, glm::vec3(-0.5, -0.5, 0.85) * Consts::SCALE, glm::vec3(-0.5, 0.5, 0.85) * Consts::SCALE, FrostedGlass));
    //scene->PushShape(new Triangle(glm::vec3(0.5, -0.5, 0.85) * Consts::SCALE, glm::vec3(-0.5, 0.5, 0.85) * Consts::SCALE, glm::vec3(0.5, 0.5, 0.85) * Consts::SCALE, FrostedGlass));

    //scene->PushShape(new Sphere(glm::vec3(-0.75, -0.8, 0.6)* Consts::SCALE, 0.2f * Consts::SCALE, whiteSpecular));
    //scene->PushShape(new Sphere(glm::vec3(-0.3, -0.7, 0.2) * Consts::SCALE, 0.3f * Consts::SCALE, redSpecular));
    //scene->PushShape(new Sphere(glm::vec3(0.4, -0.6, -0.5)* Consts::SCALE, 0.4f * Consts::SCALE, GoldGlossy));
    //scene->PushShape(new Sphere(glm::vec3(0.55, -0.75, 0.7)* Consts::SCALE, 0.25f * Consts::SCALE, FrostedGlass));
    //scene->PushShape(new Sphere(glm::vec3(0.0, -0.6, 0.0) * Consts::SCALE, 0.4f * Consts::SCALE, whiteTransparent));

    //scene->PushShape(new Sphere(glm::vec3(0.0, -0.4, 0.0) * Consts::SCALE, 0.6f * Consts::SCALE, FrostedGlass));

    // ͼ��
    float* image = new float[Consts::WIDTH * Consts::HEIGHT * 3];
    memset(image, 0, sizeof(float) * Consts::WIDTH * Consts::HEIGHT * 3);
    Randerer randerer;
    randerer.Draw(image, *scene);

    imshow(image);

	return 0;

}


int main0() {


    int width, height, nrChannels;
    float* data = stbi_loadf("D:/LxyLearn/CGnote/PathTracing/models/sphere/world-mask.png", &width, &height, &nrChannels, 0);
    TexureSampler3F t0(data, width, height, nrChannels);

    unsigned char* cBuffer = new unsigned char[width * height * nrChannels];
    unsigned char* p = cBuffer;
    float* s = data;
    //std::cout << nrChannels << std::endl;
    for (int i = 0; i < width * height * nrChannels; i++) {
        if ((*s++) < 0.01) {
            *p++ = (unsigned char)(0.5f * 255.0f);
            //*p++ = (unsigned char)(1.0f * 255.0f);
            //*p++ = (unsigned char)(1.0f * 255.0f);
        }
        else {
            *p++ = (unsigned char)(0.05f * 255.0f);
            //*p++ = (unsigned char)(0.6f * 255.0f);
            //*p++ = (unsigned char)(0.6f * 255.0f);
        }
    }

    stbi_write_png("D:/LxyLearn/CGnote/PathTracing/models/sphere/world-mask2.png", width, height, nrChannels, cBuffer, 0);
    return 0;
}


