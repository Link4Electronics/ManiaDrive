/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef DONT_INCLUDE_HEADERS
#include "index.h"
#else
#include "headers/joy.h"
#endif
#ifdef WIN32
// Need to add winmm lib to windows dll build
#include <mmsystem.h>
#endif
#ifdef MACOSX
#include <unistd.h>
#include <ctype.h>
#include <sysexits.h>
#include <mach/mach.h>
#include <mach/mach_error.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <Kernel/IOKit/hidsystem/IOHIDUsageTables.h>
#include <IOKit/hid/IOHIDLib.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <CoreFoundation/CoreFoundation.h>
#include <Carbon/Carbon.h>
#include <ForceFeedback/ForceFeedback.h>
#include <ForceFeedback/ForceFeedbackConstants.h>
#endif

// proto
int raydium_init_cli_option_default(char *option, char *value, char *default_value);

// This file needs a lot of work, again ...
// (will use Glut for windows joy support soon ...)

//#define joy_debug 1                   //do we display debug infos ?

#define JS_EVENT_BUTTON         0x01    /* button pressed/released */
#define JS_EVENT_AXIS           0x02    /* joystick moved */
#define JS_EVENT_INIT           0x80    /* initial state of device */
/*
                        // function                     3rd arg
#define JSIOCGAXES      // get number of axes           char
#define JSIOCGBUTTONS   // get number of buttons        char
#define JSIOCGVERSION   // get driver version           int
#define JSIOCGNAME(len) // get identifier string        char
#define JSIOCSCORR      // set correction values        &js_corr
#define JSIOCGCORR      // get correction values        &js_corr
*/

//int joy;                              //file handle

//struct input_event play;
//struct input_event stop;
//struct ff_effect effect;
int raydium_joy_event_handle;
#ifndef APPLE
#ifndef WIN32
struct ff_effect effect_tremble;
#else
int raydium_joy_win_id;
#endif
#else
#ifdef MACOSX
struct recElement
{
    // Unique value which identifies element.
    IOHIDElementCookie cookie;

    // Reported min value possible.
    long min;

    // Reported max value possible.
    long max;

    // Runtime variables used for auto-calibration.
    long minReport;
    long maxReport;

    // Next element in list.
    struct recElement *pNext;
};
typedef struct recElement recElement;

struct joystick_hwdata
{
    // Interface for force feedback, 0 = no ff.
    io_service_t ffservice;

    // Interface to device, NULL = no interface.
    IOHIDDeviceInterface **interface;

    // Name of product.
    char product[256];

    // Usage page from IOUSBHID Parser.h which defines general usage.
    long usage;

    // Usage within above page from IOUSBHID Parser.h which defines specific usage.
    long usagePage;

    // Number of axis (calculated, not reported by device).
    long axes;

    // Number of buttons (calculated, not reported by device).
    long buttons;

    // Number of hat switches (calculated, not reported by device).
    long hats;

    // Number of total elements (shouldbe total of above) (calculated, not reported by device).
    long elements;

    recElement *firstAxis;
    recElement *firstButton;
    recElement *firstHat;

    int removed;
    int uncentered;

    // Next device.
    struct joystick_hwdata *pNext;
};
typedef struct joystick_hwdata recDevice;

struct IOKit_Joystick
{
    // Device index.
    uint8_t index;

    // Joystick name.
    const char *name;

    // Number of axis controls on the joystick.
    int naxes;

    // Current axis states.
    int16_t *axes;

    // Number of hats on the joystick.
    int nhats;

    // Current hat states.
    uint8_t *hats;

    // Number of trackballs on the joystick
    int nballs;

    // Current ball motion deltas.
    struct balldelta
    {
        int dx;
        int dy;
    } *balls;

    // Number of buttons on the joystick.
    int nbuttons;

    // Current button states.
    uint8_t *buttons;

    // Driver dependent information.
    struct joystick_hwdata *hwdata;

    // Reference count for multiple opens.
    int ref_count;
};
typedef struct IOKit_Joystick IOKitJoystick;

// Linked list of all available devices.
static recDevice *gpDeviceList = NULL;

// The joystick/joypad device.
recDevice *device = NULL;
IOKitJoystick *joystick = NULL;

// Number of detected devices.
int detectedDevices = 0;
#endif
#endif
char effect_tremble_state=0;
unsigned long last_event;


void raydium_joy_init_vars(void)
{
memset(raydium_joy_button,0,RAYDIUM_JOY_MAX_BUTTONS);
raydium_joy_x=raydium_joy_y=raydium_joy_z=0.f;
raydium_joy_click=0;
strcpy(raydium_joy_name,"(none)");
}


void raydium_joy_key_emul(void)
{
if(raydium_joy) return;

if(raydium_key[GLUT_KEY_UP]) raydium_joy_y=1.f;
if(raydium_key[GLUT_KEY_DOWN]) raydium_joy_y=-1.f;

if(raydium_key[GLUT_KEY_LEFT]) raydium_joy_x=-1.f;
if(raydium_key[GLUT_KEY_RIGHT]) raydium_joy_x=1.f;

//buttons
}

