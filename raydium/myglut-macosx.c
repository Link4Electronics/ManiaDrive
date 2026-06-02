/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#include "headers/myglut.h"

#include <Carbon/Carbon.h>
#include <AGL/agl.h>

#define mApple 128
#define mFile 129
#define iAbout 1
#define iQuit 1

#define SETPORT(w) SetPortWindowPort(w)
#define GRAFPTR

#define ACTIVE_SLEEPTIME 0
#define INACTIVE_SLEEPTIME 10

#define APPLE_MOD_SHIFT 1
#define APPLE_MOD_CTRL 2
#define APPLE_MOD_ALT 4

static bool initialized = false;
static int origin[2] = {0, 0};
static int size[2] = {640, 480};
static SInt16 horScrSze, verScrSze;
static bool fullScreen;
static bool autoRepeatKey = false;
static bool quitFlag;
static int modifiers;
static int clickModifiers;
static SInt32 sleepTime = ACTIVE_SLEEPTIME;
static int button;
static int state;
static signed char special = 0;
static WindowPtr window = NULL;
static AGLContext currContext = NULL;

void pwInit(int x, int y, int w, int h, int multisample, char *title, int border, int num_samples, int window_style);
int raydium_init_cli_option(char *option, char *value);
char *raydium_version(void);

static void handleEvents(void);
static void initialize(void);
static bool makeWindow(int x, int y, int w, int h, char* title, bool resizable);
static void createContext(int multiSample, int numSamples, bool fullScreen);
static void makeMenu(void);
static void handleMenuEvent(long menuResult);
static void handleKeyEvent(EventRecord* eventPtr, int state);
static void handleMouseMoveEvt(EventRecord* eventPtr);
static pascal OSErr QuitAppleEventHandler(const AppleEvent *appleEvt, AppleEvent* reply, long refcon);
static void CtoPcpy(Str255 pstr, char *cstr);
static void aboutBox(void);
static void exitFunc();

static pascal OSErr 
QuitAppleEventHandler(const AppleEvent *appleEvt, AppleEvent* reply, long refcon)
{
    quitFlag = true;
    return noErr;
}

static void initialize()
{
    OSErr err = noErr;
    BitMap scrBits;

    quitFlag = false;
    err = AEInstallEventHandler(kCoreEventClass, kAEQuitApplication, NewAEEventHandlerUPP(QuitAppleEventHandler), 0, false);

    // This funtion is to handle the "Command + Q" event.
    // This key event is not send as a keyDown event with Carbon.
    if (err != noErr)
    {
        exitFunc();
    }

    InitCursor();

    // Get screen size.
    GetQDGlobalsScreenBits(&scrBits);

    horScrSze = scrBits.bounds.right - scrBits.bounds.left;
    verScrSze = scrBits.bounds.bottom - scrBits.bounds.top;
}

// Copy a C string into a Pascal string.
static void CtoPcpy(Str255 pstr, char* cstr)
{
    int i = 1;    
    while((*cstr) && (i < 255)) pstr[i++] = *cstr++;
    pstr[0] = i - 1;
}

static bool makeWindow(int x, int y, int w, int h, char* title, bool resizable)
{
    Rect wRect;
    Str255 pTitle;

    CtoPcpy(pTitle, title);
    SetRect(&wRect, x, y, w+x, h+y);

    if (fullScreen)
    {
        // RAYDIUM_RENDERING_FULLSCREEN
        // Create a plain box, no shadow or close box for the fullscreen mode.
        window = NewCWindow(nil, &wRect, pTitle, true, plainDBox, (WindowPtr) - 1, 0, 0);
    }
    else
    {
        if (resizable)
        {
            // RAYDIUM_RENDERING_WINDOW
            // Create a standard document window, which supports all of the window manipulation elements (close box, zoom box, and size box).
            window = NewCWindow(nil, &wRect, pTitle, true, zoomDocProc, (WindowPtr) - 1, true, 0);
        }
        else
        {
            // RAYDIUM_RENDERING_WINDOW_FIXED
            // Create a movable window with close box, but no size box or zoom box.
            window = NewCWindow(nil, &wRect, pTitle, true, noGrowDocProc, (WindowPtr) - 1, true, 0);
        }
    }

    if (window == NULL)
    {
        raydium_log("(my)glut: unable to open window");
        return false;
    }

    SETPORT(window);

    return true;
}

