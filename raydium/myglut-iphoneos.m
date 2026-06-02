/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/EAGLDrawable.h>

#include "myglut-iphoneos.h"

#define ACCEL_SMOOTH_STEPS 3
#define ACCEL_SAMPLE_FREQ  (1.f/30)

static int raydium_argc;
static char **raydium_argv;
extern int raydium_main(int argc, char **argv);
extern int _glutWindowSize[2];
extern signed char _glutWindowInverseRatio;

jmp_buf raydium_environment;
extern jmp_buf *raydium_jump_environment();

static id MyGLUTContext;
static int MyGLUTRenderbuffer;
static int MyGLUTFramebuffer;
static float MyGLUTAcceleration[3];
static int MyGLUTTouch[3];
static int MyGLUTContentsScale;

/*
    ##########################
    #### OpenGL ES UIView ####
    ##########################
*/

@interface MyGLUTView: UIView
{
    @private
        GLint backingWidth;
        GLint backingHeight;
        
        EAGLContext *context;
        GLuint viewRenderbuffer;
        GLuint viewFramebuffer;
        
        GLuint depthRenderbuffer;
}

- (BOOL) createFramebuffer;
- (void) destroyFramebuffer;

- (void) touchesBegan: (NSSet*) touches withEvent: (UIEvent*) event;
- (void) touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event;
- (void) touchesCancelled: (NSSet*) touches withEvent: (UIEvent*) event;
- (void) touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event;

@end

@implementation MyGLUTView

+ (Class) layerClass
{
    return [CAEAGLLayer class];
}

- (id) initWithFrame: (CGRect) frame
{
#ifndef IPHONEOS_ORIENTATION_PORTRAIT
    _glutWindowInverseRatio=1;
    int w=480;
    int h=320;
#else
    _glutWindowInverseRatio=0;
    int w=320;
    int h=480;
#endif
#ifndef IPHONEOS_NORETINA
    float ver = [[[UIDevice currentDevice] systemVersion] floatValue];
    if(ver>3.2)
        {
        int ww,hh;
        UIScreen* mainscr = [UIScreen mainScreen];
        ww = mainscr.currentMode.size.height;
        hh = mainscr.currentMode.size.width;
#ifndef IPHONEOS_ORIENTATION_PORTRAIT
        if(ww>hh)
	    { w=ww; h=hh; }
	else
	    { w=hh; h=ww; } // iPad 2 seems to reverse a few things ...
#else
        if(ww<hh)
	    { w=ww; h=hh; }
	else
	    { w=hh; h=ww; }
#endif
        }
#endif

    _glutWindowSize[0]=w;
    _glutWindowSize[1]=h;
    MyGLUTContentsScale=1;

    self = [super initWithFrame: frame];
    
    if (self != nil)
    {
        CAEAGLLayer *MyGLUTLayer = (CAEAGLLayer*) [self layer];
        MyGLUTLayer.opaque = YES;
        MyGLUTLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys: [NSNumber numberWithBool: NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGB565, kEAGLDrawablePropertyColorFormat, nil];
#ifndef IPHONEOS_ORIENTATION_PORTRAIT
        if(_glutWindowSize[1]==640)
#else
        if(_glutWindowSize[0]==640)
#endif
            {
            MyGLUTLayer.contentsScale=2;
            MyGLUTContentsScale=MyGLUTLayer.contentsScale;
            }
        context = [[EAGLContext alloc] initWithAPI: kEAGLRenderingAPIOpenGLES1];
        [EAGLContext setCurrentContext: context];
        MyGLUTContext = context;
        
        [self createFramebuffer];
        [context renderbufferStorage: GL_RENDERBUFFER_OES fromDrawable: MyGLUTLayer];
    }


    self.userInteractionEnabled = YES;
    self.multipleTouchEnabled = NO;
    self.exclusiveTouch = YES;
    
    return self;
}

- (void) layoutSubviews
{
    [EAGLContext setCurrentContext: context];
    [self destroyFramebuffer];
    [self createFramebuffer];
}