#ifndef APPLE
#ifndef WIN32
int raydium_joy_process_event(struct js_event e)
{
    e.type &= ~JS_EVENT_INIT;
    switch(e.type)
    {
        case JS_EVENT_BUTTON:
                if(e.number<RAYDIUM_JOY_MAX_BUTTONS)
                {
                    if(e.value==1)
                    {
                        raydium_joy_click=e.number+1;
                        raydium_joy_button[e.number]=e.value;
                        #ifdef joy_debug
                        raydium_log("Button %d pushed",e.number);
                        #endif
                    }
                    else
                    {
                        // release
                        raydium_joy_button[e.number]=e.value;
                    }
                }
                break;
        case JS_EVENT_AXIS:
                if(e.number<RAYDIUM_JOY_MAX_AXIS)
                {
                    #ifdef joy_debug
                    raydium_log("Axis Moved: %i",e.value);
                    #endif
                    raydium_joy_axis[e.number]=e.value/(float)32767;
                    //here we invert values from the y axis: we want
                    //1 for up and -1 for down
                        if(e.value<0)
                        {
                          #ifdef joy_debug
                           if(e.number==1)raydium_log("Up");
                           if(e.number==0)raydium_log("Left");
                          #endif
                          if(e.number==2)
                          {
                           raydium_joy_z=e.value/(float)32767*-1;
                          }
                          if(e.number==1)
                          {
                           raydium_joy_y=e.value/(float)32767*-1;
                          }
                          if(e.number==0)
                          {
                           raydium_joy_x=e.value/(float)32767;
                          }
                        }
                        if(e.value>0)
                        {
                          #ifdef joy_debug
                           if(e.number==1)raydium_log("Down");
                           if(e.number==0)raydium_log("Right");
                          #endif
                          if(e.number==2)
                          {
                           raydium_joy_z=e.value/(float)32767*-1;
                          }
                          if(e.number==1)
                          {
                           raydium_joy_y=e.value/(float)32767*-1;
                          }
                          if(e.number==0)
                          {
                           raydium_joy_x=e.value/(float)32767;
                          }
                        }
                        if(e.value==0)
                        {
                          if(e.number==1)
                          {
                           raydium_joy_y=0.0;
                          }
                          if(e.number==0)
                          {
                           raydium_joy_x=0.0;
                          }
                        }
                }
                break;
    }
    return(e.type);
}
#endif
#else
#ifdef MACOSX
static void HIDReportErrorNum(char* strError, long numError)
{
    raydium_log("joy: %s (%i)", strError, numError);
}

static void HIDGetCollectionElements(CFMutableDictionaryRef deviceProperties, recDevice* pDevice);

// Returns current value for element, polling element.
// Will return 0 on error conditions which should be accounted for by application.
static int HIDGetElementValue(recDevice *pDevice, recElement *pElement)
{
    IOReturn result = kIOReturnSuccess;
    IOHIDEventStruct hidEvent;
    hidEvent.value = 0;

    if (NULL != pDevice && NULL != pElement && NULL != pDevice->interface)
    {
        result = (*(pDevice->interface))->getElementValue(pDevice->interface,
                                                          pElement->cookie,
                                                          &hidEvent);
        if (kIOReturnSuccess == result)
        {
            // Record min and max for auto calibration.
            if (hidEvent.value < pElement->minReport)
                pElement->minReport = hidEvent.value;
            if (hidEvent.value > pElement->maxReport)
                pElement->maxReport = hidEvent.value;
        }
    }

    // Auto user scale.
    return hidEvent.value;
}

static int HIDScaledCalibratedValue(recDevice *pDevice, recElement *pElement, long min, long max)
{
    float deviceScale = max - min;
    float readScale = pElement->maxReport - pElement->minReport;
    SInt32 value = HIDGetElementValue(pDevice, pElement);
    if (readScale == 0)
        return value;
    else
        return ((value - pElement->minReport) * deviceScale / readScale) + min;
}

static void HIDRemovalCallback(void* target, IOReturn result, void* refcon, void* sender)
{
    recDevice* device = (recDevice*) refcon;
    device->removed = 1;
    device->uncentered = 1;
}

// Create and open an interface to device, required prior to extracting values or building queues.
// Note: appliction now owns the device and must close and release it prior to exiting.
static IOReturn HIDCreateOpenDeviceInterface(io_object_t hidDevice, recDevice* pDevice)
{
    IOReturn result = kIOReturnSuccess;
    HRESULT plugInResult = S_OK;
    SInt32 score = 0;
    IOCFPlugInInterface** ppPlugInInterface = NULL;

    if (NULL == pDevice->interface)
    {
        result = IOCreatePlugInInterfaceForService(hidDevice,
                                                   kIOHIDDeviceUserClientTypeID,
                                                   kIOCFPlugInInterfaceID,
                                                   &ppPlugInInterface, &score);
        if (kIOReturnSuccess == result)
        {
            // Call a method of the intermediate plug-in to create the device interface.
            plugInResult = (*ppPlugInInterface)->QueryInterface(ppPlugInInterface,
                                                                CFUUIDGetUUIDBytes
                                                                (kIOHIDDeviceInterfaceID),
                                                                (void*)
                                                                &(pDevice->interface));
            if (S_OK != plugInResult)
                HIDReportErrorNum("could not query HID class device interface from plugInInterface",
                                  plugInResult);

            (*ppPlugInInterface)->Release(ppPlugInInterface);
        }
        else
        {
            HIDReportErrorNum("failed to create **plugInInterface via IOCreatePlugInInterfaceForService",
                              result);
        }
    }

    if (NULL != pDevice->interface)
    {
        result = (*(pDevice->interface))->open(pDevice->interface, 0);
        if (kIOReturnSuccess != result)
            HIDReportErrorNum("failed to open pDevice->interface via open",
                              result);
        else
        {
            (*(pDevice->interface))->setRemovalCallback(pDevice->interface,
                                                        HIDRemovalCallback,
                                                        pDevice, pDevice);
        }
    }

    return result;
}

// Closes and releases interface to device, should be done prior to exting application.
// Note: Will have no affect if device or interface do not exist
// application will "own" the device if interface is not closed
// (device may have to be plug and re-plugged in different location to get it working again without a restart)
static IOReturn HIDCloseReleaseInterface(recDevice* pDevice)
{
    IOReturn result = kIOReturnSuccess;

    if ((NULL != pDevice) && (NULL != pDevice->interface))
    {
        // Close the interface.
        result = (*(pDevice->interface))->close(pDevice->interface);
        if (kIOReturnNotOpen == result)
        {
            // Do nothing as device was not opened, thus can't be closed.
        }
        else if (kIOReturnSuccess != result)
            HIDReportErrorNum("failed to close IOHIDDeviceInterface",
                              result);
        // Release the interface.
        result = (*(pDevice->interface))->Release(pDevice->interface);
        if (kIOReturnSuccess != result)
            HIDReportErrorNum("failed to release IOHIDDeviceInterface",
                              result);
        pDevice->interface = NULL;
    }

    return result;
}

