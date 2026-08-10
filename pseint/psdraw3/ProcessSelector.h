#ifndef PROCESSSELECTOR_H
#define PROCESSSELECTOR_H
#include "Scene.h"

class ProcessSelector : public Scene {
	constexpr static int NO_SELECTION = -1;
	bool m_mouse_down = false, m_dragged = false;
	int m_selection = NO_SELECTION;
	int m_anim_base = 0;
	int m_anim_delta = 0;
	void ClearSelection();
public:
	void MakeCurrent() override;
	void Draw() override;
	void ProcessMotion(int x, int y) override;
	void ProcessPassiveMotion(int x, int y) override;
	void ProcessClick(int button, int state, int x, int y) override;
	void ProcessIddle() override;
	void ProcessKey(unsigned char key) override;
	static void Initialize();
};

extern ProcessSelector *g_process_selector;

#endif