- (BOOL) createFramebuffer
{
    glGenFramebuffersOES(1, &viewFramebuffer);
    glGenRenderbuffersOES(1, &viewRenderbuffer);
    
    MyGLUTRenderbuffer = viewRenderbuffer;
    MyGLUTFramebuffer = viewFramebuffer;
    
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, viewFramebuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, viewRenderbuffer);
    [context renderbufferStorage: GL_RENDERBUFFER_OES fromDrawable: (CAEAGLLayer*) self.layer];
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, viewRenderbuffer);
    
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &backingWidth);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &backingHeight);
    
    glGenRenderbuffersOES(1, &depthRenderbuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthRenderbuffer);
    glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, backingWidth, backingHeight);
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthRenderbuffer);
    
    if(glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES)
    {
        return NO;
    }
    
    return YES;
}

- (void) destroyFramebuffer
{
    glDeleteFramebuffersOES(1, &viewFramebuffer);
    viewFramebuffer = 0;
    glDeleteRenderbuffersOES(1, &viewRenderbuffer);
    viewRenderbuffer = 0;
    
    if(depthRenderbuffer)
    {
        glDeleteRenderbuffersOES(1, &depthRenderbuffer);
        depthRenderbuffer = 0;
    }
}

- (void) touchesBegan: (NSSet*) touches withEvent: (UIEvent*) event
{
    UITouch *touch = touches.anyObject;
    CGPoint touchPosition = [touch locationInView: self];

    MyGLUTTouch[0] = 1;

    // Swap the axes, because the content of the screen is in landscape mode.
#ifndef IPHONEOS_ORIENTATION_PORTRAIT
    MyGLUTTouch[1] = ((int)touchPosition.y*MyGLUTContentsScale);
    MyGLUTTouch[2] = (_glutWindowSize[1]-(int)touchPosition.x*MyGLUTContentsScale);
#else
    MyGLUTTouch[1] = ((int)touchPosition.x*MyGLUTContentsScale);
    MyGLUTTouch[2] = ((int)touchPosition.y*MyGLUTContentsScale);
#endif
}

- (void) touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event
{
    [self touchesBegan: touches withEvent: event];
    MyGLUTTouch[0] = 3;
}

- (void) touchesCancelled: (NSSet*) touches withEvent: (UIEvent*) event
{
    [self touchesEnded: touches withEvent: event];
}

- (void) touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event
{
    [self touchesBegan: touches withEvent: event];
    MyGLUTTouch[0] = 2;
}

@end

/*
    ###############################
    #### UIApplicationDelegate ####
    ###############################
*/

@interface MyGLUTDelegate: NSObject<UIApplicationDelegate, UIAccelerometerDelegate>
{
    UIWindow *window;
    NSTimer *timer;
}

@property (nonatomic, retain) UIWindow *window;

- (void) onTimer: (NSTimer*) timer;

@end

@implementation MyGLUTDelegate

@synthesize window;

- (void) applicationDidFinishLaunching: (UIApplication*) application
{
#ifndef IPHONEOS_ORIENTATION_PORTRAIT
    [[UIApplication sharedApplication] setStatusBarOrientation: UIInterfaceOrientationLandscapeRight animated: NO];
#endif
    [UIApplication sharedApplication].statusBarHidden = YES;
    [[NSFileManager defaultManager] changeCurrentDirectoryPath: [[NSBundle mainBundle] resourcePath]];
    
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    UIView *view = [[MyGLUTView alloc] initWithFrame: [[UIScreen mainScreen] bounds]];
    [window addSubview: view];
    [window makeKeyAndVisible];
    
    [[UIAccelerometer sharedAccelerometer] setUpdateInterval: ACCEL_SAMPLE_FREQ];
    [[UIAccelerometer sharedAccelerometer] setDelegate: self];
    
    timer = [NSTimer scheduledTimerWithTimeInterval: 0 target: self selector: @selector (onTimer:) userInfo: nil repeats: NO];
}

