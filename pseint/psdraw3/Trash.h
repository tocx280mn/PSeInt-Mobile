#ifndef TRASH_H
#define TRASH_H
#include "Textures.h"

class Trash {
	bool m_visible, m_extended;
	Texture m_texture;
	int m_size;
public:
	Trash();
	void Draw();
	void Show() { m_visible = true; }
	void Hide() { m_visible = m_extended = false; }
	void ProcessMotion(int x, int y);
	void ProcessIdle();
	bool IsSelected() const { return m_visible && m_extended; }
	static void Initialize();
};

extern Trash *g_trash;

#endif

