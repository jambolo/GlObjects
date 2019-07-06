/** @file *//********************************************************************************************************

                                                       main.cpp

						                    Copyright 2003, John J. Bolton
	--------------------------------------------------------------------------------------------------------------

	$Header: //depot/GlObjects/Mirror/Test/main.cpp#1 $

	$NoKeywords: $

 ********************************************************************************************************************/

#define USING_REFLECTION

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <mmsystem.h>

#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>

#if defined( USING_REFLECTION )
#include "../Reflection.h"
#else // defined( USING_REFLECTION )
#include "../Mirror.h"
#endif // defined( USING_REFLECTION )

#include "GlObjects/SkyBox/SkyBox.h"
#include "GlObjects/TextureLoader/TextureLoader.h"
#include "Glx/Glx.h"
#include "Math/Matrix33.h"
#include "Math/Matrix44.h"
#include "Math/Vector3.h"
#include "Misc/Etc.h"
#include "Misc/Max.h"
#include "Misc/SafeStr.h"
#include "TgaFile/TgaFile.h"
#include "Wglx/Wglx.h"
#include "Wx/Wx.h"

#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <cmath>


static LRESULT CALLBACK WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
static void InitializeRendering();
static void Display();
static void Reshape( int w, int h );
static bool Update( HWND hWnd );

static void DrawScene( bool bReflectionPass );

static void DrawHud();

static char						s_aAppName[]			= "Reflection";
static char						s_aTitleBar[]			= "Reflection";

static Glx::Lighting *			s_pLighting				= 0;
static Glx::DirectionalLight *	s_pDirectionalLight		= 0;

static Glx::Camera *			s_pCamera				= 0;
static float					s_CameraSpeed			= 1.0f;

static WGlx::Font *				s_pFont;

static float const	MIRROR_X	= 10.0f;
static float const	MIRROR_Y	= 10.0f;
static float const	MIRROR_Z	= 10.0f;
static float const	MIRROR_W	= 20.0f;
static float const	MIRROR_H	= 20.0f;

#if defined( USING_REFLECTION )
static GlObjects::Reflection *	s_pReflection				= 0;
#else // defined( USING_REFLECTION )
static GlObjects::Mirror *		s_pMirror					= 0;
#endif // defined( USING_REFLECTION )

static Glx::Texture *			s_pReflectionTexture		= 0;
static Glx::Material *			s_pReflectionMaterial		= 0;
static Quaternion				s_ReflectionOrientation		= Quaternion::Identity();

static GlObjects::SkyBox *		s_pSky						= 0;

static Glx::Material *			s_pTorusMaterial			= 0;
static Quaternion				s_TorusOrientation			= Quaternion::Identity();

static Glx::Material *			s_pCubeMaterial				= 0;
static Quaternion				s_CubeOrientation			= Quaternion::Identity();

static Glx::Material *			s_pTetrahedronMaterial		= 0;
static Quaternion				s_TetrahedronOrientation	= Quaternion::Identity();

static Glx::Material *			s_pSphereMaterial			= 0;

static float					s_Reflectivity				= 0.5f;

static GlObjects::TextureLoader	s_TextureLoader;