- (void) applicationWillTerminate: (UIApplication*) application
{
    exit(0);
    longjmp(*(raydium_jump_environment()), 1);
}

- (void) dealloc
{
    [window release];
    [super dealloc];
}

- (void) accelerometer: (UIAccelerometer*) accelerometer didAccelerate: (UIAcceleration*) acceleration
{
    static float accel[3][ACCEL_SMOOTH_STEPS];
    float smooth;
    int i,j;
    for(i=0;i<3;i++)
       for(j=0;j<ACCEL_SMOOTH_STEPS-1;j++)
           accel[i][j]=accel[i][j+1];

    // Swap the axes, because the content of the screen is in landscape mode.
#ifndef IPHONEOS_ORIENTATION_PORTRAIT
    accel[0][ACCEL_SMOOTH_STEPS-1]=-acceleration.y;
    accel[1][ACCEL_SMOOTH_STEPS-1]=acceleration.x;
    accel[2][ACCEL_SMOOTH_STEPS-1]=acceleration.z;
#else
    accel[0][ACCEL_SMOOTH_STEPS-1]=acceleration.x;
    accel[1][ACCEL_SMOOTH_STEPS-1]=acceleration.y;
    accel[2][ACCEL_SMOOTH_STEPS-1]=acceleration.z;
#endif


    for(i=0;i<3;i++)
       {
       smooth=0;
       for(j=0;j<ACCEL_SMOOTH_STEPS;j++)
           smooth+=accel[i][j];
       MyGLUTAcceleration[i]=smooth/ACCEL_SMOOTH_STEPS;
       }
}

- (void) onTimer: (NSTimer*) timer
{
    /*
        We don't want to see the last drawing from other applications while
        the engine is starting up.
    */
    myglutMakeCurrent();
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
    myglutSwapBuffers();
    
    int raydium_main_result_code = raydium_main(raydium_argc, raydium_argv);
    exit(raydium_main_result_code);
}

@end

jmp_buf *raydium_jump_environment()
{
    return &raydium_environment;
}

/*
    ################
    #### MyGLUT ####
    ################
*/

// Activate the context with the surface.
int myglutMakeCurrent(void)
{
   return NO == [EAGLContext setCurrentContext: MyGLUTContext];
}

// Swap buffers to show the drawing.
int myglutSwapBuffers(void)
{
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, MyGLUTRenderbuffer);
    return NO == [MyGLUTContext presentRenderbuffer: GL_RENDERBUFFER_OES];
}

// Let the event cycle run to prevent abrupt termination.
void myglutPumpEvents(void)
{
    if(setjmp(*raydium_jump_environment()) == 0)
    {
        signed int result;
        do
        {
            result = CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, TRUE);
        }
        while(result == kCFRunLoopRunHandledSource);
    }
}

// Get acceleration value of the device.
float myglutGetAcceleration(int pivot)
{
    return MyGLUTAcceleration[pivot];
}

// Get touch value of the screen.
int myglutGetTouch(int index)
{
    int value = MyGLUTTouch[index];
    MyGLUTTouch[0] = 0;
    return value;
}

// Emulate a warp pointer event to the given coordinates.
void myglutWarpPointer(int x, int y)
{
    MyGLUTTouch[0] = 2;
    MyGLUTTouch[1] = x;
    MyGLUTTouch[2] = y;
}

/*
    ###########################
    #### UIApplicationMain ####
    ###########################
*/

int main(int argc, char **argv)
{
    int i;
    
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    raydium_argc = argc;
    raydium_argv = (char**)malloc(argc*sizeof(char*));
    
    for(i=0;i<argc;i++)
    {
        raydium_argv[i] = malloc((strlen(argv[i])+1)*sizeof(char));
        strcpy(raydium_argv[i], argv[i]);
    }
    
    int UIApplicationMainResultCode = UIApplicationMain(argc, argv, nil, @"MyGLUTDelegate");
    
    for(i=0;i<raydium_argc;i++)
        free(raydium_argv[i]);
    free(raydium_argv);
    
    [pool release];
    
    return UIApplicationMainResultCode;
}