static void createContext(int multiSample, int numSamples, bool fullScreen)
{
    GDHandle screen;
    AGLPixelFormat aglPixFmt;

    // Choose pixel format.
    GLint attrib[14];
    GLint i = 0;
    attrib[i++] = AGL_WINDOW;
    attrib[i++] = AGL_RGBA;
    attrib[i++] = AGL_NO_RECOVERY;
    attrib[i++] = AGL_SINGLE_RENDERER;
    attrib[i++] = AGL_DOUBLEBUFFER;

    if (fullScreen)
    {
        screen = GetGWorldDevice(GetWindowPort(window));
        attrib[0] = AGL_FULLSCREEN;
    }

    attrib[i++] = AGL_DEPTH_SIZE;
    attrib[i++] = 32;
    attrib[i++] = AGL_STENCIL_SIZE;
    attrib[i++] = 8;
    attrib[i++] = AGL_NONE;

    if (multiSample)
    {
        i--;
        attrib[i++] = AGL_SAMPLE_BUFFERS_ARB;
        attrib[i++] = 1;
        attrib[i++] = AGL_SAMPLES_ARB;
        attrib[i++] = numSamples;
        attrib[i++] = AGL_NONE;

        if (fullScreen)
        {
            aglPixFmt = aglChoosePixelFormat(&screen, 1, attrib);
        }
        else
        {
            aglPixFmt = aglChoosePixelFormat(NULL, 0, attrib);
        }

        if (aglPixFmt == NULL)
        {
            multiSample = 0;
            raydium_log("(my)glut: multisample pixel format not found");
            i -= 5;
            attrib[i++] = AGL_NONE;
        }
    }

    if (!multiSample)
    {
        if (fullScreen)
        {
            aglPixFmt = aglChoosePixelFormat(&screen, 1, attrib);
        }
        else
        {
            aglPixFmt = aglChoosePixelFormat(NULL, 0, attrib);
        }
    }

    if (aglPixFmt == NULL)
    {
        raydium_log("(my)glut: could not find pixel format");
    }

    // Create an AGL context.
    currContext = aglCreateContext(aglPixFmt, NULL);
    aglDestroyPixelFormat(aglPixFmt);

    if (currContext == NULL)
    {
        raydium_log("(my)glut: could not create OpenGL context");
    }
}

void makeMenu()
{
    SInt32 response;
    MenuRef menu;
    CreateNewMenu(mApple, 0, &menu);
    SetMenuTitleWithCFString(menu, CFSTR("Raydium"));

    InsertMenu(menu, 0);
    InsertMenuItemTextWithCFString(menu, CFSTR("About Raydium..."), 0, 0, iAbout);

    // If we're not running on Mac OS X then we need to add a File:Quit command.
    Gestalt(gestaltMenuMgrAttr, &response);

    if ((response & gestaltMenuMgrAquaLayoutMask) == 0)
    {
        menu = NewMenu(mFile, (ConstStr255Param)"File");
        InsertMenu(menu, 0);
        AppendMenu(menu, (ConstStr255Param)"Quit");
    }

    DrawMenuBar();
}

static void exitFunc ()
{
    myglutCloseWindow();
}

static void aboutBox()
{
    // Show a dialog box with info on Raydium.
    SInt16 outItemHit;
    char version[32];
    Str255 Pversion;
    char* about = "Raydium 3D Game Engine <http://raydium.org>";
    Str255 Pabout;
    sprintf(version, "Raydium %s, CQFD Corp.", raydium_version());
    CtoPcpy(Pversion, version);
    CtoPcpy(Pabout, about);
    StandardAlert(kAlertPlainAlert, Pversion, Pabout, NULL, &outItemHit);
}

static void handleMenuEvent(long menuCmd)
{
    int menuNum = HiWord(menuCmd);
    int itemNum = LoWord(menuCmd);

    switch(menuNum)
    {
        case mApple:
        {
            switch (itemNum)
            {
                case iAbout: aboutBox(); break;
                default: break;
            }
            break;
        }
        case mFile:
        {
            switch (itemNum)
            {
                case iQuit: exitFunc(); break;
                default: break;
            }
        }
    }

    // Remove the highlight on the selected menu.
    HiliteMenu(0);
}

