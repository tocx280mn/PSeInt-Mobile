#ifndef STATUSBAR_H
#define STATUSBAR_H
#include <string>

class StatusBar {
	std::string status_text; // texto para la barra de estado
	const float *status_color = nullptr; // color del texto para la barra de estado
public:
	static void Initialize();
	void Clear();
	bool IsEmpty() const { return status_color==nullptr; }
	void Set(const std::string &text);
	void Set(const float *color, const std::string &text);
	void Draw() const;
};

extern StatusBar *g_status_bar;

#endif

