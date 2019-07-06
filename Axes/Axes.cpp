/** @file *//********************************************************************************************************

                                                       Axes.cpp

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/GlObjects/Axes/Axes.cpp#2 $

	$NoKeywords: $

 ********************************************************************************************************************/

#include "Axes.h"

#include "Glx/Rgba.h"
#include "Math/Vector3.h"

#include <gl/glaux.h>

namespace GlObjects
{


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

///
/// @param	size	Length of each axis

Axes::Axes( float size/* = 1.0f*/ )
	: m_Size( size )
{
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

Axes::~Axes()
{
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

void Axes::Apply() const
{
	glPushMatrix();

	glScalef( m_Size, m_Size, m_Size );

	// X axis

	glColor3fv( Glx::Rgba::RED.m_C );
	glBegin( GL_LINES );
	glVertex3fv( Vector3::Origin().m_V );
	glVertex3fv( Vector3::XAxis().m_V );
	glEnd();
	glPushMatrix();
	glRotatef( 90.0f, 0.0f, 1.0f, 0.0f );
	glTranslatef( 0.0f, 0.0f, 0.9f );
	auxWireCone( 0.05f, 0.1f );
	glPopMatrix();

	// Y axis

	glColor3fv( Glx::Rgba::GREEN.m_C );
	glBegin( GL_LINES );
	glVertex3fv( Vector3::Origin().m_V );
	glVertex3fv( Vector3::YAxis().m_V );
	glEnd();
	glPushMatrix();
	glRotatef( -90.0f, 1.0f, 0.0f, 0.0f );
	glTranslatef( 0.0f, 0.0f, 0.9f );
	auxWireCone( 0.05f, 0.1f );
	glPopMatrix();

	// Z axis

	glColor3fv( Glx::Rgba::BLUE.m_C );
	glBegin( GL_LINES );
	glVertex3fv( Vector3::Origin().m_V );
	glVertex3fv( Vector3::ZAxis().m_V );
	glEnd();
	glPushMatrix();
	glTranslatef( 0.0f, 0.0f, 0.9f );
	auxWireCone( 0.05f, 0.1f );
	glPopMatrix();

	glPopMatrix();
}


} // namespace GlObjects