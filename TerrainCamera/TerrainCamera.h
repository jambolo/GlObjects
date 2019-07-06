#if !defined( GLOBJECTS_TERRAINCAMERA_H_INCLUDED )
#define GLOBJECTS_TERRAINCAMERA_H_INCLUDED

#pragma once

/** @file *//********************************************************************************************************

                                                   TerrainCamera.h

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/GlObjects/TerrainCamera/TerrainCamera.h#3 $

	$NoKeywords: $

 ********************************************************************************************************************/


#include "Glx/Camera.h"
#include "Math/Vector3.h"

namespace GlObjects
{


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

class TerrainCamera : public Glx::Camera
{
public:

	/// Constructor
	TerrainCamera( float angleOfView,
				   float nearDistance,
				   float farDistance,
				   Vector3 const & position		= Vector3::Origin(),
				   float yaw					= 0.f,
				   float pitch					= 0.f,
				   float roll					= 0.f );

	/// Destructor
	virtual ~TerrainCamera();

	/// @name	Camera Overrides
	//@{
	//	void Reshape( GLint w, GLint h ) const;
	//	void Look() const;
	//	void SetPosition( Vector3 const & position );
	//	Vector3 const & GetPosition() const;
	//	void SetOrientation( Quaternion const & orientation );
	//	Quaternion const & GetOrientation() const;
	//	void SetNearDistance( float nearDistance );
	//	float GetNearDistance() const;
	//	void SetFarDistance( float farDistance );
	//	float GetFarDistance() const;
	//	void Turn( GLfloat angle, Vector3 const & axis );	// Note: angle is in degrees
	//	void Turn( Quaternion const & rotation );
	//	void Move( Vector3 const & distance );
	//	Vector3 GetDirection() const;
	//	Vector3 GetUp() const;
	//	Vector3 GetRight() const;
	//@}

	/// Turns the camera right or left
	void Yaw( float angle );

	/// Pitches the camera up or down
	void Pitch( float angle );

	/// Rolls the camera CW or CCW
	void Roll( float angle );
};

} // namespace GlObjects


#include "TerrainCamera.inl"


#endif // !defined( GLOBJECTS_TERRAINCAMERA_H_INCLUDED )