// Extracts actual specific element information from each element CF dictionary entry.
static void HIDGetElementInfo(CFTypeRef refElement, recElement* pElement)
{
    long number;
    CFTypeRef refType;

    refType = CFDictionaryGetValue(refElement, CFSTR(kIOHIDElementCookieKey));
    if (refType && CFNumberGetValue(refType, kCFNumberLongType, &number))
        pElement->cookie = (IOHIDElementCookie) number;

    refType = CFDictionaryGetValue(refElement, CFSTR(kIOHIDElementMinKey));
    if (refType && CFNumberGetValue(refType, kCFNumberLongType, &number))
        pElement->minReport = pElement->min = number;

    pElement->maxReport = pElement->min;

    refType = CFDictionaryGetValue(refElement, CFSTR(kIOHIDElementMaxKey));
    if (refType && CFNumberGetValue(refType, kCFNumberLongType, &number))
        pElement->maxReport = pElement->max = number;
}

// Examines CF dictionary vlaue in device element hierarchy to determine if it is element of interest or a collection of more elements
// if element of interest allocate storage, add to list and retrieve element specific info
// if collection then pass on to deconstruction collection into additional individual elements.
static void HIDAddElement(CFTypeRef refElement, recDevice* pDevice)
{
    recElement* element = NULL;
    recElement** headElement = NULL;
    long elementType, usagePage, usage;
    CFTypeRef refElementType =
    CFDictionaryGetValue(refElement, CFSTR(kIOHIDElementTypeKey));
    CFTypeRef refUsagePage = CFDictionaryGetValue(refElement, CFSTR(kIOHIDElementUsagePageKey));
    CFTypeRef refUsage = CFDictionaryGetValue(refElement, CFSTR(kIOHIDElementUsageKey));

    if ((refElementType) && (CFNumberGetValue(refElementType, kCFNumberLongType, &elementType)))
    {
        // Look at types of interest.
        if ((elementType == kIOHIDElementTypeInput_Misc)
            || (elementType == kIOHIDElementTypeInput_Button)
            || (elementType == kIOHIDElementTypeInput_Axis))
        {
            if (refUsagePage
                && CFNumberGetValue(refUsagePage, kCFNumberLongType, &usagePage)
                && refUsage
                && CFNumberGetValue(refUsage, kCFNumberLongType, &usage))
            {
                switch (usagePage)
                {
                    case kHIDPage_GenericDesktop:
                    {
                        switch (usage)
                        {
                            case kHIDUsage_GD_X:
                            case kHIDUsage_GD_Y:
                            case kHIDUsage_GD_Z:
                            case kHIDUsage_GD_Rx:
                            case kHIDUsage_GD_Ry:
                            case kHIDUsage_GD_Rz:
                            case kHIDUsage_GD_Slider:
                            case kHIDUsage_GD_Dial:
                            case kHIDUsage_GD_Wheel:
                                element = (recElement*)
                                NewPtrClear(sizeof(recElement));
                                if (element)
                                {
                                    pDevice->axes++;
                                    headElement = &(pDevice->firstAxis);
                                }
                                break;
                                case kHIDUsage_GD_Hatswitch:
                                element = (recElement*)
                                NewPtrClear(sizeof(recElement));
                                if (element)
                                {
                                    pDevice->hats++;
                                    headElement = &(pDevice->firstHat);
                                }
                                break;
                        }
                    }
                        break;
                    case kHIDPage_Button:
                        element = (recElement*)
                        NewPtrClear(sizeof(recElement));
                        if (element)
                        {
                            pDevice->buttons++;
                            headElement = &(pDevice->firstButton);
                        }
                        break;
                        default:
                        break;
                }
            }
        }
        else if (kIOHIDElementTypeCollection == elementType)
            HIDGetCollectionElements((CFMutableDictionaryRef) refElement,
                                     pDevice);
    }

    if (element && headElement)
    {
        pDevice->elements++;
        if (NULL == *headElement)
            *headElement = element;
        else
        {
            recElement* elementPrevious, *elementCurrent;
            elementCurrent = *headElement;
            while (elementCurrent)
            {
                elementPrevious = elementCurrent;
                elementCurrent = elementPrevious->pNext;
            }

            elementPrevious->pNext = element;
        }

        element->pNext = NULL;
        HIDGetElementInfo(refElement, element);
    }
}

// Collects information from each array member in device element list (each array memeber = element).
static void HIDGetElementsCFArrayHandler(const void* value, void* parameter)
{
    if (CFGetTypeID(value) == CFDictionaryGetTypeID())
        HIDAddElement((CFTypeRef) value, (recDevice*) parameter);
}

// Handles retrieval of element information from arrays of elements in device IO registry information.
static void HIDGetElements(CFTypeRef refElementCurrent, recDevice* pDevice)
{
    CFTypeID type = CFGetTypeID(refElementCurrent);

    if (type == CFArrayGetTypeID())
    {
        CFRange range = { 0, CFArrayGetCount(refElementCurrent) };
        // CountElementsCFArrayHandler called for each array member.
        CFArrayApplyFunction(refElementCurrent, range, HIDGetElementsCFArrayHandler, pDevice);
    }
}

// Handles extracting element information from element collection CF types
// used from top level element decoding and hierarchy deconstruction to flatten device element list.
static void HIDGetCollectionElements(CFMutableDictionaryRef deviceProperties, recDevice* pDevice)
{
    CFTypeRef refElementTop = CFDictionaryGetValue(deviceProperties, CFSTR(kIOHIDElementKey));
    if (refElementTop) HIDGetElements(refElementTop, pDevice);
}