static int translateKey(int ch, int key)
{
    special = 1;

    switch (ch)
    {
        case kLeftArrowCharCode:
            ch = GLUT_KEY_LEFT;
            break;
        case kUpArrowCharCode:
            ch = GLUT_KEY_UP;
            break;
        case kRightArrowCharCode:
            ch = GLUT_KEY_RIGHT;
            break;
        case kDownArrowCharCode:
            ch = GLUT_KEY_DOWN;
            break;
        case kPageUpCharCode:
            ch = GLUT_KEY_PAGE_UP;
            break;
        case kPageDownCharCode:
            ch = GLUT_KEY_PAGE_DOWN;
            break;
        case kHomeCharCode:
            ch = GLUT_KEY_HOME;
            break;
        case kEndCharCode:
            ch = GLUT_KEY_END;
            break;
        case kHelpCharCode:
            ch = GLUT_KEY_INSERT;
            break;
        case kFunctionKeyCharCode:
            switch(key)        
            {
                case 0x7A:
                    ch = GLUT_KEY_F1;
                    break;
                case 0x78:
                    ch = GLUT_KEY_F2;
                    break;
                case 0x63:
                    ch = GLUT_KEY_F3;
                    break;
                case 0x76:
                    ch = GLUT_KEY_F4;
                    break;
                case 0x60:
                    ch = GLUT_KEY_F5;
                    break;
                case 0x61:
                    ch = GLUT_KEY_F6;
                    break;
                case 0x62:
                    ch = GLUT_KEY_F7;
                    break;
                case 0x64:
                    ch = GLUT_KEY_F8;
                    break;
                case 0x65:
                    ch = GLUT_KEY_F9;
                    break;
                case 0x6D:
                    ch = GLUT_KEY_F10;
                    break;
                case 0x67:
                    ch = GLUT_KEY_F11;
                    break;
                case 0x6F:
                    ch = GLUT_KEY_F12;
                    break;
                default:
                    // Above F12.
                    raydium_log("(my)glut: unsupported function key");
                    break;
            }
            break;
        default:
            special = 0;
            break;
    }
    return ch;
}

static void handleKeyEvent(EventRecord* eventPtr, int updown)
{
    GrafPtr origPort;

    if (!glutKeyboardFuncCB) return;

    // Handle key events.
    int ch = eventPtr->message & charCodeMask;
    int key = (eventPtr->message & keyCodeMask) >> 8;
    ch = translateKey(ch, key);
    GetPort(&origPort);
    SETPORT(window);
    GlobalToLocal(&eventPtr->where);

    // Special down.
    if (special && updown == GLUT_DOWN && glutSpecialFuncCB && !raydium_key[ch])
        glutSpecialFuncCB(ch, eventPtr->where.h, eventPtr->where.v);

    // Special up.
    if( special && updown == GLUT_UP && glutSpecialUpFuncCB && raydium_key[ch])
        glutSpecialUpFuncCB(ch, eventPtr->where.h, eventPtr->where.v);

    // Normal.
    if(!special && updown == GLUT_DOWN && glutKeyboardFuncCB)
        glutKeyboardFuncCB(ch, eventPtr->where.h, eventPtr->where.v);

    SetPort(origPort);

    return;
}

static void handleDrag(WindowPtr wd, Point mouseloc)
{
    Point loc;
    GrafPtr origPort;
    Rect dragBounds, windowBounds;

    GetRegionBounds(GetGrayRgn(), &dragBounds);
    DragWindow(wd, mouseloc, &dragBounds);

    GetPort(&origPort);
    SETPORT(window);

    GetWindowPortBounds(wd, &windowBounds);
    loc.h = windowBounds.left;
    loc.v = windowBounds.top;

    LocalToGlobal(&loc);
    origin[0] = loc.h;
    origin[1] = loc.v;

    MoveWindow(GRAFPTR window, loc.h, loc.v, false);
    aglUpdateContext(currContext);
    SetPort(origPort);
}

static void handleResize(WindowPtr wd)
{
    GrafPtr origPort;
    Rect rectPort;

    GetPort(&origPort);
    SETPORT(window);

    aglSetCurrentContext(currContext);
    aglUpdateContext(currContext);

    GetWindowPortBounds(wd, &rectPort);

    size[0] = rectPort.right - rectPort.left;
    size[1] = rectPort.bottom - rectPort.top;

    _glutWindowSize[0] = size[0];
    _glutWindowSize[1] = size[1];

    SetPort(origPort);
}

static void handleGoAwayBox(WindowPtr wd, Point mouseloc)
{
    GrafPtr origPort;

    GetPort(&origPort);
    SETPORT(wd);

    if (TrackGoAway(wd, mouseloc))
    { 
        exitFunc();
    }

    SetPort(origPort);
}

static int last_m_h = 0;
static int last_m_v = 0;

