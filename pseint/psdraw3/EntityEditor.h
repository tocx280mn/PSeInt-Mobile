#ifndef ENTITYEDITOR_H
#define ENTITYEDITOR_H
#include "Scene.h"

class EntityEditor : public Scene {
public:
	static void Initialize();
	void Draw() override;
	void ProcessIddle() override;
	void ProcessMotion(int x, int y) override;
	void ProcessPassiveMotion(int x, int y) override;
	void ProcessClick(int button, int state, int x, int y) override;
	void ProcessDoubleClick(int x, int y) override;
	void ProcessKey(unsigned char key) override;
	void ProcessSpecialKey(int key) override;
	
private:
};

extern EntityEditor *g_entity_editor;

#endif