// Use top level element usage page and usage to discern device usage page and usage setting appropriate vlaues in device record.
static void HIDTopLevelElementHandler(const void* value, void* parameter)
{
    CFTypeRef refCF = 0;

    if (CFGetTypeID(value) != CFDictionaryGetTypeID())
        return;

    refCF = CFDictionaryGetValue(value, CFSTR(kIOHIDElementUsagePageKey));
    if (!CFNumberGetValue(refCF, kCFNumberLongType, &((recDevice*) parameter)->usagePage))
        raydium_log("joy: CFNumberGetValue error retrieving pDevice->usagePage");

    refCF = CFDictionaryGetValue(value, CFSTR(kIOHIDElementUsageKey));
    if (!CFNumberGetValue(refCF, kCFNumberLongType, &((recDevice*) parameter)->usage))
        raydium_log("joy: CFNumberGetValue error retrieving pDevice->usage");
}

// Extracts device info from CF dictionary records in IO registry.
static void HIDGetDeviceInfo(io_object_t hidDevice, CFMutableDictionaryRef hidProperties, recDevice* pDevice)
{
    CFMutableDictionaryRef usbProperties = 0;
    io_registry_entry_t parent1, parent2;

    // Mac OS X currently is not mirroring all USB properties to HID page so need to look at USB device page also
    // get dictionary for usb properties: step up two levels and get CF dictionary for USB properties.
    if ((KERN_SUCCESS == IORegistryEntryGetParentEntry(hidDevice, kIOServicePlane, &parent1))
        && (KERN_SUCCESS == IORegistryEntryGetParentEntry(parent1, kIOServicePlane, &parent2))
        && (KERN_SUCCESS == IORegistryEntryCreateCFProperties(parent2, &usbProperties, kCFAllocatorDefault, kNilOptions)))
    {
        if (usbProperties)
        {
            CFTypeRef refCF = 0;
            // Get device info
            // Try hid dictionary first, if fail then go to usb dictionary.

            // Get product name.
            refCF = CFDictionaryGetValue(hidProperties, CFSTR(kIOHIDProductKey));
            if (!refCF) refCF = CFDictionaryGetValue(usbProperties, CFSTR("USB Product Name"));

            if (refCF)
            {
                if (!CFStringGetCString(refCF, pDevice->product, 256, CFStringGetSystemEncoding()))
                    raydium_log("joy: CFStringGetCString error retrieving pDevice->product");
            }

            // Get usage page and usage.
            refCF = CFDictionaryGetValue(hidProperties, CFSTR(kIOHIDPrimaryUsagePageKey));
            if (refCF)
            {
                if (!CFNumberGetValue(refCF, kCFNumberLongType, &pDevice->usagePage))
                    raydium_log("joy: CFNumberGetValue error retrieving pDevice->usagePage");

                refCF = CFDictionaryGetValue(hidProperties, CFSTR(kIOHIDPrimaryUsageKey));
                if (refCF)
                    if (!CFNumberGetValue(refCF, kCFNumberLongType, &pDevice->usage))
                        raydium_log("joy: CFNumberGetValue error retrieving pDevice->usage");
            }

            // Get top level element HID usage page or usage.
            if (NULL == refCF)
            {
                // Use top level element instead.
                CFTypeRef refCFTopElement = 0;
                refCFTopElement = CFDictionaryGetValue(hidProperties, CFSTR(kIOHIDElementKey));
                {
                    // RefCFTopElement points to an array of element dictionaries.
                    CFRange range = { 0, CFArrayGetCount(refCFTopElement) };
                    CFArrayApplyFunction(refCFTopElement, range, HIDTopLevelElementHandler, pDevice);
                }
            }

            CFRelease(usbProperties);
        } else raydium_log("joy: IORegistryEntryCreateCFProperties failed to create usbProperties");

        if (kIOReturnSuccess != IOObjectRelease(parent2))
            raydium_log("joy: IOObjectRelease error with parent2");
        if (kIOReturnSuccess != IOObjectRelease(parent1))
            raydium_log("joy: IOObjectRelease error with parent1");
    }
}

static recDevice* HIDBuildDevice(io_object_t hidDevice)
{
    recDevice* pDevice = (recDevice*) NewPtrClear(sizeof(recDevice));
    if (pDevice)
    {
        // Get dictionary for HID properties.
        CFMutableDictionaryRef hidProperties = 0;
        kern_return_t result = IORegistryEntryCreateCFProperties(hidDevice,
                                                                 &hidProperties,
                                                                 kCFAllocatorDefault,
                                                                 kNilOptions);
        if ((result == KERN_SUCCESS) && hidProperties)
        {
            // Create device interface.
            result = HIDCreateOpenDeviceInterface(hidDevice, pDevice);
            if (kIOReturnSuccess == result)
            {
                // HIDDevice used to find parents in registry tree.
                HIDGetDeviceInfo(hidDevice, hidProperties, pDevice);
                HIDGetCollectionElements(hidProperties, pDevice);
            }
            else
            {
                DisposePtr((Ptr) pDevice);
                pDevice = NULL;
            }

            CFRelease(hidProperties);
        }
        else
        {
            DisposePtr((Ptr) pDevice);
            pDevice = NULL;
        }
    }

    return pDevice;
}

// Disposes of the element list associated with a device and the memory associated with the list.
static void HIDDisposeElementList(recElement** elementList)
{
    recElement* pElement = *elementList;
    while (pElement)
    {
        recElement* pElementNext = pElement->pNext;
        DisposePtr((Ptr) pElement);
        pElement = pElementNext;
    }

    *elementList = NULL;
}

// Disposes of a single device, closing and releaseing interface, freeing memory fro device and elements, setting device pointer to NULL.
// All your device no longer belong to us... (i.e., you do not 'own' the device anymore).
static recDevice* HIDDisposeDevice(recDevice** ppDevice)
{
    kern_return_t result = KERN_SUCCESS;
    recDevice *pDeviceNext = NULL;
    if (*ppDevice)
    {
        // Save next device prior to disposing of this device.
        pDeviceNext = (*ppDevice)->pNext;

        // Free posible io_service_t.
        if ((*ppDevice)->ffservice)
        {
            IOObjectRelease((*ppDevice)->ffservice);
            (*ppDevice)->ffservice = 0;
        }

        // Free element lists.
        HIDDisposeElementList(&(*ppDevice)->firstAxis);
        HIDDisposeElementList(&(*ppDevice)->firstButton);
        HIDDisposeElementList(&(*ppDevice)->firstHat);

        // Function sanity checks interface value (now application does not own device).
        result = HIDCloseReleaseInterface(*ppDevice);
        if (kIOReturnSuccess != result)
            HIDReportErrorNum("HIDCloseReleaseInterface failed when trying to dipose device",
                              result);
        DisposePtr((Ptr)* ppDevice);
        *ppDevice = NULL;
    }

    return pDeviceNext;
}
#endif
#endif

