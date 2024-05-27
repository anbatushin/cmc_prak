#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>

#define S_FUNCTION_NAME sf
#define S_FUNCTION_LEVEL 2

#define IN_0_FRAME_BASED    FRAME_NO

#define OUT_0_FRAME_BASED    FRAME_NO

#define SAMPLE_TIME_0        INHERITED_SAMPLE_TIME

#define SFUNWIZ_GENERATE_TLC 1
#define SOURCEFILES ""
#define PANELINDEX           6
#define SFUNWIZ_REVISION     2.0

#include "simstruc.h"

static  double x1 = 0, x2 = 0, x1_ = 0, x2_ = 0, X = 0;
static  double l1, l2;

static int glInited = 0;
static  HWND  hGlWindow = NULL;
static  HDC   hDC = NULL;
static  HGLRC hRC = NULL;

#define GL_CLASS_NAME   "gl_class_name"

void CALLBACK resize ( int width, int height ) {
    glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60,float(width)/height,(l1+l2),5*(l1+l2));
}

#define Pi 3.1415

void CALLBACK display() {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(X,0,0);
    //glRotated(-180,0,1,0);
    glBegin(GL_QUADS);
    glColor3f(1,1,1);
    glVertex3d(-l1/10,-l1/45,-l1/25);
    glVertex3d( l1/10,-l1/45,-l1/25);
    glVertex3d( l1/10, l1/45,-l1/25);
    glVertex3d(-l1/10, l1/45,-l1/25);

    glVertex3d(-l1/10, l1/45, l1/2);
    glVertex3d(-l1/10,-l1/45, l1/2);
    glVertex3d( l1/10,-l1/45, l1/2);
    glVertex3d( l1/10, l1/45, l1/2);

    glVertex3d( l1/10,-l1/45,-l1/25);
    glVertex3d( l1/10,-l1/45, l1/2);
    glVertex3d( l1/10, l1/45, l1/2);
    glVertex3d( l1/10, l1/45,-l1/25);

    glVertex3d(-l1/10,-l1/45,-l1/25);
    glVertex3d(-l1/10,-l1/45, l1/2);
    glVertex3d(-l1/10, l1/45, l1/2);
    glVertex3d(-l1/10, l1/45,-l1/25);

    glColor3f(0.8,0.1,0);
    glVertex3d(-l1/10, l1/45,-l1/25);
    glVertex3d(-l1/10, l1/45, l1/2);
    glVertex3d( l1/10, l1/45, l1/2);
    glVertex3d( l1/10, l1/45,-l1/25);

    glVertex3d(-l1/10,-l1/45,-l1/25);
    glVertex3d(-l1/10,-l1/45, l1/2);
    glVertex3d( l1/10,-l1/45, l1/2);
    glVertex3d( l1/10,-l1/45,-l1/25);
    glEnd();

    glTranslated(0,l1/20,-l1/20);
    glRotated(-180*x1/Pi,0,0,1);
    GLUquadricObj *qObj = gluNewQuadric();
    glPushMatrix();
    glRotated(90,1,0,0);
    glColor3f(1,0,0);
    gluCylinder(qObj,l1/30,l1/30,2*l1/18,3,1);
    gluSphere(qObj,l1/30,3,3);
    glPushMatrix();
    glPopMatrix();
    glTranslated(0,0,2*l1/18);
    //gluSphere(qObj,l1/30,3,3);
    glPopMatrix();
    gluDeleteQuadric(qObj);
    
    SwapBuffers ( hDC );
}

static int getBitDepth () {
    HDC hDC  = GetDC ( NULL );
    HBITMAP hBmp = CreateCompatibleBitmap ( hDC, 1, 1 );
    BITMAPINFO * bmi  = (LPBITMAPINFO) malloc ( sizeof (BITMAPINFOHEADER) + 4 * sizeof (RGBQUAD) );
    int res, bitCount;

    bmi -> bmiHeader.biBitCount = (WORD) GetDeviceCaps ( hDC, BITSPIXEL );
    bmi -> bmiHeader.biCompression = BI_BITFIELDS;
    bmi -> bmiHeader.biSize = sizeof ( BITMAPINFOHEADER );
    bmi -> bmiHeader.biWidth = 1;
    bmi -> bmiHeader.biHeight = 1;
    bmi -> bmiHeader.biClrUsed = 0;
    bmi -> bmiHeader.biPlanes = 1;

    res = GetDIBits ( hDC, hBmp, 0, 1, NULL, bmi, DIB_RGB_COLORS );
    bitCount = bmi->bmiHeader.biBitCount;

    DeleteObject( hBmp );
    ReleaseDC( NULL, hDC );
    DeleteObject( hBmp );
    free( bmi );

    return bitCount;
}

