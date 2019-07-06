/** @file *//********************************************************************************************************

                                                  TerrainCamera.cpp

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/GlObjects/TerrainCamera/TerrainCamera.cpp#2 $

	$NoKeywords: $

 ********************************************************************************************************************/

#include "TerrainCamera.h"

#include "Math/Quaternion.h"
#include "Math/Constants.h"
#include "Math/Vector3.h"

namespace GlObjects
{


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

/// @param	angleOfView		Angle of view. Valid range is (0,90).
/// @param	nearDistance	Distance to the near plane
/// @param	farDistance		Distance to the far plane
/// @param	position		Initial position
/// @param	yaw				Initial yaw
/// @param	pitch			Initial pitch
/// @param	roll			Initial roll
///
/// @note	The unrotated position of the camera is: facing -Z, Y is up, X is right. The initial orientation angles
///			are applied in this order: yaw, pitch, roll.

TerrainCamera::TerrainCamera( float				angleOfView,
							  float				nearDistance,
							  float				farDistance,
							  Vector3 const &	position		/*= Vector3::ORIGIN*/,
							  float				yaw				/*= 0.f*/,
							  float				pitch			/*= 0.f*/,
							  float				roll			/*= 0.f*/ )
	: Camera( angleOfView, nearDistance, farDistance, position,
				Quaternion( Vector3::YAxis(), Math::ToRadians( yaw   ) )
			  * Quaternion( Vector3::XAxis(), Math::ToRadians( pitch ) )
			  * Quaternion( Vector3::ZAxis(), Math::ToRadians( roll  ) ) )
{
}


} // namespace GlObjects