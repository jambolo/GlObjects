#if !defined( MIRROR_MIRROR_H_INCLUDED )
#define MIRROR_MIRROR_H_INCLUDED

#pragma once

/** @file *//********************************************************************************************************

                                                       Mirror.h

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/GlObjects/Mirror/Mirror.h#1 $

	$NoKeywords: $

 ********************************************************************************************************************/

#include "Glx/Frame.h"
#include "Glx/Camera.h"
#include "Glx/Texture.h"
#include "Glx/Material.h"

class Vector3;
class Quaternion;

namespace GlObjects
{


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

/// A flat rectangular reflective surface

class Mirror
{
public:

	/// Constructor
	Mirror( Vector3 const & position, Quaternion const & orientation, float w, float h, int tw, int th );
//	Mirror( Glx::Frame const & frame, int tw, int th );

	/// Destructor
	virtual ~Mirror();

	/// Enters the reflection state. Returns @c false if the reflection is aborted because it can not be seen. 
	bool Begin( Glx::Camera const & camera );

	/// Exits the reflection state.
	void End();

	/// Draws the mirror
	void Apply() const;

	Glx::Frame	m_Frame;								///< Mirror's frame

private:

	Glx::Texture *	m_pTexture;							///< Surface texture of the mirror
	Glx::Material *	m_pMaterial;						///< Surface material of the mirror
	float			m_MirrorWidth, m_MirrorHeight;		///< Size of the mirror
	int				m_SavedViewportParameters[ 4 ];

//	// Stencil buffer implementation data
//	Reflection		m_Reflection;						///< The mirror's reflection

	// Render-to-texture implementation data
	bool			m_IsReflecting;						///< True if the camera can see a reflection
};


} // namespace GlObjects



#endif // !defined( MIRROR_MIRROR_H_INCLUDED )
