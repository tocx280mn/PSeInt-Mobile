#ifndef MX_APPLICATION_H
#define MX_APPLICATION_H

#include <wx/app.h>
#include <wx/splash.h>

class mxApplication : public wxApp {
public:
	virtual bool OnInit();
	void RecoverFromError();
};

#endif