/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPreviousInst, LPSTR lpszCmdLine, int nCmdShow )
{
	if ( Wx::RegisterWindowClass(	CS_OWNDC,
									( WNDPROC )WindowProc,
									hInstance,
									s_aAppName,
									LoadCursor( NULL, IDC_ARROW ) ) == NULL )
	{
		MessageBox( NULL, "Wx::RegisterWindowClass() failed.", "Error", MB_OK );
		exit( 1 );
	}

	HWND hWnd = CreateWindowEx( 0,
								s_aAppName,
								s_aTitleBar,
								WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
								0, 0, 800, 600,
								NULL,
								NULL,
								hInstance,
								NULL );

	if ( hWnd == NULL )
	{
		MessageBox( NULL, "CreateWindowEx() failed.", "Error", MB_OK );
		exit( 1 );
	}

//	WGlx::ChangeDisplayMode( 800, 600, 32 );

	HDC const	hDC	= GetDC( hWnd );
	int			rv;

	WGlx::SetPixelFormat( hDC, 0, true );

	{
		WGlx::CurrentRenderingContext	rc( hDC );	// Current rendering context

		InitializeRendering();

		s_pCamera	= new Glx::Camera( 60.0f, 1.0f, 1000.0f, Vector3( 0.0f, 0.0f, 30.0f ) );
		if ( !s_pCamera ) exit( 1 );

		Reshape( 800, 600 );

		s_pLighting			= new Glx::Lighting( Glx::Rgba( 0.2f, 0.2f, 0.3f ) );
		if ( !s_pLighting ) exit( 1 );
		s_pDirectionalLight	= new Glx::DirectionalLight( GL_LIGHT0, Vector3( -0.6f, .4f, -0.7f ).Normalize(), Glx::Rgba( 0.8f, 0.8f, 0.7f ) );
		if ( !s_pDirectionalLight ) exit( 1 );

		{
#if defined( USING_REFLECTION )

			s_pReflection = new GlObjects::Reflection( Vector3( MIRROR_X, MIRROR_Y, MIRROR_Z ), Vector3::ZAxis() );
			if ( !s_pReflection ) exit( 1 );

#else // defined( USING_REFLECTION )

			s_pMirror = new GlObjects::Mirror( Vector3( MIRROR_X, MIRROR_Y, MIRROR_Z ), Quaternion::Identity()/*( Math::PI_OVER_2, 0, 0 )*/, MIRROR_W, MIRROR_H, 256, 256 );
			if ( !s_pMirror ) exit( 1 );

#endif // defined( USING_REFLECTION )

			// Create the reflection texture

			s_pReflectionTexture		= s_TextureLoader.Load( "res/water2_256.tga", GL_CLAMP ).release();
			if ( !s_pReflectionTexture ) exit( 1 );

			s_pReflectionMaterial = new Glx::Material( s_pReflectionTexture,
													   GL_MODULATE,
													   Glx::Rgba( 1.0f, 1.0f, 1.0f, 1.0f - s_Reflectivity ) );
			if ( !s_pReflectionMaterial ) exit( 1 );
		}

		{
			s_TorusOrientation			= Quaternion::Identity();

			s_pTorusMaterial = new Glx::Material( Glx::Rgba::RED );
			if ( !s_pTorusMaterial ) exit( 1 );
		}

		{
			s_CubeOrientation			= Quaternion::Identity();

			s_pCubeMaterial = new Glx::Material( Glx::Rgba::GREEN );
			if ( !s_pCubeMaterial ) exit( 1 );
		}

		{
			s_TetrahedronOrientation	= Quaternion::Identity();

			s_pTetrahedronMaterial = new Glx::Material( Glx::Rgba::BLUE );
			if ( !s_pTetrahedronMaterial ) exit( 1 );
		}

		{
			s_pSphereMaterial = new Glx::Material( Glx::Rgba::YELLOW );
			if ( !s_pSphereMaterial ) exit( 1 );
		}

		s_pFont = new WGlx::Font( hDC );
		if ( !s_pFont ) exit( 1 );

		s_pSky = new GlObjects::SkyBox( "res/Skybox" );
		if ( !s_pSky ) exit( 1 );

		ShowWindow( hWnd, nCmdShow );

		rv = Wx::MessageLoop( hWnd, Update );

		delete s_pSky;
		delete s_pFont;
		delete s_pReflectionMaterial;
#if defined( USING_REFLECTION )
		delete s_pReflection;
#else // defined( USING_REFLECTION )
		delete s_pMirror;
#endif // defined( USING_REFLECTION )
		delete s_pDirectionalLight;
		delete s_pLighting;
		delete s_pCamera;
	}

	ReleaseDC( hWnd, hDC );

	DestroyWindow( hWnd );

	return rv;
}



/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

