#ifndef SCENE_H
#define SCENE_H

class Scene {
public:
	static Scene *current;
	virtual void Draw() =0;
	virtual void ProcessIddle() { }
	virtual void ProcessMotion(int x, int y) { }
	virtual void ProcessPassiveMotion(int x, int y) { }
	virtual void ProcessClick(int button, int state, int x, int y) { }
	virtual void ProcessDoubleClick(int x, int y) { }
	virtual void ProcessKey(unsigned char key) { }
	virtual void ProcessSpecialKey(int key) { }
	virtual void MakeCurrent() { current = this; }
};

#endif
