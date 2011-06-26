/*-----------------------------------------------------------------
LOG
    GEM - Graphics Environment for Multimedia

    Snap a pix of the render buffer and write it to a file

    Copyright (c) 1997-1999 Mark Danks. mark@danks.org
    Copyright (c) G�nther Geiger. geiger@epy.co.at
    Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::f�r::uml�ute. IEM. zmoelnig@iem.kug.ac.at
    For information on usage and redistribution, and for a DISCLAIMER OF ALL
    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.

-----------------------------------------------------------------*/

#ifndef INCLUDE_fux_freeimage_H_
#define INCLUDE_fux_freeimage_H_

#include "Base/GemBase.h"
#include "Base/GemPixUtil.h"
#include "Base/GemPixImageSave.h"

#include "FreeImage.h"

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    fux_freeimage
    
    Writes a pix of the render buffer
    
KEYWORDS
    pix
    
DESCRIPTION

    Inlet for a list - "vert_size"
    Inlet for a list - "vert_pos"

    "file" - filename to write to
    "bang" - do write now
    "auto 0/1" - stop/start writing automatically

    "vert_size" - Set the size of the pix
    "vert_pos" - Set the position of the pix
    
-----------------------------------------------------------------*/
class GEM_EXTERN fux_freeimage : public GemBase
{
    CPPEXTERN_HEADER(fux_freeimage, GemBase)

    public:

        //////////
        // Constructor
    	fux_freeimage(int argc, t_atom *argv);
    	
    protected:
    	
    	//////////
    	// Destructor
    	virtual ~fux_freeimage();

    	//////////
    	// Write to the current filename
    	virtual void	doWrite();

      // check extensions
      virtual bool isRunnable(void);
    	
    	//////////
    	// Do the rendering
    	virtual void 	render(GemState *state);

    	//////////
    	// Clear the dirty flag on the pixBlock
    	virtual void 	postrender(GemState *state) {};

    	//////////
    	// Set the filename and filetype
    	virtual void	fileMess(int argc, t_atom *argv);
 
    	//////////
    	// When a size message is received
    	virtual void	sizeMess(int width, int height);
    	
    	//////////
    	// When a position message is received
    	virtual void	posMess(int x, int y);
    	
    	//////////
    	// Clean up the image
    	void	    	cleanImage();
    	
    	//////////
    	// The original fux_freeimage
    	imageStruct 	*m_originalImage;


	//////////
	// Manual writing
	bool            m_banged;
    	
	//////////
	// Automatic writing
	bool            m_automatic;

	//////////
	// Counter for automatic writing
	int             m_autocount;
    	
    	//////////
	// path to write to
    	char	    	m_pathname[MAXPDSTRING];
    	//////////
	// current file to write to
    	char	    	m_filename[MAXPDSTRING+10];

    	//////////
	// current file to write to
    	int	    	m_filetype; // 0=tiff, [1..6=jpeg]

    	//////////
    	// The x position
    	int     	m_xoff;
    	
    	//////////
    	// The y position
    	int     	m_yoff;
    	
    	//////////
    	// The width
    	int     	m_width;
    	
    	//////////
    	// The height
    	int     	m_height;
    	
    private:
    	
    	//////////
    	// static member functions
    	static void 	fileMessCallback(void *data, t_symbol *s, int argc, t_atom *argv);
    	static void 	autoMessCallback(void *data, t_floatarg on);
    	static void 	bangMessCallback(void *data);
    	static void 	sizeMessCallback(void *data, t_floatarg width, t_floatarg height );
    	static void 	posMessCallback(void *data, t_floatarg x, t_floatarg y);
};

#endif	// for header file
