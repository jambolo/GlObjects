#if !defined( SKY_SKYBOX_H_INCLUDED )
#define SKY_SKYBOX_H_INCLUDED

#pragma once

/** @file *//********************************************************************************************************

                                                       SkyBox.h

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/GlObjects/SkyBox/SkyBox.h#4 $

	$NoKeywords: $

 ********************************************************************************************************************/


#include "Glx/Texture.h"
#include "Glx/Camera.h"

namespace GlObjects
{


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

class SkyBox
{
public:

	enum Faces
	{
		FACE_FRONT		= 0x01,		///< -Z
		FACE_BACK		= 0x02,		///< +Z
		FACE_RIGHT		= 0x04,		///< +X
		FACE_LEFT		= 0x08,		///< -X
		FACE_UP			= 0x10,		///< +Y
		FACE_DOWN		= 0x20,		///< -Y
									
		FACE_ALL_FACES	= 0x3f,		
		FACE_TOP_FACES	= 0x1f,		///< all except FACE_DOWN
		FACE_SIDE_FACES	= 0x0f,		///< all except FACE_UP and FACE_DOWN

		NUM_FACES		= 6,
	};

	SkyBox( char const * filename, unsigned faceMask = FACE_ALL_FACES );
	~SkyBox();

	/// Draws the skybox
	void Apply( Glx::Camera const & camera, bool bTestZ = false );

	/// Draws the skybox
	void Apply( Vector3 const & vp, float r, bool bTestZ = false );

private:

	Glx::Texture *	m_aTextures[ NUM_FACES ];
};


} // namespace GlObjects

#endif // !defined( SKY_SKYBOX_H_INCLUDED )
