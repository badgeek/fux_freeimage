////////////////////////////////////////////////////////
//
// GEM - Graphics Environment for Multimedia
//
// zmoelnig@iem.kug.ac.at
//
// Implementation file
//
//    Copyright (c) 1997-1999 Mark Danks.
//    Copyright (c) Günther Geiger.
//    Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::für::umläute. IEM
//    Copyright (c) 2002 James Tittle & Chris Clepper
//    For information on usage and redistribution, and for a DISCLAIMER OF ALL
//    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.
//
/////////////////////////////////////////////////////////


#include "fux_freeimage.h"

#include "Base/GemMan.h"
#include "Base/GemCache.h"

#include <stdio.h>
#include <string.h>

CPPEXTERN_NEW_WITH_GIMME(fux_freeimage)

  /////////////////////////////////////////////////////////
//
// fux_freeimage
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
fux_freeimage :: fux_freeimage(int argc, t_atom *argv)
  : m_originalImage(NULL)
{
  m_xoff = m_yoff = 0;
  m_width = m_height = 0;
  if (argc == 4) {
    m_xoff = (int)atom_getfloat(&argv[0]);
    m_yoff = (int)atom_getfloat(&argv[1]);
    m_width = (int)atom_getfloat(&argv[2]);
    m_height = (int)atom_getfloat(&argv[3]);
  } else if (argc == 2) {
    m_width = (int)atom_getfloat(&argv[0]);
    m_height = (int)atom_getfloat(&argv[1]);
  } else if (argc != 0){
    error("needs 0, 2, or 4 values");
    m_xoff = m_yoff = 0;
    m_width = m_height = 128;
  }

  inlet_new(this->x_obj, &this->x_obj->ob_pd, gensym("list"), gensym("vert_pos"));
  inlet_new(this->x_obj, &this->x_obj->ob_pd, gensym("list"), gensym("vert_size"));

  m_automatic = false;
  m_autocount = 0;
  m_filetype=0;
  snprintf(m_pathname, MAXPDSTRING, "gem");

  m_banged = false;


  m_originalImage = new imageStruct();
  m_originalImage->xsize=m_width;
  m_originalImage->ysize=m_height;
  m_originalImage->setCsizeByFormat(GL_RGBA);
  m_originalImage->allocate();
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
fux_freeimage :: ~fux_freeimage()
{
  cleanImage();
}


/////////////////////////////////////////////////////////
// extension checks
//
/////////////////////////////////////////////////////////
bool fux_freeimage :: isRunnable(void) {
  if(GLEW_VERSION_1_1 || GLEW_EXT_texture_object)
    return true;

  error("your system lacks texture support");
  return false;
}


/////////////////////////////////////////////////////////
// writeMess
//
/////////////////////////////////////////////////////////
void fux_freeimage :: doWrite()
{
  int width  = m_width;
  int height = m_height;

  GemMan::getDimen(((m_width >0)?NULL:&width ),
		   ((m_height>0)?NULL:&height));

  m_originalImage->xsize = width;
  m_originalImage->ysize = height;

#ifndef __APPLE__
  m_originalImage->setCsizeByFormat(GL_RGB);
#else
  m_originalImage->setCsizeByFormat(GL_RGBA);
#endif /* APPLE */

  m_originalImage->reallocate();

  /* the orientation is always correct, since we get it from openGL */
  /* if we do need flipping, this must be handled in mem2image() */
  m_originalImage->upsidedown=false;

  //glPixelStorei(GL_PACK_ALIGNMENT, 4);
  //glPixelStorei(GL_PACK_ROW_LENGTH, 0);
  //glPixelStorei(GL_PACK_SKIP_ROWS, 0);
  //glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

  glReadPixels(m_xoff, m_yoff, width, height, GL_BGR, GL_UNSIGNED_BYTE, m_originalImage->data);

#if 0 // asynchronous texture fetching idea sketch
/* Enable AGP storage hints */
	glPixelStorei( GL_UNPACK_CLIENT_STORAGE_APPLE, 1 );
	glTextureRangeAPPLE(...);
	glTexParameteri(..., GL_TEXTURE_STORAGE_HINT_APPLE, GL_STORAGE_SHARED_APPLE );
	
	/* Copy from Frame Buffer */
	glCopyTexSubImage2d(...);
	
	/* Flush into AGP */
	glFlush(...);
	
	/* Pull out of AGP */
	glGetTexImage(...);
#endif

  FIBITMAP *GLITCH_FUCK = FreeImage_ConvertFromRawBits((BYTE*)m_originalImage->data, m_width, m_height, 3 * m_width, 24, 0, 0, 0, false);
  FreeImage_Save(FIF_JPEG, GLITCH_FUCK, "/Volumes/Users/Public/Kinect/pdkinect.jpg", JPEG_QUALITYSUPERB);
  FreeImage_Unload(GLITCH_FUCK);	    

  //mem2image(m_originalImage, m_filename, m_filetype);
}

/////////////////////////////////////////////////////////
// render
//
/////////////////////////////////////////////////////////
void fux_freeimage :: render(GemState *state)
{
  if (m_automatic || m_banged) {
    char *extension;
    if (m_filetype<0)m_filetype=0;
    if (m_filetype==0) {
      extension=(char*)"tif";
    } else {
      extension=(char*)"jpg";
    }
    
    snprintf(m_filename, (size_t)(MAXPDSTRING+10), "%s%05d.%s", m_pathname, m_autocount, extension);
    
    m_autocount++;
    m_banged = false;
    doWrite();
  }
}


/////////////////////////////////////////////////////////
// sizeMess
//
/////////////////////////////////////////////////////////
void fux_freeimage :: sizeMess(int width, int height)
{
  m_width = width;
  m_height = height;
}

/////////////////////////////////////////////////////////
// posMess
//
/////////////////////////////////////////////////////////
void fux_freeimage :: posMess(int x, int y)
{
  m_xoff = x;
  m_yoff = y;
}

void fux_freeimage :: fileMess(int argc, t_atom *argv)
{
  char *extension = (char*)".tif";
  if (argc) {
    if (argv->a_type == A_SYMBOL) {
      atom_string(argv++, m_pathname, MAXPDSTRING);
      argc--;
      snprintf(m_filename, (size_t)(MAXPDSTRING+10), "%s.%s", m_pathname, extension);
    }
    if (argc>0)
      m_filetype = atom_getint(argv);
  }

  m_autocount = 0;
}

/////////////////////////////////////////////////////////
// cleanImage
//
/////////////////////////////////////////////////////////
void fux_freeimage :: cleanImage()
{
  // release previous data
  if (m_originalImage)
    {
      delete m_originalImage;
      m_originalImage = NULL;
    }
}

/////////////////////////////////////////////////////////
// static member functions
//
/////////////////////////////////////////////////////////
void fux_freeimage :: obj_setupCallback(t_class *classPtr)
{
  class_addmethod(classPtr, (t_method)&fux_freeimage::fileMessCallback,
		  gensym("file"), A_GIMME, A_NULL);
  class_addmethod(classPtr, (t_method)&fux_freeimage::autoMessCallback,
		  gensym("auto"), A_FLOAT, A_NULL);
  class_addbang(classPtr, (t_method)&fux_freeimage::bangMessCallback);

  class_addmethod(classPtr, (t_method)&fux_freeimage::sizeMessCallback,
		  gensym("vert_size"), A_FLOAT, A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&fux_freeimage::posMessCallback,
		  gensym("vert_pos"), A_FLOAT, A_FLOAT, A_NULL);
}

void fux_freeimage :: fileMessCallback(void *data, t_symbol *s, int argc, t_atom *argv)
{
  GetMyClass(data)->fileMess(argc, argv);
}
void fux_freeimage :: autoMessCallback(void *data, t_floatarg on)
{
  GetMyClass(data)->m_automatic=(on!=0);
}
void fux_freeimage :: bangMessCallback(void *data)
{
  GetMyClass(data)->m_banged=true;
}

void fux_freeimage :: sizeMessCallback(void *data, t_floatarg width, t_floatarg height)
{
  GetMyClass(data)->sizeMess((int)width, (int)height);
}
void fux_freeimage :: posMessCallback(void *data, t_floatarg x, t_floatarg y)
{
  GetMyClass(data)->posMess((int)x, (int)y);
}