void raydium_joy_callback(void)
{
if(raydium_joy_emul_type==RAYDIUM_JOY_EMUL_KEY)
    {
    raydium_joy_x=0;
    raydium_joy_y=0;

    if(raydium_key[GLUT_KEY_UP]) raydium_joy_y=1.f;
    if(raydium_key[GLUT_KEY_DOWN]) raydium_joy_y=-1.f;

    if(raydium_key[GLUT_KEY_LEFT]) raydium_joy_x=-1.f;
    if(raydium_key[GLUT_KEY_RIGHT]) raydium_joy_x=1.f;

    raydium_joy_axis[0]=raydium_joy_x;
    raydium_joy_axis[1]=raydium_joy_y;

    return;
    }

if(raydium_joy_emul_type==RAYDIUM_JOY_EMUL_MOUSE)
    {
    raydium_joy_x=0;
    raydium_joy_y=0;

    if(raydium_mouse_button[0])
        {
        raydium_joy_x=((float)raydium_mouse_x/raydium_window_tx-0.5f)*2.f;
        raydium_joy_y=((float)raydium_mouse_y/raydium_window_ty-0.5f)*-2.f;
        }

    raydium_joy_axis[0]=raydium_joy_x;
    raydium_joy_axis[1]=raydium_joy_y;

    return;
    }

#ifndef APPLE
#ifndef WIN32
 struct js_event e;                     //structure for storing an event

        if(!raydium_joy) { raydium_joy_init_vars(); return; }
        raydium_joy_click=0;

        while (read (raydium_joy, &e, sizeof(struct js_event)) > 0)
        {
            raydium_joy_process_event (e);
            //raydium_log("joy_DEBUG number:%d, value:%d",e.number,e.value);
        }
#else
        JOYINFOEX ActualPos;
        int i;
        unsigned int mask=1;
        static DWORD dwButtons=0; // saves buttons state between calls

        if(!raydium_joy) { raydium_joy_init_vars(); return; }
        raydium_joy_click=0;

        memset(&ActualPos,0,sizeof(JOYINFOEX));
        ActualPos.dwSize = sizeof(JOYINFOEX);
        ActualPos.dwFlags = JOY_RETURNALL;

        joyGetPosEx(raydium_joy_win_id,&ActualPos);
        raydium_joy_x=(ActualPos.dwXpos/(float)32768-1);
        raydium_joy_y=(ActualPos.dwYpos/(float)32768-1)*-1;
        raydium_joy_z=(ActualPos.dwZpos/(float)32768-1)*-1;

        // "raw" values
        raydium_joy_axis[0]=ActualPos.dwXpos/(float)32768-1;
        raydium_joy_axis[1]=ActualPos.dwYpos/(float)32768-1;
        raydium_joy_axis[2]=ActualPos.dwZpos/(float)32768-1;
        raydium_joy_axis[3]=ActualPos.dwRpos/(float)32768-1;
        raydium_joy_axis[4]=ActualPos.dwUpos/(float)32768-1;
        raydium_joy_axis[5]=ActualPos.dwVpos/(float)32768-1;

        for (i=0;i<RAYDIUM_JOY_MAX_BUTTONS;i++)
        {
            if (ActualPos.dwButtons & mask)
            {
                if(!(dwButtons & mask))
                        raydium_joy_click=i+1;
                raydium_joy_button[i]=1;

            }
            else
                raydium_joy_button[i]=0;

            mask=mask<<1;
        }
        dwButtons = ActualPos.dwButtons;
#endif
#else
#ifdef MACOSX
    if (!raydium_joy)
    {
        raydium_joy_init_vars();
        return;
    }

    raydium_joy_click = 0;

    recElement *element;
    int value, range;
    int i;

    if (device && joystick)
    {
        // Device was unplugged, ignore it.
        if (device->removed)
        {
            if (device->uncentered)
            {
                device->uncentered = 0;
            }

            // Tell the app that everything is centered.
            raydium_joy_x = 0.0;
            raydium_joy_axis[0] = 0.0;

            raydium_joy_y = 0.0;
            raydium_joy_axis[1] = 0.0;

            raydium_joy_z = 0.0;
            raydium_joy_axis[2] = 0.0;

            return;
        }

        element = device->firstAxis;
        i = 0;
        while (element)
        {
            value = HIDScaledCalibratedValue(device, element, -32768, 32767);
            switch (i)
            {
                case 0:
                    raydium_joy_x = value / (float)32767;
                    raydium_joy_axis[0] = value / (float)32767;
                    break;
                case 1:
                    raydium_joy_y = value / (float)32767 * -1;
                    raydium_joy_axis[1] = value / (float)32767;
                    break;
                case 2:
                    raydium_joy_z = value / (float)32767 * -1;
                    raydium_joy_axis[2] = value / (float)32767;
                    break;
                default:
                    raydium_joy_x = 0.0;
                    raydium_joy_axis[0] = 0.0;

                    raydium_joy_y = 0.0;
                    raydium_joy_axis[1] = 0.0;

                    raydium_joy_z = 0.0;
                    raydium_joy_axis[2] = 0.0;
                    break;
            }
            element = element->pNext;
            ++i;
        }

        element = device->firstButton;
        i = 0;
        while (element)
        {
            value = HIDGetElementValue(device, element);
            if (value > 1) value = 1;
            raydium_joy_button[i] = value;
            if (value == 1)
            {
                raydium_joy_click = i + 1;
                #ifdef joy_debug
                raydium_log("joy: button %i %s", i + 1, "pressed");
                #endif
            }
            element = element->pNext;
            ++i;
        }

        element = device->firstHat;
        i = 0;
        while (element)
        {
            uint8_t pos = 0;

            range = (element->max - element->min + 1);
            value = HIDGetElementValue(device, element) - element->min;
            // 4 position hatswitch - scale up value.
            if (range == 4)
                value *= 2;
            // Neither a 4 nor 8 positions - fall back to default position (centered).
            else if (range != 8)
                value = -1;
            switch (value)
            {
                case 0:
                    // Up
                    pos = 0x01;
                    #ifdef joy_debug
                    raydium_log("joy: hatswitch up");
                    #endif
                    break;
                case 1:
                    // Right up
                    pos = (0x02 | 0x01);
                    #ifdef joy_debug
                    raydium_log("joy: hatswitch right up");
                    #endif
                    break;
                case 2:
                    // Right
                    pos = 0x02;
                    #ifdef joy_debug
                    raydium_log("joy: hatswitch right");
                    #endif
                    break;
                case 3:
                    // Right down
                    pos = (0x02 | 0x04);
                    #ifdef joy_debug
                    raydium_log("joy: hatswitch right down");
                    #endif
                    break;
                case 4:
                    // Down
                    pos = 0x04;
                    #ifdef joy_debug
                    raydium_log("joy: hatswitch down");
                    #endif
                    break;
                case 5:
                    // Left down
                    pos = (0x08 | 0x04);
                    #ifdef joy_debug
                    raydium_log("joy: hatswitch left down");
                    #endif
                    break;
                case 6:
                    // Left
                    pos = 0x08;
                    #ifdef joy_debug
                    raydium_log("joy: hatswitch left");
                    #endif
                    break;
                case 7:
                    // Left up
                    pos = (0x08 | 0x01);
                    #ifdef joy_debug
                    raydium_log("joy: hatswitch left up");
                    #endif
                    break;
                default:
                    // Every other value is mapped to center. We do that because some
                    // joysticks use 8 and some 15 for this value, and apparently
                    // there are even more variants out there - so we try to be generous.
                    pos = 0x00;
                    break;
            }

            element = element->pNext;
            ++i;
        }
    }
#endif
#ifdef IPHONEOS
    if (!raydium_joy)
    {
        raydium_joy_init_vars();
        return;
    }

    raydium_joy_click = 0;

    raydium_joy_axis[0] = raydium_joy_x = myglutGetAcceleration(0);
    raydium_joy_axis[1] = raydium_joy_y = myglutGetAcceleration(1);
    raydium_joy_axis[2] = raydium_joy_z = myglutGetAcceleration(2);
#endif
#endif
//raydium_log("Joy x=%f,y=%f,z=%f",raydium_joy_x,raydium_joy_y,raydium_joy_z);
}

