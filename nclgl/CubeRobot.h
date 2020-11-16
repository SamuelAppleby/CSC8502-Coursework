#pragma once
#include "SceneNode.h"

class CubeRobot : public SceneNode {
public:
	CubeRobot(Mesh * mesh);
	~CubeRobot(void) {};
	void Update(float dt) override;
	
protected:
	SceneNode* body;
	SceneNode * head;
	SceneNode * leftArm;
	SceneNode * rightArm;
	SceneNode * hips;
	SceneNode* leftLeg;
	SceneNode* rightLeg;
};