static void handleMouseMoveEvt(EventRecord* eventPtr)
{
    GrafPtr origPort;

    if (!glutPassiveMotionFuncCB) return;

    int m_h = eventPtr->where.h;
    int m_v = eventPtr->where.v;

    if ((m_v != last_m_v) || (m_h != last_m_h))
    {
        last_m_v = m_v;
        last_m_h = m_h ;
        GetPort(&origPort);
        SETPORT(window);
        GlobalToLocal(&eventPtr->where);

        if (glutPassiveMotionFuncCB)
        {
            glutPassiveMotionFuncCB(eventPtr->where.h, eventPtr->where.v);
        }

        SetPort(origPort);
    }
    return;
}

static void handleEvents()
{
    EventRecord event;
    GrafPtr origPort;

    if (quitFlag) exitFunc();

    WaitNextEvent(everyEvent, &event, sleepTime, nil);

    button = GLUT_LEFT_BUTTON;
    state = GLUT_UP;

    modifiers = 0;

    if (event.modifiers & (shiftKey | rightShiftKey))
        modifiers |= APPLE_MOD_SHIFT;
    if (event.modifiers & (optionKey | rightOptionKey))
        modifiers |= APPLE_MOD_ALT;
    if (event.modifiers & (controlKey | rightControlKey))
        modifiers |= APPLE_MOD_CTRL;

    switch (event.what)
    {
        case keyDown:
        {            
            state = GLUT_DOWN;

            // Look for a "Command + Key" for the menu.
            if (event.modifiers & cmdKey)
            {
                char achar = (char)(event.message & charCodeMask);
                handleMenuEvent(MenuKey(achar));
                return;
            }

            handleKeyEvent(&event, state);

            return;
        }
        case autoKey:
        {
            if (!autoRepeatKey)
            {
                // Report a mouse move event.
                handleMouseMoveEvt(&event);
                return;
            }

            // Report a key down event.
            state = GLUT_DOWN;
        }
        case keyUp:
        {
            state = GLUT_UP;
            handleKeyEvent(&event, state);
            return;
        }
        case mouseDown:
        {
            short part;
            WindowPtr wd;

            state = GLUT_DOWN;
            part = FindWindow(event.where, &wd);

            switch(part)
            {
                case inMenuBar:
                {
                    handleMenuEvent(MenuSelect(event.where));
                    return;
                }
                case inDrag:
                {
                    handleDrag(wd, event.where);
                    return;
                }
                case inGoAway:
                {
                    handleGoAwayBox(wd, event.where);
                    return;
                }
                case inZoomOut:
                {
                    ZoomWindow(wd, inZoomOut, true);
                    handleResize(wd);
                    return;
                }
                case inZoomIn:
                {
                    ZoomWindow(wd, inZoomIn, false);
                    handleResize(wd);
                    return;
                }
                case inContent:
                {                    
                    if (!wd) return;

                    if (GRAFPTR window != FrontWindow())
                    {
                        SelectWindow(GRAFPTR window);
                        return;
                    }
                }
            }

            clickModifiers = modifiers;

            if (glutMouseFuncCB)
            {
                glutMouseFuncCB(button, state, event.where.h, event.where.v);
            }

            SetPort(origPort);

            return;
        }
        case mouseUp:
        {
            state = GLUT_UP;

            switch(clickModifiers)
            {
                case APPLE_MOD_ALT:
                    button = GLUT_RIGHT_BUTTON;
                    break;
                case APPLE_MOD_CTRL:
                    button = GLUT_MIDDLE_BUTTON;
                    break;
                default:
                    button = GLUT_LEFT_BUTTON;
                    break;
            }

            GetPort(&origPort);
            SETPORT(window);
            GlobalToLocal(&event.where);
            modifiers = 0;

            if (glutMouseFuncCB)
            {
                glutMouseFuncCB(button, state, event.where.h, event.where.v);
            }

            SetPort(origPort);

            return;
        }
        case osEvt:
        {
            unsigned char subcode = event.message >> 24;

            if (subcode == suspendResumeMessage)
                if (resumeFlag & event.message)
                {
                    sleepTime = ACTIVE_SLEEPTIME;
                    return;
                }
                else
                    sleepTime = INACTIVE_SLEEPTIME;
            break;
        }
        case activateEvt:
        {
            if (event.modifiers & activeFlag)
            {
                    sleepTime = ACTIVE_SLEEPTIME;
                    return;
            }
            else
                sleepTime = INACTIVE_SLEEPTIME;
            break;
        }
        case updateEvt:
        {            
            if (glutReshapeFuncCB)
            {
                glutReshapeFuncCB(size[0], size[1]);
            }
        }
        case nullEvent:
        {
            // Likely an idle event.
            // Report mouse location, because there are no mouse move events.
            handleMouseMoveEvt(&event);
            return;
        }
        default:
        {
            raydium_log("(my)glut: unhandled event: %i", event.what);
            break;
        }
    }
}