void raydium_joy_ff_autocenter(int perc)
{
#ifndef APPLE
#ifndef WIN32
struct input_event ie;

if(raydium_joy_event_handle<0) return;

ie.type = EV_FF;
ie.code = FF_AUTOCENTER;
ie.value = 0xFFFFUL * perc / 100;

if (write(raydium_joy_event_handle, &ie, sizeof(ie)) == -1)
        perror("set auto-center");

#endif
#endif
}


void raydium_joy_init(void)
{
#ifndef APPLE
#ifndef WIN32
 int ret;                                       //test var for ioctls
#endif
#endif
 char name[128];                                //driver String (and temp things)

int autocenter=5;         /* default value. between 0 and 100 */

raydium_joy_init_vars();

// is any emulation requested ?
raydium_joy_emul_type=RAYDIUM_JOY_EMUL_NONE;
name[0]=0;
if(!raydium_joy && raydium_init_cli_option("joy-emul",name))
    {
    if(!strcmp(name,"keyboard"))
        {
        raydium_joy_emul_type=RAYDIUM_JOY_EMUL_KEY;
        raydium_joy = 1;
        raydium_joy_n_axes = 2;
        raydium_joy_n_buttons = 0; // may change, obviously ...
        strcpy(raydium_joy_name, "Raydium joystick by keyboard emulation");
        }
    if(!strcmp(name,"mouse"))
        {
        raydium_joy_emul_type=RAYDIUM_JOY_EMUL_MOUSE;
        raydium_joy = 1;
        raydium_joy_n_axes = 2;
        raydium_joy_n_buttons = 0; // may change, obviously ...
        strcpy(raydium_joy_name, "Raydium joystick by mouse emulation");
        }

    if(raydium_joy)
        {
        raydium_log("joy: OK (emulated: %s)",raydium_joy_name);
        return;
        }
    }

#ifndef APPLE
#ifndef WIN32

    raydium_init_cli_option_default("joydev",name,"/dev/js0");
    raydium_joy=open(name,O_RDONLY|O_NONBLOCK);
    if(raydium_joy==-1)
    {
            raydium_joy=open("/dev/input/js0",O_RDONLY|O_NONBLOCK);
        }

    if(raydium_joy==-1)
        {
                raydium_log("joy: FAILED (cannot open %s)",name);
                raydium_joy=0;
        }
        else
        {
                raydium_log("joy: OK (found)");
        }

        raydium_init_cli_option_default("evdev",name,"/dev/input/event0");

        raydium_joy_event_handle = open(name, O_RDWR);
        if(raydium_joy_event_handle==-1)
          raydium_log("%s: cannot open (rw), no Force Feedback.",name);
        last_event=raydium_timecall_clock();

        raydium_joy_ff_autocenter(autocenter);



        if(raydium_joy)
        {
                ret=ioctl (raydium_joy,JSIOCGNAME(sizeof(name)),name);
                if(ret==-1)
                {
                        raydium_log("Error reading joystick driver's signature");
                        strncpy(name,"Unknown",sizeof(name));
                }
                else
                {
                        raydium_log("Joystick driver's signature: %s",name);
                        strcpy(raydium_joy_name,name);
                }

                ret=ioctl (raydium_joy,JSIOCGAXES,&raydium_joy_n_axes);
                if(ret==-1)
                {
                        raydium_log("Error reading number of axes");
                }
                else
                {
                        raydium_log("This joystick has %d axes",raydium_joy_n_axes);
                }

                ret=ioctl (raydium_joy,JSIOCGBUTTONS,&raydium_joy_n_buttons);
                if(ret==-1)
                {
                        raydium_log("Error reading number of buttons");
                }
                else
                {
                        raydium_log("This joystick has %d buttons",raydium_joy_n_buttons);
                }

        // Linux blacklist:
        // Microsoft Wired Keyboard 600 create a spurious joystick device
        if(!strcmp(raydium_joy_name,"Microsoft Wired Keyboard 600"))
            {
            raydium_log("Joystick blacklisted ! Now disabled.");
            raydium_joy=0;
            }

        }

#else
    {
        JOYINFO structtmp;

        raydium_init_cli_option_default("joydev",name,"0");

        raydium_joy_win_id=atoi(name);

        if (joyGetPos(raydium_joy_win_id,&structtmp) == JOYERR_NOERROR)
          raydium_joy=1;
        else
        {
            raydium_joy=0;
            raydium_log ("Joystick %d not connected",raydium_joy_win_id);
            return;
        }

        raydium_joy_ff_autocenter(autocenter);
        raydium_log("joy: OK (found %d)",raydium_joy_win_id);

        if(raydium_joy)
        {
            JOYCAPS InfoCaps;

            if (joyGetDevCaps(raydium_joy_win_id, &InfoCaps, sizeof(JOYCAPS)) == JOYERR_NOERROR)
            {
                raydium_log("Joystick: %s", InfoCaps.szPname);
                strcpy(raydium_joy_name,InfoCaps.szPname);

                raydium_joy_n_axes=InfoCaps.wNumAxes;
                raydium_log("This joystick has %d axes",raydium_joy_n_axes);


                raydium_joy_n_buttons=InfoCaps.wNumButtons;
                raydium_log("This joystick has %d buttons",raydium_joy_n_buttons);
             }
        }
    }
#endif
#else
#ifdef MACOSX
    raydium_init_cli_option_default("joydev", name, "0");

    IOReturn result = kIOReturnSuccess;
    mach_port_t masterPort = 0;
    io_iterator_t hidObjectIterator = 0;
    CFMutableDictionaryRef hidMatchDictionary = NULL;
    recDevice* lastDevice;
    io_object_t ioHIDDeviceObject = 0;
    int requestedDevice = 0;

    if (raydium_joy || gpDeviceList)
    {
        raydium_log("joy: device list already inited");
        return;
    }

    result = IOMasterPort(bootstrap_port, &masterPort);

    if (kIOReturnSuccess != result)
    {
        raydium_log("joy: IOMasterPort error with bootstrap_port");
        return;
    }

    // Set up a matching dictionary to search I/O Registry by class name for all HID class devices.
    hidMatchDictionary = IOServiceMatching(kIOHIDDeviceKey);
    if (!hidMatchDictionary)
    {
        raydium_log("joy: failed to get HID CFMutableDictionaryRef via IOServiceMatching");
        return;
    }

    // Now search I/O Registry for matching devices.
    result = IOServiceGetMatchingServices(masterPort, hidMatchDictionary, &hidObjectIterator);
    if (kIOReturnSuccess != result)
    {
        raydium_log("joy: could not create a HID object iterator");
        return;
    }

    if (!hidObjectIterator)
    {
        // There are no joysticks.
        raydium_log("joy: FAILED (no device found)");
        device = NULL;
        joystick = NULL;
        raydium_joy = 0;
        return;
    }

    // IOServiceGetMatchingServices consumes a reference to the dictionary,
    // so we don't need to release the dictionary ref.

    // Build flat linked list of devices from device iterator.
    gpDeviceList = lastDevice = NULL;

    while ((ioHIDDeviceObject = IOIteratorNext(hidObjectIterator)))
    {
        // Build a device record.
        device = HIDBuildDevice(ioHIDDeviceObject);
        if (!device) continue;

        // Filter device list to non-keyboard/mouse stuff.
        if ((device->usagePage != kHIDPage_GenericDesktop) ||
            ((device->usage != kHIDUsage_GD_Joystick &&
              device->usage != kHIDUsage_GD_GamePad &&
              device->usage != kHIDUsage_GD_MultiAxisController)))
        {
            // Release memory for the device.
            HIDDisposeDevice(&device);
            DisposePtr((Ptr) device);
            continue;
        }

        // We have to do some storage of the io_service_t.
        if (FFIsForceFeedback(ioHIDDeviceObject) == FF_OK)
        {
            device->ffservice = ioHIDDeviceObject;
        }
        else
        {
            device->ffservice = 0;
        }

        // Add device to the end of the list.
        if (lastDevice)
            lastDevice->pNext = device;
        else
            gpDeviceList = device;

        lastDevice = device;
    }

    // Release the iterator.
    result = IOObjectRelease(hidObjectIterator);

    // Count the total number of devices we found.
    device = gpDeviceList;

    while(device)
    {
        detectedDevices++;
        device = device->pNext;
    }

    if (detectedDevices < 1)
    {
        // There are no joysticks.
        raydium_log("joy: FAILED (no device found)");
        device = NULL;
        joystick = NULL;
        raydium_joy = 0;
        return;
    }

    raydium_log("joy: OK (found %i)", detectedDevices);

    // Print information about the devices.
    device = gpDeviceList;
    int detectedDevice = 0;

    while(device)
    {
        raydium_log("joy: #%i %s (%i axes, %i buttons, %i hats)",
                    detectedDevice,
                    device->product,
                    device->axes,
                    device->buttons,
                    device->hats);

        detectedDevice++;
        device = device->pNext;
    }

    if (detectedDevices > 1)
    {
        // Let the user choose the device.
        requestedDevice = atoi(name);
        device = gpDeviceList;
        detectedDevice = 0;

        while(device)
        {
            if (detectedDevice == requestedDevice) break;
            detectedDevice++;
            device = device->pNext;
        }
    }
    else
    {
        // Use the first device in list.
        device = gpDeviceList;
    }

    // Create and initialize the joystick.
    joystick = (IOKitJoystick*) malloc((sizeof *joystick));

    if (joystick != NULL)
    {
        memset(joystick, 0, (sizeof *joystick));
        joystick->index = 0;
        if (joystick->naxes > 0)
        {
            joystick->axes = (int16_t*) malloc
            (joystick->naxes * sizeof(int16_t));
        }
        if (joystick->nhats > 0)
        {
            joystick->hats = (uint8_t*) malloc
            (joystick->nhats * sizeof(uint8_t));
        }
        if (joystick->nballs > 0)
        {
            joystick->balls = (struct balldelta*) malloc
            (joystick->nballs * sizeof(*joystick->balls));
        }
        if (joystick->nbuttons > 0)
        {
            joystick->buttons = (uint8_t*) malloc
            (joystick->nbuttons * sizeof(uint8_t));
        }
        if (joystick->axes)
        {
            memset(joystick->axes, 0, joystick->naxes * sizeof(int16_t));
        }
        if (joystick->hats)
        {
            memset(joystick->hats, 0, joystick->nhats * sizeof(uint8_t));
        }
        if (joystick->balls)
        {
            memset(joystick->balls, 0, joystick->nballs * sizeof(*joystick->balls));
        }
        if (joystick->buttons)
        {
            memset(joystick->buttons, 0, joystick->nbuttons * sizeof(uint8_t));
        }
    }

    joystick->hwdata = device;
    joystick->name = device->product;

    joystick->naxes = device->axes;
    joystick->nhats = device->hats;
    joystick->nballs = 0;
    joystick->nbuttons = device->buttons;

    strcpy(raydium_joy_name, device->product);
    raydium_joy_n_axes = device->axes;
    raydium_joy_n_buttons = device->buttons;

    raydium_log("joy: using device #%i (%i elements)", requestedDevice, device->elements);

    // Enable joystick/joypad support.
    raydium_joy = 1;
#endif
#ifdef IPHONEOS
    raydium_log("joy: OK (using internal multi-axis accelerometer motion sensor)");
    raydium_joy = 1;
    raydium_joy_n_axes = 3;
    raydium_joy_n_buttons = 0;
    strcpy(raydium_joy_name, "iPhone internal multi-axis accelerometer motion sensor");
#endif
#endif
}

