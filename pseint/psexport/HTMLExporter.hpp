#ifndef HTML_EXPORTER_HPP
#define HTML_EXPORTER_HPP
#include "JSExporter.hpp"

class HTMLExporter : public JSExporter {
public:
	HTMLExporter();
	void translate(t_output &out, Programa &prog) override;
};

#endif
