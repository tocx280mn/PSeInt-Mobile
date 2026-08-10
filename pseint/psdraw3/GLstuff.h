#ifdef _FOR_EXPORT
//#	include "GLtoWX.h"
#else
#	ifdef __APPLE__
#		include <OpenGL/gl.h>
#		include <OpenGL/glu.h>
#	else
#		include <GL/gl.h>
#		include <GL/glu.h>
#		ifdef _USE_DF
#			include "gl_lite.h"
#		endif
#	endif
#endif