void raydium_joy_close(void)
{
#ifndef WIN32
        if(raydium_joy) close(raydium_joy);
#endif
}

void raydium_joy_ff(void)
{
//memset(&effect,0,sizeof(struct ff_effect));
// effect.type=FF_CONSTANT;
// effect.u.constant.level=0xEFF0;
// effect.u.constant.direction=0x2000;
// effect.replay.length=65535;
// effect.u.constant.shape.attack_length=40000;
// effect.u.constant.shape.attack_level=0x8888;
// effect.u.constant.shape.fade_length=20000;
// effect.u.constant.shape.fade_level=0x2222;
/*      effect.type = FF_SPRING;
        effect.u.interactive.direction=0x4000;
        effect.u.interactive.right_saturation = 32767;
        effect.u.interactive.left_saturation = 32767;
        effect.u.interactive.right_coeff = 32767;
        effect.u.interactive.left_coeff = 32767;
        effect.u.interactive.deadband = 0x0;
        effect.u.interactive.center = 0x0;*/
//    effect.type = FF_PERIODIC;
/*
#define FF_SQUARE       0x58
#define FF_TRIANGLE     0x59
#define FF_SINE         0x5a
#define FF_SAW_UP       0x5b
#define FF_SAW_DOWN     0x5c
#define FF_CUSTOM       0x5d
*/
/*    effect.u.periodic.waveform=FF_TRIANGLE;
    effect.u.periodic.period=50;
    effect.u.periodic.magnitude=16000;
    effect.u.periodic.direction=0x4000;*/






// raydium_log("upload effect valeur=%d", ioctl(fd, EVIOCSFF, &effect));
// perror("merde:");

// play.type = EV_FF;
// play.code = effect.id;
//raydium_log("effect_id=%d",effect.id);
// play.value = 10000;
// write(fd, (const void*) &play, sizeof(play));

/* stop.type = EV_FF;
 stop.code = effect.id;
 stop.value = 0;

 write(fd, (const void*) &play, sizeof(stop));*/
//raydium_log("remove effectvaleur=%d", ioctl(fd, EVIOCRMFF, effect.id));
}