static bool Update( HWND hWnd )
{
	static DWORD const	startTime	= timeGetTime();
	float const			elapsedTime	= ( timeGetTime() - startTime ) / 1000.0f;

	s_ReflectionOrientation		=   Quaternion( Vector3::XAxis(), (float)Math::TWO_PI /   97.0f * elapsedTime )
								  * Quaternion( Vector3::YAxis(), (float)Math::TWO_PI / -101.0f * elapsedTime )
								  * Quaternion( Vector3::ZAxis(), (float)Math::TWO_PI /  103.0f * elapsedTime );

#if defined( USING_REFLECTION )

	Vector3 const	reflectionNormal	= Vector3( Vector3::ZAxis() ).Rotate( s_ReflectionOrientation );
	s_pReflection->m_Plane = Plane( reflectionNormal, Dot( reflectionNormal, Vector3( MIRROR_X, MIRROR_Y, MIRROR_Z ) ) );

#else // defined( USING_REFLECTION )

	s_pMirror->m_Frame.SetRotation( s_ReflectionOrientation );

#endif // defined( USING_REFLECTION )

	
	s_TorusOrientation			=   Quaternion( Vector3::XAxis(), (float)Math::TWO_PI / -53.0f * elapsedTime )
								  * Quaternion( Vector3::YAxis(), (float)Math::TWO_PI /  59.0f * elapsedTime )
								  * Quaternion( Vector3::ZAxis(), (float)Math::TWO_PI / -61.0f * elapsedTime );
	s_CubeOrientation			=   Quaternion( Vector3::XAxis(), (float)Math::TWO_PI /  67.0f * elapsedTime )
								  * Quaternion( Vector3::YAxis(), (float)Math::TWO_PI / -71.0f * elapsedTime )
								  * Quaternion( Vector3::ZAxis(), (float)Math::TWO_PI /  73.0f * elapsedTime );
	s_TetrahedronOrientation	=   Quaternion( Vector3::XAxis(), (float)Math::TWO_PI / -79.0f * elapsedTime )
								  * Quaternion( Vector3::YAxis(), (float)Math::TWO_PI /  83.0f * elapsedTime )
								  * Quaternion( Vector3::ZAxis(), (float)Math::TWO_PI / -89.0f * elapsedTime );

//	std::ostringstream	msg;
//
//	msg << elapsedTime << "   ";
//	msg << r.m_X << " " << r.m_Y << " " << r.m_Z << " " << r.m_W << std::endl;
//
//	OutputDebugString( msg.str().c_str() );

	Display();

	return true;
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

static LRESULT CALLBACK WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{ 
	switch( uMsg )
	{
	case WM_SIZE:
		Reshape( LOWORD( lParam ), HIWORD( lParam ) );
		PostMessage( hWnd, WM_PAINT, 0, 0 );
		return 0;

	case WM_CHAR:
	{
		switch ( wParam )
		{
		case VK_ESCAPE:	// Quit
			PostQuitMessage( 0 );
			break;

		case ' ':	// Forward
			s_pCamera->Move( -Vector3::ZAxis() * s_CameraSpeed );
			break;

		case 's':	// Backwards
			s_pCamera->Move( Vector3::ZAxis() * s_CameraSpeed );
			break;

		case 'd':	// Strafe right
			s_pCamera->Move( Vector3::XAxis() * s_CameraSpeed );
			break;

		case 'a':	// Strafe left
			s_pCamera->Move( -Vector3::XAxis() * s_CameraSpeed );
			break;

		case 'w':	// Strafe up
			s_pCamera->Move( Vector3::YAxis() * s_CameraSpeed );
			break;

		case 'x':	// Strafe down
			s_pCamera->Move( -Vector3::YAxis() * s_CameraSpeed );
			break;

		case '1':	// Decrease reflectivity
			s_Reflectivity = limit( 0.0f, s_Reflectivity - .1f, 1.0f );
			s_pReflectionMaterial->SetColor( Glx::Rgba( 1.0f, 1.0f, 1.0f, 1.0f - s_Reflectivity ) );
			break;

		case '2':	// Increase reflectivity
			s_Reflectivity = limit( 0.0f, s_Reflectivity + .1f, 1.0f );
			s_pReflectionMaterial->SetColor( Glx::Rgba( 1.0f, 1.0f, 1.0f, 1.0f - s_Reflectivity ) );
			break;
		}
		return 0;
	}

	case WM_KEYDOWN:
	{
		switch ( wParam )
		{
		case VK_UP:
			s_pCamera->Turn( 5.0f, Vector3::XAxis() );
			break;

		case VK_DOWN:
			s_pCamera->Turn( -5.0f, Vector3::XAxis() );
			break;

		case VK_LEFT:
			s_pCamera->Turn( 5.0f, Vector3::YAxis() );
			break;

		case VK_RIGHT:
			s_pCamera->Turn( -5.0f, Vector3::YAxis() );
			break;

		case VK_PRIOR:
			s_pCamera->Turn( 5.0f, -Vector3::ZAxis() );
			break;

		case VK_INSERT:
			s_pCamera->Turn( -5.0f, -Vector3::ZAxis() );
			break;
		}
		return 0;
	}

	case WM_CLOSE:
		PostQuitMessage( 0 );
		return 0;
	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam ); 
} 


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

static void InitializeRendering()
{
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

static void Display()
{
	Glx::Enable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	Glx::Enable( GL_DEPTH_TEST );
	glDepthMask( GL_TRUE );
	glClear( GL_DEPTH_BUFFER_BIT );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	// Place the camera

	s_pCamera->Look();

#if defined( USING_REFLECTION )

	// Create the reflection in the frame buffer

	if ( s_pReflection->Begin( *s_pCamera ) )
	{
		DrawScene( true );
		s_pReflection->End();
	}

#else // defined( USING_REFLECTION )

	// Create the reflection texture

	if ( s_pMirror->Begin( *s_pCamera ) )
	{
		DrawScene( true );
		s_pMirror->End();
	}

#endif // defined( USING_REFLECTION )

	// Draw the scene

	DrawScene( false );

	// Draw the HUD

	DrawHud();
	
	// Display the scene

	glFlush();
	SwapBuffers( wglGetCurrentDC() );

	// Report any errors

	Glx::ReportAnyErrors();
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

static void Reshape( int w, int h )
{
	if ( s_pCamera )
	{
		glViewport( 0, 0, GLsizei( w ), GLsizei( h ) );
		s_pCamera->Reshape( w, h );
	}
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

static void DrawScene( bool bReflectionPass )
{
	// Lights

	s_pLighting->Enable();
	s_pDirectionalLight->Apply();

	// If this is not the reflection pass, then draw the mirror now. This sets the depth buffer values for the
	// mirror and prevents the reflection image currently in the frame buffer from being overdrawn by anything
	// behind the mirror.

	if ( !bReflectionPass )
	{
		glPushMatrix();

		glTranslatef( MIRROR_X, MIRROR_Y, MIRROR_Z );

		Vector3		mirrorRotationAxis;
		GLfloat		mirrorRotationAngle;

#if defined( USING_REFLECTION )

		s_ReflectionOrientation.GetRotationAxisAndAngle( &mirrorRotationAxis, &mirrorRotationAngle );

#else // defined( USING_REFLECTION )

		s_pMirror->m_Frame.GetRotation().GetRotationAxisAndAngle( &mirrorRotationAxis, &mirrorRotationAngle );

#endif // defined( USING_REFLECTION )

		glRotatef( Math::ToDegrees( mirrorRotationAngle ), mirrorRotationAxis.m_X, mirrorRotationAxis.m_Y, mirrorRotationAxis.m_Z );

#if defined( USING_REFLECTION )

		s_pLighting->Enable();

		Glx::Enable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		s_pReflectionMaterial->Apply();

		glBegin( GL_QUADS );
		glNormal3f( 0.0f, 0.0f, 1.0f );
		glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -MIRROR_W*0.5f, -MIRROR_H*0.5f, 0.0f );
		glTexCoord2f( 1.0f, 0.0f ); glVertex3f(  MIRROR_W*0.5f, -MIRROR_H*0.5f, 0.0f );
		glTexCoord2f( 1.0f, 1.0f ); glVertex3f(  MIRROR_W*0.5f,  MIRROR_H*0.5f, 0.0f );
		glTexCoord2f( 0.0f, 1.0f ); glVertex3f( -MIRROR_W*0.5f,  MIRROR_H*0.5f, 0.0f );
		glEnd();

		Glx::Disable( GL_BLEND );

		s_pLighting->Disable();

#else // defined( USING_REFLECTION )

		s_pLighting->Disable();
		s_pMirror->Apply();

#endif // defined( USING_REFLECTION )

		glDepthMask( GL_TRUE );
		Glx::Disable( GL_TEXTURE_2D );

		// Draw the outline

		glColor3fv( Glx::Rgba::RED.m_C );
		glBegin( GL_LINE_LOOP );
		glVertex3f( -MIRROR_W*0.5f-0.1f, -MIRROR_H*0.5f-0.1f, -0.01f );
		glVertex3f(  MIRROR_W*0.5f+0.1f, -MIRROR_H*0.5f-0.1f, -0.01f );
		glVertex3f(  MIRROR_W*0.5f+0.1f,  MIRROR_H*0.5f+0.1f, -0.01f );
		glVertex3f( -MIRROR_W*0.5f-0.1f,  MIRROR_H*0.5f+0.1f, -0.01f );
		glEnd();

		// Draw the back

		glColor3fv( Glx::Rgba::BLACK.m_C );
		glBegin( GL_QUADS );
		glVertex3f( -MIRROR_W*0.5f,  MIRROR_H*0.5f, -0.01f );
		glVertex3f(  MIRROR_W*0.5f,  MIRROR_H*0.5f, -0.01f );
		glVertex3f(  MIRROR_W*0.5f, -MIRROR_H*0.5f, -0.01f );
		glVertex3f( -MIRROR_W*0.5f, -MIRROR_H*0.5f, -0.01f );
		glEnd();

		s_pLighting->Enable();

		glPopMatrix();
	}

	// Draw the sky.
	// The reflection pass version is a standard skybox, but because the main pass skybox is drawn after the reflection, it must be drawn as far back as possible and
	// depth-testing must be enabled.

	s_pLighting->Disable();

	if ( !bReflectionPass )
	{
		s_pSky->Apply( s_pCamera->GetPosition(), s_pCamera->GetFarDistance() * float( Math::SQRT_OF_3_OVER_3 ), true );
	}
	else
	{
		s_pSky->Apply( s_pCamera->GetPosition(), s_pCamera->GetFarDistance() * float( Math::SQRT_OF_3_OVER_3 ) );
		Glx::Enable( GL_DEPTH_TEST );
	}

	s_pLighting->Enable();
	glDepthMask( GL_TRUE );
	Glx::Disable( GL_TEXTURE_2D );

	// Draw the back shape

	s_pTetrahedronMaterial->Apply();

	glPushMatrix();

	glTranslatef( 0.0f, 0.0f, 8.0f );
	glMultMatrixf( &Matrix44( s_TetrahedronOrientation.GetRotationMatrix33() ).m_M[0][0] );
	auxSolidTetrahedron( 1. );

	glPopMatrix();

	// Draw the center shape

	s_pCubeMaterial->Apply();

	glPushMatrix();

	glTranslatef( 0.0f, 0.0f, 0.0f );
	glMultMatrixf( &Matrix44( s_CubeOrientation.GetRotationMatrix33() ).m_M[0][0] );
	auxSolidCube( 1. );

	glPopMatrix();

	// Draw the left shape

	s_pSphereMaterial->Apply();

	glPushMatrix();

	glTranslatef( 0.0f, 8.0f, 0.0f );
	auxSolidSphere( .5 );

	glPopMatrix();

	// Draw the right shape

	s_pTorusMaterial->Apply();

	glPushMatrix();

	glTranslatef( 8.0f, 0.0f, 0.0f );
	glMultMatrixf( &Matrix44( s_TorusOrientation.GetRotationMatrix33() ).m_M[0][0] );
	auxSolidTorus( .2, .5 );

	glPopMatrix();
}

/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

static void DrawHud()
{
	// Switch to ortho for 2d

	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	glOrtho( 0., 1., 0., 1., 0., 1. );

	// Reset view

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glPushMatrix();

	// 2D stuff

	s_pLighting->Disable();

	{
		std::ostringstream	buffer;

		buffer << "Reflectivity = " << s_Reflectivity << std::ends;

		glColor3f( 1.0f, 1.0f, 1.0f );
		glRasterPos2f( .02f, .02f );

		s_pFont->DrawString( buffer.str().c_str() );
	}
	
	// Restore view

	glPopMatrix();

	// Switch back to perspective

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();

	glMatrixMode( GL_MODELVIEW );
}



