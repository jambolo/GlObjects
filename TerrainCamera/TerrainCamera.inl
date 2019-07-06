#if !defined( GLOBJECTS_TERRAINCAMERA_INL_INCLUDED )
#define GLOBJECTS_TERRAINCAMERA_INL_INCLUDED

#pragma once

/** @file *//********************************************************************************************************

                                                  TerrainCamera.inl

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/GlObjects/TerrainCamera/TerrainCamera.inl#3 $

	$NoKeywords: $

 ********************************************************************************************************************/

#include "TerrainCamera.h"

#include "Math/Vector3.h"

namespace GlObjects
{


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

inline TerrainCamera::~TerrainCamera()
{
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

/// @param	angle	Angle of rotation in degrees.
///
/// @note	Yaw for this camera is a rotation around the @b world Z axis.

inline void TerrainCamera::Yaw( float angle )
{
	//  Un-orient the camera's Z axis to get the world Z axis.
	Vector3 const	worldZAxis	= Vector3( Vector3::ZAxis() ).Rotate( -GetOrientation() );

	Turn( angle, worldZAxis );
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

///
/// @param	angle	Angle of rotation in degrees.

inline void TerrainCamera::Pitch( float angle )
{
	Turn( angle, Vector3::XAxis() );
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

///
/// @param	angle	Angle of rotation in degrees.

inline void TerrainCamera::Roll( float angle )
{
	Turn( angle, -Vector3::ZAxis() );
}


} // namespace GlObjects


#endif // !defined( GLOBJECTS_TERRAINCAMERA_INL_INCLUDED )
