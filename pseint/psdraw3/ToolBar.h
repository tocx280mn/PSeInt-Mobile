#ifndef TOOLBAR_H
#define TOOLBAR_H

class ToolBar {
	int m_selection = -1;
public:
	void SetSelection(int i);
	void Draw();
	static void Initialize();
};

extern ToolBar *g_toolbar;

#endif

