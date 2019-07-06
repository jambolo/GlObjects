#if !defined( MIRROR_REFLECTION_H_INCLUDED )
#define MIRROR_REFLECTION_H_INCLUDED

#pragma once

/** @file *//********************************************************************************************************

                                                     Reflection.h

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/GlObjects/Mirror/Reflection.h#1 $

	$NoKeywords: $

 ********************************************************************************************************************/

#include "Math/Plane.h"
#include "Glx/Camera.h"

class Vector3;

namespace GlObjects
{

/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

// A class for drawing a reflection of a scene

class Reflection
{
public:

	/// Constructor
	Reflection( Vector3 const & position, Vector3 const & normal );

	/// Destructor
	virtual ~Reflection();

	/// Enters the reflection state. Returns @c false if the reflection is aborted because it can not be seen. 
	bool Begin( Glx::Camera const & camera );

	/// Exits the reflection state
	void End();

	Plane	m_Plane;			///< Reflection plane

private:

	bool	m_IsReflecting;		///< @c true if the camera can see the reflection
};


} // namespace GlObjects


#endif // !defined( MIRROR_REFLECTION_H_INCLUDED )
