#include <cstdlib>
#include <cmath>
#include <iostream>
#include <wx/string.h>
#include <wx/image.h>
#include "Textures.h"
#include "Entity.h"
#include "Text.h"
#include "Global.h"

// función basada en código tomado de http://wiki.wxwidgets.org/Using_wxImage_to_load_textures_for_OpenGL
static bool loadImage(GLuint &ID, wxString path, int* imageWidth, int* imageHeight, int* textureWidth, int* textureHeight) {
	
	// check the file exists
	if(!wxFileExists(path)) return false;

	wxImage* img=new wxImage( path );
	(*imageWidth)=img->GetWidth();
	(*imageHeight)=img->GetHeight();
	
	glGenTextures(1,&ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	
	/*
	* Many graphics card require that textures be power of two.
	* Below is a simple implementation, probably not optimal but working.
	* If your texture sizes are not restricted to power of 2s, you can
	* of course adapt the bit below as needed.
	*/
	double power_of_two_that_gives_correct_width=std::log((double)(*imageWidth))/std::log(2.0);
	double power_of_two_that_gives_correct_height=std::log((double)(*imageHeight))/std::log(2.0);
	// check if image dimensions are a power of two
	if( (int)power_of_two_that_gives_correct_width == power_of_two_that_gives_correct_width &&
		(int)power_of_two_that_gives_correct_height == power_of_two_that_gives_correct_height) {
		// note: must make a local copy before passing the data to OpenGL, as GetData() returns RGB 
		// and we want the Alpha channel if it's present. Additionally OpenGL seems to interpret the 
		// data upside-down so we need to compensate for that.
		GLubyte *bitmapData=img->GetData();
		GLubyte *alphaData=img->GetAlpha();
		
		int bytesPerPixel = img->HasAlpha() ?  4 : 3;
		
		int imageSize = (*imageWidth) * (*imageHeight) * bytesPerPixel;
		GLubyte *imageData=new GLubyte[imageSize];
		
		int rev_val=(*imageHeight)-1;
		
		for(int y=0; y<(*imageHeight); y++)
		{
			for(int x=0; x<(*imageWidth); x++)
			{
				imageData[(x+y*(*imageWidth))*bytesPerPixel+0]=
					bitmapData[( x+(rev_val-y)*(*imageWidth))*3];
				
				imageData[(x+y*(*imageWidth))*bytesPerPixel+1]=
					bitmapData[( x+(rev_val-y)*(*imageWidth))*3 + 1];
				
				imageData[(x+y*(*imageWidth))*bytesPerPixel+2]=
					bitmapData[( x+(rev_val-y)*(*imageWidth))*3 + 2];
				
				if(bytesPerPixel==4) imageData[(x+y*(*imageWidth))*bytesPerPixel+3]=
					alphaData[ x+(rev_val-y)*(*imageWidth) ];
			}//next
		}//next
		
		// if yes, everything is fine
		glTexImage2D(GL_TEXTURE_2D,
			0,
			bytesPerPixel,
			*imageWidth,
			*imageHeight,
			0,
			img->HasAlpha() ?  GL_RGBA : GL_RGB,
			GL_UNSIGNED_BYTE,
			imageData);
		
		(*textureWidth)  = (*imageWidth);
		(*textureHeight) = (*imageHeight);
		
		delete [] imageData;
	}
	else // texture is not a power of two. We need to resize it
		{
			
			int newWidth=(int)std::pow( 2.0, (int)(std::ceil(power_of_two_that_gives_correct_width)) );
			int newHeight=(int)std::pow( 2.0, (int)(std::ceil(power_of_two_that_gives_correct_height)) );
			
			//printf("Unsupported image size. Recommand values: %i %i\n",newWidth,newHeight);   
			
			GLubyte	*bitmapData=img->GetData();
			GLubyte        *alphaData=img->GetAlpha();
			
			int old_bytesPerPixel = 3;
			int bytesPerPixel = img->HasAlpha() ?  4 : 3;
			
			int imageSize = newWidth * newHeight * bytesPerPixel;
			GLubyte	*imageData=new GLubyte[imageSize];
			
			int rev_val=(*imageHeight)-1;
			
			for(int y=0; y<newHeight; y++)
			{
				for(int x=0; x<newWidth; x++)
				{
					
					if( x<(*imageWidth) && y<(*imageHeight) ){
						imageData[(x+y*newWidth)*bytesPerPixel+0]=
							bitmapData[( x+(rev_val-y)*(*imageWidth))*old_bytesPerPixel + 0];
						
						imageData[(x+y*newWidth)*bytesPerPixel+1]=
							bitmapData[( x+(rev_val-y)*(*imageWidth))*old_bytesPerPixel + 1];
						
						imageData[(x+y*newWidth)*bytesPerPixel+2]=
							bitmapData[( x+(rev_val-y)*(*imageWidth))*old_bytesPerPixel + 2];
						
						if(bytesPerPixel==4) imageData[(x+y*newWidth)*bytesPerPixel+3]=
							alphaData[ x+(rev_val-y)*(*imageWidth) ];
						
					}
					else
						{
							
							imageData[(x+y*newWidth)*bytesPerPixel+0] = 0;
							imageData[(x+y*newWidth)*bytesPerPixel+1] = 0;
							imageData[(x+y*newWidth)*bytesPerPixel+2] = 0;
							if(bytesPerPixel==4) imageData[(x+y*newWidth)*bytesPerPixel+3] = 0;
						}
					
				}//next
			}//next
			
			
			glTexImage2D(GL_TEXTURE_2D,
				0,
				img->HasAlpha() ?  4 : 3,
				newWidth,
				newHeight,
				0, 
				img->HasAlpha() ?  GL_RGBA : GL_RGB, 
				GL_UNSIGNED_BYTE,
				imageData);
			
			(*textureWidth)=newWidth;
			(*textureHeight)=newHeight;
			
			delete [] imageData;
		}
	
	// set texture parameters as you wish
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
//	
	return true;
	
}

bool Texture::Load () {
	int tex_w=0, tex_h=0; w=h=0;
	if (!loadImage(m_id,m_fname.c_str(), &w, &h, &tex_w, &tex_h)) {
		std::cerr << "ERROR: LOADING TEXTURE: " << m_fname << std::endl;
		return false;
	}
	if (w==0||h==0) {
		std::cerr << "ERROR: EMPTY TEXTURE: " << m_fname << std::endl;
		return false;
	}
	r=double(w)/h;
	max_s=double(w)/tex_w;
	max_t=double(h)/tex_h;
	return true;
}

Texture g_texture_font;

std::set<Texture*> Texture::m_to_load;
bool Texture::LoadTextures() {
# ifdef _USE_DF
	g_texture_font.SetTexture(g_constants.imgs_path+"../font.png");
# else
	g_texture_font.SetTexture(g_constants.imgs_path+"font.png");
# endif
	
	bool all_ok = true;
	for( auto it=m_to_load.begin(); it!=m_to_load.end(); ++it ) { 
		if (!(*it)->Load()) all_ok = false;
	}
	m_to_load.clear();
	return all_ok;
}