static  int    createOpenGlWindow ( int x, int y, int width, int height, int depthBits ) {
    PIXELFORMATDESCRIPTOR pfd = {
                 sizeof (PIXELFORMATDESCRIPTOR),                         // size of this Pixel Format Descriptor
                 1,                                                      // version number
                 PFD_DRAW_TO_WINDOW |                                    // format must support Wind
                 PFD_SUPPORT_OPENGL |                                    // format must support OpenGL
                 PFD_DOUBLEBUFFER,                                       // must support double buffering
                 PFD_TYPE_RGBA,                                          // request an RGBA format
                 0,                                            // select color depth
                 0, 0, 0, 0, 0, 0,                                       // color bits
                 0,                                                      // no alpha buffer
                 0,                                                      // shift bit ignored
                 0,                                                      // no accumulation cuffer
                 0, 0, 0, 0,                                             // accumulation bits ignored
                 0,                                                // Z-Buffer depth
                 8,                                                      // 8-bit stencil buffer
                 0,                                                      // no auxiliary buffer
                 PFD_MAIN_PLANE,                                         // main drawing layer
                 0,                                                      // reserved
                 0, 0, 0                                                 // layer masks ignored
                 };
                 GLuint pixelFormat;                                        // will hold the selected pixel format

    DWORD windowStyle = WS_OVERLAPPEDWINDOW;            // set window style
    DWORD windowExtendedStyle = WS_EX_APPWINDOW;                // set window extended style
    RECT  windowRect;  // define window coordinates
    int   bpp;

    windowRect.left = x;
    windowRect.top = y;
    windowRect.right = x+width+100;
    windowRect.bottom = y+height+100;
    pfd.cColorBits = getBitDepth();
    pfd.cDepthBits = depthBits;

    windowExtendedStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;   // window xxtended style
    windowStyle         = WS_OVERLAPPEDWINDOW;                  // window style      
    // adjust window, Account for window worders    
    AdjustWindowRectEx ( &windowRect, windowStyle, 0, windowExtendedStyle );

                                                                // create the OpenGL window
    hGlWindow = CreateWindowEx ( windowExtendedStyle,           // extended style
                                 GL_CLASS_NAME,                      // class name
                                 "model",                             // window title
                                  windowStyle,                        // window style
                                  windowRect.left,                    // window (X,Y) position
                                windowRect.top,
                                windowRect.right - windowRect.left, // window width
                                windowRect.bottom - windowRect.top, // window height
                            HWND_DESKTOP,                       // desktop is window's parent
                            0,                                  // no nenu
                            GetModuleHandle ( NULL ),           // pass the window instance
                                NULL );

    if ( hGlWindow == NULL )                                    // was window creation a success?
        return 0;                                               // if not then return false

    hDC = GetDC ( hGlWindow );                                  // get device context for this window

    if ( hDC == NULL )                                          // did we get a device context?
        return 0;                                               //done ();

    bpp = getBitDepth ();                                       // bit depth

    pixelFormat = ChoosePixelFormat ( hDC, &pfd );              // find a compatible pixel format

    if ( pixelFormat == 0 )                                     // did we find a compatible format?
        return 0;                                               // failed

    if ( SetPixelFormat ( hDC, pixelFormat, &pfd ) == FALSE )   // try to set pixel format
        return 0;                                               // failed

    hRC = wglCreateContext ( hDC );                             // try to get a rendering context

    if ( hRC == NULL )                                          // did we get a rendering context?
        return 0;                                               // failed

                                                                // make the rendering context our current rendering context
    if ( wglMakeCurrent ( hDC, hRC ) == FALSE )
        return 0;                                               // failed

    resize ( width, height );                                   // reshape our GL window

    ShowWindow ( hGlWindow, SW_SHOW );

    return 1;                                                   // window creating was successful                                                                 
// further initialization will be done in WM_CREATE
    }

static int destroyOpenGlWindow () {
    wglMakeCurrent ( NULL, NULL );

    if ( hRC != NULL )
        wglDeleteContext ( hRC );                               // delete The Rendering Context

    if ( hDC != NULL )
        ReleaseDC ( hGlWindow, hDC );                           // release Our Device Context

    if ( hGlWindow != NULL )
        DestroyWindow ( hGlWindow );                            // destroy the window

    hGlWindow = NULL;                                           // zero the window handle
    hDC       = NULL;                                           // zero the device context
    hRC       = NULL;                                           // zero the rendering context

    return 1;
}

