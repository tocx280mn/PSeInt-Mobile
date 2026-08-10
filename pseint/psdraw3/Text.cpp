#define GL_LITE_IMPLEMENTATION // antes de incluir GLstuff
#include "GLstuff.h"
#undef GL_LITE_IMPLEMENTATION // antes de incluir GLstuff
#include <iostream>
#include "Text.h"
#include "Textures.h"
#include "Global.h"

void GetTextSize(const std::string &label, int &w, int &h) {
	w=label.size()*10;
	h=15;
}

#define c2i(ch) ((int)((unsigned char)(ch)))

void DrawTextRaster(const float *color, int x, int y, const char *t) {
	glColor3fv(color);
	glPushMatrix();
	glTranslated(x,y,0);
	if (g_config.big_icons) glScaled(.12,.15,0);
	else                    glScaled(.09,.11,0);
	begin_texto();
	for(int i=0;t[i]!='\0';++i)
		dibujar_caracter(t[i]);
	end_texto();
	glPopMatrix();
}


static double x_texto = 0;

#ifdef _USE_DF

bool CompilerInfo(GLuint id){
  int len; glGetShaderiv(id,GL_INFO_LOG_LENGTH,&len); // cant de caracteres
  if (len){
    char * infoLog=(char *)malloc(sizeof(char)*(len+1));
    glGetShaderInfoLog(id,len+1,nullptr,infoLog);
	std::cout << "ERROR COMPILING SHADER: " << infoLog << std::endl;
    free(infoLog);
  }
  int status; glGetShaderiv(id,GL_COMPILE_STATUS,&status); return status;
}

// imprime la info del linker de la GPU
//bool LinkerInfo(GLuint id){
//  int len; glGetProgramiv(id,GL_INFO_LOG_LENGTH,&len); // cant de caracteres
//  if (len){
//    char * infoLog=(char *)malloc(sizeof(char)*(len+1));
//    glGetProgramInfoLog(id,len+1,nullptr,infoLog);
//    cout << "ERROR LINKING SHADER: " << infoLog << endl;
//    free(infoLog);
//  }
//  int status; glGetProgramiv(id,GL_LINK_STATUS,&status); return status;
//}
GLuint progID;
bool init_shaders() {
#ifdef __APPLE__
	return false;
#else
	static bool inited = false;
	static bool shader_ok = false;
	if (inited) return shader_ok;
	
	inited = true;
	if (!gl_lite_init()) return false;
	
	const char *sf4 = // 4 muestras para anti-aliasing
		"#version 110\n"
		"uniform sampler2D font_texture;\n"
		"void main() {\n"
		"    gl_FragColor.rgb = gl_Color.rgb;\n"
		"    vec2 st = gl_TexCoord[0].st;"
		"    vec2 dx = dFdx(gl_TexCoord[0].st);\n" 
		"    vec2 dy = dFdy(gl_TexCoord[0].st);\n"
		"    float tol = 0.6;\n"
	    "    st+=dx/8.0; st+=3.0*dy/8.0;\n"
	    "    float                   a = texture2D(font_texture, st).a >tol ? 1.0 : 0.0;\n"
	    "    st-=dx/2.0; st-=dy/4.0; a+= texture2D(font_texture, st).a >tol ? 1.0 : 0.0;\n"
	    "    st+=dx/4.0; st-=dy/2.0; a+= texture2D(font_texture, st).a >tol ? 1.0 : 0.0;\n"
	    "    st+=dx/2.0; st+=dy/4.0; a+= texture2D(font_texture, st).a >tol ? 1.0 : 0.0;\n"
		"    gl_FragColor.a = a/4.0;\n"
		"}\n";
	
	GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(f,1,&sf4,nullptr);
	glCompileShader(f);
	shader_ok = CompilerInfo(f);
	if (!shader_ok) return false;
	
	progID = glCreateProgram();
	glAttachShader(progID,f);
	glLinkProgram(progID); 
	
	return shader_ok = true;
#endif
}

#endif


void begin_texto( ) {
	x_texto = 0;
	g_texture_font.Select();
#ifdef _USE_DF
	if (init_shaders()) {
		glUseProgram(progID);
	} else {
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER,0.5);
		glBlendFunc(GL_ONE,GL_ZERO);
	}
#endif
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
}

void end_texto( ) {
	glEnd();
	glDisable(GL_TEXTURE_2D);
# ifdef _USE_DF
	if (init_shaders()) {
		glUseProgram(0);
	} else {
		glPopAttrib();
	}
# endif
}


void dibujar_caracter(const char chr) {
	const double f=1.0/16.0;
	const int w=120, h=150,y0=-25;
	double x0=x_texto;
	unsigned char ch=chr;
	int r=15-ch/16, c=ch%16;
	glTexCoord2f(c*f,r*f);
	glVertex2i(x0,y0);
	glTexCoord2f(c*f+f,r*f);
	glVertex2i(x0+w,y0);
	glTexCoord2f(c*f+f,r*f+f);
	glVertex2i(x0+w,y0+h);
	glTexCoord2f(c*f,r*f+f);
	glVertex2i(x0,y0+h);
	x_texto += 800.0/7.0;
}

double get_char_width( ) {
	return (800.0/7.0)*(g_config.big_icons ? .12 : .09);
}

