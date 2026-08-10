#ifndef MX_APPLICATION_H
#define MX_APPLICATION_H

#include <wx/app.h>
#include "Package.h"

extern Package pack;


class mxApplication : public wxApp {
public:
	virtual bool OnInit();
};

#endif