int currScreen;
int currConnect;

void glutInit(int *argc, char **argv)
{
    currScreen  = 0;
    currConnect = 0;

    glutReshapeFuncCB=NULL;
    glutKeyboardFuncCB=NULL;
    glutSpecialUpFuncCB=NULL;
    glutSpecialFuncCB=NULL;
    glutMotionFuncCB=NULL;
    glutPassiveMotionFuncCB=NULL;
    glutMouseFuncCB=NULL;
    glutDisplayFuncCB=NULL;
    glutIdleFuncCB=NULL;

    _glutMouseVisible=1;
}

void pwInit(int x, int y, int w, int h, int multisample, char *title, int border, int num_samples, int window_style)
{
    if (initialized)
    {
        raydium_log("(my)glut: pwInit already called");
        return;
    }

    fullScreen = ((w < 0) || (h < 0)) ? true : false;
    bool resizable = (window_style == RAYDIUM_RENDERING_WINDOW);
    initialize();
    initialized = true;
    window = NULL;

    if (!makeWindow(x, y, w, h, title, resizable))
    {
        exitFunc();
        exit(4);
    }

    // Initialize OpenGL.
    createContext(multisample, num_samples, fullScreen);

    if (fullScreen)
    {
        sleepTime = 0;

        raydium_log("Entering %ix%i:%i fullscreen mode", horScrSze, verScrSze, 32);

        aglSetCurrentContext(currContext);
        aglSetFullScreen(currContext, 0, 0, 0, 0);

        // Initialise origin and size of the screen.
        origin[0] = 0;
        origin[1] = 0;
        size[0] = horScrSze;
        size[1] = verScrSze;
    }
    else
    {
        // Attach the context to the window.
        aglSetDrawable(currContext, GetWindowPort(GRAFPTR window));
        aglSetCurrentContext(currContext);

        // Create a default menu bar with the quit command.
        makeMenu();

        // Initialise origin and size of the window.
        origin[0] = (horScrSze/2)-(w/2);
        origin[1] = (verScrSze/2)-(h/2);
        size[0] = w;
        size[1] = h;

        MoveWindow(GRAFPTR window, origin[0], origin[1], false);
    }

    aglUpdateContext(currContext);

    _glutWindowSize[0] = size[0];
    _glutWindowSize[1] = size[1];
    _glutWindowInverseRatio = 0;

    glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers();
    glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
}

void glutSetCursor(int c)
{
    int csr = 0xFFFF;
    Cursor* csrPtr;

    switch (c)
    {
        case GLUT_CURSOR_NONE:
            HideCursor();
            _glutMouseVisible = 0;
            return;
        case GLUT_CURSOR_LEFT_ARROW:
            csr = kThemeArrowCursor;
            break;
        default:
            csr = kThemeArrowCursor;
            raydium_log("(my)glut: unavailable cursor type");
    }

    if (csr==0xFFFF)
        SetCursor(csrPtr);
    else
        SetThemeCursor(csr);

    ShowCursor();
    _glutMouseVisible = 1;
}

void glutWarpPointer(int x, int y)
{
    GrafPtr origPort;
    CGPoint pos;

    if (fullScreen)
    {
        x = x - 3;
        y = y - 3;
    }

    pos.x = origin[0] + x;
    pos.y = origin[1] + y;

    GetPort(&origPort);
    SETPORT(window);

    CGSetLocalEventsSuppressionInterval(0);
    CGWarpMouseCursorPosition(pos);

    SetPort(origPort);
}

void glutSwapBuffers(void)
{
    if (!initialized)
    {
        raydium_log("(my)glut: call to glutSwapBuffers without call to pwInit");
        return;
    }

    aglSwapBuffers(currContext);
}

// glutMainLoop is generic (myglut.c)

void myglutGetEvents(void)
{
    handleEvents();
}

void myglutCloseWindow(void)
{
    if (!initialized)
    {
        raydium_log("(my)glut: call to myglutCloseWindow without call to pwInit");
        return;
    }

    if (currContext != NULL)
    {
        aglSetDrawable(currContext, NULL);
        aglSetCurrentContext(NULL);
        aglDestroyContext(currContext);
    }

    if (window != NULL) DisposeWindow(GRAFPTR window);

    initialized = false;

    exit(0);
}
