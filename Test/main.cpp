/*****************************************************************************

                                   main.cpp

						Copyright 2001, John J. Bolton
	----------------------------------------------------------------------

	$Header: //depot/GlObjects/Test/main.cpp#4 $

	$NoKeywords: $

*****************************************************************************/

//#include <cstdio>
//#include <cstdlib>
//#include <sstream>
//#include <cmath>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <mmsystem.h>

#include <gl/gl.h>
#include <gl/glaux.h>

#include "Glx/Glx.h"
#include "GlObjects/Axes/Axes.h"
#include "GlObjects/SkyBox/SkyBox.h"
#include "GlObjects/TerrainCamera/TerrainCamera.h"
#include "GlObjects/TextureLoader/TextureLoader.h"
#include "Wglx/Wglx.h"
#include "Wx/Wx.h"

//#include "Misc/Etc.h"
#include "Math/Vector3.h"

static LRESULT CALLBACK WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
static void InitializeRendering();
static void Display();
static void Reshape( int w, int h );
static bool Update( HWND hWnd );

static char						s_AppName[]	 = "SkyBox";
static char						s_TitleBar[] = "SkyBox";


static float					s_CameraSpeed	=	2.f;

static WGlx::Font *				s_pFont;

static GlObjects::SkyBox *			s_pSkyBox;
static GlObjects::Axes *			s_pAxes;
static GlObjects::TerrainCamera *	s_pCamera;
static GlObjects::TextureLoader		s_TextureLoader;


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPreviousInst, LPSTR lpszCmdLine, int nCmdShow )
{
	if ( Wx::RegisterWindowClass(	CS_OWNDC, ( WNDPROC )WindowProc, hInstance, s_AppName ) == NULL )
	{
		MessageBox( NULL, "Wx::RegisterWindowClass() failed.", "Error", MB_OK );
		exit( 1 );
	}

	HWND hWnd = CreateWindowEx( 0,
								s_AppName,
								s_TitleBar,
								WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
								0, 0, 500, 500,
								NULL,
								NULL,
								hInstance,
								NULL );

	if ( hWnd == NULL )
	{
		MessageBox( NULL, "CreateWindowEx() failed.", "Error", MB_OK );
		exit( 1 );
	}


	HDC const	hDC	= GetDC( hWnd );
	int			rv;

	WGlx::SetPixelFormat( hDC, 0, true );

	{
		WGlx::CurrentRenderingContext	rc( hDC );	// Current rendering context

		InitializeRendering();

		s_pCamera	= new GlObjects::TerrainCamera( 60.f, 1.f, 1000.f, Vector3::Origin(), 0.0f, 90.0f, 0.0f );
		if ( !s_pCamera ) exit( 1 );


		{
			LOGFONT	lf =
			{
				16, 0,
				0, 0,
				FW_BOLD, FALSE, FALSE, FALSE,
				ANSI_CHARSET,
				OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH|FF_DONTCARE,
				"Courier New"
			};

			s_pFont = new WGlx::Font( hDC, lf );
			if ( !s_pFont ) exit( 1 );
		}

		// Create the skybox

		s_pSkyBox = new GlObjects::SkyBox( "res/test" );
		if ( !s_pSkyBox ) exit( 1 );

		s_pAxes = new GlObjects::Axes( 10.0f );

		ShowWindow( hWnd, nCmdShow );

		rv = Wx::MessageLoop( hWnd, Update );

		delete s_pAxes;
		delete s_pSkyBox;
		delete s_pFont;
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
	Display();
	
	return true;	// Update as often as possible
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
			s_pCamera->Move( -Vector3::ZAxis() * -s_CameraSpeed );
			break;

		case 'd':	// Strafe right
			s_pCamera->Move( Vector3::XAxis() * s_CameraSpeed );
			break;

		case 'a':	// Strafe left
			s_pCamera->Move( Vector3::XAxis() * -s_CameraSpeed );
			break;

		case 'w':	// Strafe up
			s_pCamera->Move( Vector3::YAxis() * s_CameraSpeed );
			break;

		case 'x':	// Strafe down
			s_pCamera->Move( Vector3::YAxis() * -s_CameraSpeed );
			break;
		}
		return 0;
	}

	case WM_KEYDOWN:
		switch ( wParam )
		{
		case VK_UP:
			s_pCamera->Pitch( 5.f  );
			break;

		case VK_DOWN:
			s_pCamera->Pitch( -5.f  );
			break;

		case VK_LEFT:
			s_pCamera->Yaw( 5.f );
			break;

		case VK_RIGHT:
			s_pCamera->Yaw( -5.f );
			break;

		case VK_PRIOR:
			s_pCamera->Roll( 5.f );
			break;

		case VK_INSERT:
			s_pCamera->Roll( -5.f );
			break;
		}
		return 0;

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
	glCullFace( GL_BACK );
	glEnable( GL_CULL_FACE );
}



/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

static void DrawHud()
{
	// Switch to ortho projection for 2d

	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	glOrtho( 0., 1., 0., 1., -1., 1. );

	// Reset view for hud

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glDisable( GL_TEXTURE_2D );
	glDisable( GL_DEPTH_TEST );

	static DWORD	oldTime	= timeGetTime();
	static int		frames	= 0;
	static char		buffer[ 256 ];

	DWORD const		newTime			= timeGetTime();
	int const		dt				= newTime - oldTime;

	++frames;
	if ( dt >= 1000 )
	{
		sprintf( buffer, "Frame rate: %5.2f fps", frames / ( dt * .001  ) );
		oldTime = newTime;
		frames = 0;
	}

	glColor3fv( Glx::Rgba::YELLOW.m_C );
	glRasterPos2f( .01f, .01f );
	s_pFont->DrawString( buffer );

	// Switch back to perspective projection

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

static void Display()
{
	glMatrixMode( GL_MODELVIEW );

	glClear( GL_DEPTH_BUFFER_BIT );

	glLoadIdentity();

	// Place the camera

	s_pCamera->Look();

	// Draw the sky

	s_pSkyBox->Apply( *s_pCamera );

	glDepthMask( GL_TRUE );
	glEnable( GL_DEPTH_TEST );
	glDisable( GL_TEXTURE_2D );

	// Draw the axes

	s_pAxes->Apply();

	// Draw the HUD

	DrawHud();

	// Display the scene

	glFlush();
	SwapBuffers( wglGetCurrentDC() );

	// Report any GL errors

	Glx::ReportAnyErrors();
}


/********************************************************************************************************************/
/*																													*/
/*																													*/
/********************************************************************************************************************/

static void Reshape( int w, int h )
{
	glViewport( 0, 0, GLsizei( w ), GLsizei( h ) );

	s_pCamera->Reshape( w, h );
}