void raydium_joy_ff_tremble_set(GLfloat period, GLfloat force)
{
#ifndef APPLE
#ifndef WIN32
struct input_event play;
struct input_event stop;

if (raydium_timecall_clock() < last_event + raydium_timecall_clocks_per_sec/10)
return;


if(effect_tremble_state)
{
 stop.type = EV_FF;
 stop.code = effect_tremble.id;
 stop.value = 0;
 write(raydium_joy_event_handle, (const void*) &stop, sizeof(stop));
// perror("ff: stop tremble");
 ioctl(raydium_joy_event_handle, EVIOCRMFF, effect_tremble.id);
// perror("ff: free tremble");
}

memset(&effect_tremble,0,sizeof(struct ff_effect));
effect_tremble.type = FF_PERIODIC;
effect_tremble.u.periodic.waveform=FF_TRIANGLE;
effect_tremble.u.periodic.period=period;
effect_tremble.u.periodic.magnitude=force;
//effect_tremble.u.periodic.direction=0x4000; // NEED SOME WORK HERE !!
effect_tremble.replay.length=65535;
ioctl(raydium_joy_event_handle, EVIOCSFF, &effect_tremble);
//perror("ff: upload tremble");
play.type = EV_FF;
play.code = effect_tremble.id;
play.value = 1;
write(raydium_joy_event_handle, (const void*) &play, sizeof(play));
//perror("ff: play tremble");
effect_tremble_state=1;
last_event=raydium_timecall_clock();
//printf("ff event refreshed\n");
#endif
#endif
}