static LRESULT CALLBACK windowProc ( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
    switch ( msg )                                              // evaluate window message
    {         case WM_SIZE:                                           // resizing action has taken place             
                  resize (LOWORD (lParam), HIWORD (lParam));          // resize window             
                  if (glInited) display ();

            return 0;
    }

return DefWindowProc ( hWnd, msg, wParam, lParam );         // pass nnhandled messages to DefWindowProc
}

static void initGL () {
    float pos[4] = {1,1,0,1};
    float dir[3] = {-1,-1,1};

    GLfloat mat_specular[] = {1,1,1,1};                                                                 // create window class
    WNDCLASSEX windowClass;

    ZeroMemory ( &windowClass, sizeof (WNDCLASSEX) );           // clear the memory

    windowClass.cbSize = sizeof (WNDCLASSEX);          // size of the windowClass structure
    windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;     // redraws the window on any movement / resizing
     windowClass.lpfnWndProc     = (WNDPROC) windowProc;         // windowProc Handles Messages
     windowClass.hInstance       = GetModuleHandle ( NULL );     // set the instance
     windowClass.hbrBackground   = (HBRUSH)COLOR_APPWORKSPACE;   // class background brush color
     windowClass.hCursor     = LoadCursor ( NULL, IDC_ARROW );   // load the arrow pointer
     windowClass.lpszClassName   = GL_CLASS_NAME;                // sets the classname
     windowClass.hIcon           = NULL;

    if ( RegisterClassEx ( &windowClass ) == 0 )
        return;

    createOpenGlWindow ( 50, 10, 500, 500, 32 );
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);

    float amb[4] = {0.5,0.5,0.5,1};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,amb);

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 128.0);
}


static void mdlInitializeSizes (SimStruct *S) {
    char buf[100];
    l1 = mxGetString(ssGetSFcnParam(S,0),buf,100);
    sscanf(buf,"%lf",&l1);
    l2 = mxGetString(ssGetSFcnParam(S,1),buf,100);
    sscanf(buf,"%lf",&l2);

    if (!ssSetNumInputPorts(S, 1))
        return;
    ssSetInputPortVectorDimension(S, 0, 3);
    ssSetInputPortRequiredContiguous(S, 0, 1);
    ssSetInputPortDirectFeedThrough(S, 0, 1);
    if (!ssSetNumOutputPorts(S, 1))
        return;
    ssSetOutputPortVectorDimension(S, 0, 3);

    ssSetNumContStates(S, 0);
    ssSetNumDiscStates(S, 0);

    ssSetNumSFcnParams(S, 2);

    ssSetNumSampleTimes(S, 1);

    ssSetNumRWork(S, 0);
    ssSetNumIWork(S, 0);
    ssSetNumPWork(S, 0);
    ssSetNumModes(S, 0);

    ssSetNumNonsampledZCs(S, 0);

    ssSetOptions(S, (SS_OPTION_EXCEPTION_FREE_CODE |SS_OPTION_USE_TLC_WITH_ACCELERATOR |SS_OPTION_WORKS_WITH_CODE_REUSE));

    if (!glInited){
        initGL();
        glInited = 1;
    }
}

static void mdlStart(SimStruct *S)
{ }

static void mdlInitializeSampleTimes(SimStruct *S) {
    ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
    ssSetOffsetTime(S, 0, FIXED_IN_MINOR_STEP_OFFSET);
}

static void mdlOutputs(SimStruct *S, int_T tid) {
    const double  *u  = (const double*)ssGetInputPortSignal(S,0);
    double *y  = (double*)ssGetOutputPortRealSignal(S,0);

    l1 = 0;
    char buf[100] = {0};
    l1 = mxGetScalar(ssGetSFcnParam(S,0));
    l2 = 0;
    l2 = mxGetScalar(ssGetSFcnParam(S,1));

    x1 =  u[0];
    x1_ = u[1];
    X   = u[2];

    y[0] = u[0];
    y[1] = u[1];
    y[2] = u[2];

    display();

    Sleep(30);
}

static void mdlTerminate(SimStruct *S) {
    destroyOpenGlWindow ();
    if (UnregisterClass(GL_CLASS_NAME, GetModuleHandle(NULL))!=0)
        glInited = 0;
}

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */

#endif  MATLAB_MEX_FILE
#include "simulink.c"      /* MEX-file interface mechanism */
//#else
//#include "cg_sfun.h"       /* Code generation registration function */
//#endif
