#if !defined( GLOBJECTS_AXES_H_INCLUDED )
#define GLOBJECTS_AXES_H_INCLUDED

#pragma once

/** @file *//********************************************************************************************************

                                                        Axes.h

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/GlObjects/Axes/Axes.h#2 $

	$NoKeywords: $

 ********************************************************************************************************************/

#pragma once


namespace GlObjects
{


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

/// A class that draws the coordinate system axes

class Axes
{
public:

	/// Constructor
	Axes( float	size = 1.0f );

	/// Destructor
	virtual ~Axes();

	/// Draws the axes.
	void Apply() const;

private:

	float	m_Size;	///< Length of each axis
};


} // namespace GlObjects


#endif // !defined( GLOBJECTS_AXES_H_INCLUDED )
