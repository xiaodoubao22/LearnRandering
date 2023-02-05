#include "BvhTree.h"
#include <algorithm>
#include <queue>

BvhTree::BvhTree() {

}

BvhTree::BvhTree(std::vector<Shape*>& shapes) {
	Build(shapes);
}

BvhTree::~BvhTree() {

}





void BvhTree::Build(std::vector<Shape*>& shapes) {
	mRoot = TraversalBuild(shapes);

//#define SAVE_BOUNDING_BOX
#ifdef SAVE_BOUNDING_BOX
	FILE* fpLog = nullptr;
	fpLog = fopen("../scripts/Boxes10.txt", "wb");

	std::queue<BvhNode*> queue;
	queue.push(mRoot);
	int layer = 0;
	int targetLayer = 10;
	while (!queue.empty()) {
		int size = queue.size();
		for (int i = 0; i < size; i++) {
			BvhNode* node = queue.front();
			queue.pop();
			if (node != nullptr) {	// ����ڵ�
				if (layer == targetLayer) {
					fprintf_s(fpLog, "%.6f %.6f %.6f %.6f %.6f %.6f\n",
						node->bound->mMin.x, node->bound->mMin.y, node->bound->mMin.z,
						node->bound->mMax.x, node->bound->mMax.y, node->bound->mMax.z);
				}
				queue.push(node->left);
				queue.push(node->right);
			}
		}
		layer++;
	}
	fclose(fpLog);
#endif // SAVE_BOUNDING_BOX

}

HitResult BvhTree::GetIntersect(Ray& ray) {

	return TraversalIntersect(ray, mRoot);
}

HitResult BvhTree::TraversalIntersect(Ray& ray, BvhNode* node) {
	HitResult res;
	if (node == nullptr) {
		return res;
	}

	// δ���б��ڵ�İ�Χ��
	if (node->bound->Intersect(ray) == false) {
		return res;
	}

	// ���б��ڵ��Χ�в�������
	// ���ڵ��Ǹ��ڵ�
	if (node->left == nullptr && node->right == nullptr && node->shape != nullptr) {
		
		res = node->shape->Intersect(ray);
		//std::cout << " hit bvh" << res.isHit << std::endl;
		return res;
	}

	// �Ǹ��ڵ�
	HitResult res1 = TraversalIntersect(ray, node->left);
	HitResult res2 = TraversalIntersect(ray, node->right);

	// ѡ�������
	return res1.distance < res2.distance ? res1 : res2;
}

BvhNode* BvhTree::TraversalBuild(std::vector<Shape*>& shapes) {
	if (shapes.size() == 0) return nullptr;

	BvhNode* node = new BvhNode;

	BoundingBox* nodeBound = new BoundingBox;
	for (auto shape : shapes) {
		nodeBound->Union(*shape->GetBoundingBox());
	}

	if (shapes.size() == 1) {
		// Ҷ�ӽڵ�
		node->bound = nodeBound;
		node->left = nullptr;
		node->right = nullptr;
		node->shape = shapes[0];
	}
	else if (shapes.size() == 2) {
		node->bound = nodeBound;
		node->left = TraversalBuild(std::vector<Shape*>(1, shapes[0]));
		node->right = TraversalBuild(std::vector<Shape*>(1, shapes[1]));
		node->shape = nullptr;
	}
	else {
		// �������ĵİ�Χ��
		BoundingBox centerBound;
		for (auto shape : shapes) {
			centerBound.BoundPoint(shape->GetCenter());
		}

		// ѡ���������
		int maxAxis = centerBound.GetMaxAxis();
		std::sort(shapes.begin(), shapes.end(), 
			[=](Shape* f1, Shape* f2) {
			return f1->GetCenter()[maxAxis] < f2->GetCenter()[maxAxis];
			}
		);

		std::vector<Shape*>::iterator beginIt = shapes.begin();
		std::vector<Shape*>::iterator middleIt = shapes.begin() + shapes.size() / 2;
		std::vector<Shape*>::iterator endIt = shapes.end();

		std::vector<Shape*> leftShapes(beginIt, middleIt);
		std::vector<Shape*> rightShapes(middleIt, endIt);

		node->bound = nodeBound;
		node->left = TraversalBuild(leftShapes);
		node->right = TraversalBuild(rightShapes);
		node->shape = nullptr;
	}

	return node;
}