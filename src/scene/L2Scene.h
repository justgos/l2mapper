#pragma once

#include "../_afx.h"

class L2Scene
{
public:
	void Init();

	void preUpdate();
	void update();

	float getVisibleDist() { return _visibleDist; }

	bool injectMouseDown(MyGUI::MouseButton btn);
	bool injectMouseUp(MyGUI::MouseButton btn);
protected:
	AActor *targetActor;
	UBspNode *targetNode;
	UModel *targetModel;

	float _visibleDist;
public:
	L2Scene(void);
	~L2Scene(void);
};

