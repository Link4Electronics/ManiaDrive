/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef DONT_INCLUDE_HEADERS
#include "index.h"
#else
#include "headers/live.h"
#endif

#include "live.h"

// From univision. Untested yet.
// http://sourceforge.net/projects/univision/
void uyvy2rgb24( unsigned char *dest, unsigned char *source, size_t pixels )
{
	size_t i;
	int dest_offset = 0;


	for( i = 0; i < (pixels*2); i+=4 )
	{
		unsigned char *r, *b, *g;
		unsigned char *y1, *y2, *u, *v;

		double fr, fg, fb;
		double fy1, fy2, fu, fv;

		r = dest + dest_offset;
		g = r + 1;
		b = g + 1;

		u = source + i;
		y1 = u + 1;
		v = y1 + 1;
		y2 = v + 1;

		fu = *u;
		fv = *v;
		fy1= *y1;
		fy2= *y2;

		//printf("u=%.1f v=%.1f",fu,fv);

/* 		fr = fy1 + ( 1.4075f * ( fv - 128.0f ) ); */
/* 		fg = fy1 - ( 0.3455f * ( fu - 128.0f ) - ( 0.7169f * ( fv - 128.0f ) ) ); */
/* 		fb = fy1 + ( 1.7790f * ( fu - 128.0f ) ); */
		fr = fy1 - 0.0009267 * ( fu - 128 ) + 1.4016868 * ( fv - 128 );
		fg = fy1 - 0.3436954 * ( fu - 128 ) - 0.7141690 * ( fv - 128 );
		fb = fy1 + 1.7721604 * ( fu - 128 ) + 0.0009902 * ( fv - 128 );

		*r = (unsigned char) ( fr > 255 ? 255 : ( fr < 0 ? 0 : fr ) );
		*g = (unsigned char) ( fg > 255 ? 255 : ( fg < 0 ? 0 : fg ) );
		*b = (unsigned char) ( fb > 255 ? 255 : ( fb < 0 ? 0 : fb ) );


		dest_offset += 3;

		r = dest + dest_offset;
		g = r + 1;
		b = g + 1;

/* 		fr = fy2 + ( 1.4075f * ( fv - 128.0f ) ); */
/* 		fg = fy2 - ( 0.3455f * ( fu - 128.0f ) - ( 0.7169f * ( fv - 128.0f ) ) ); */
/* 		fb = fy2 + ( 1.7790f * ( fu - 128.0f ) ); */
		fr = fy2 - 0.0009267 * ( fu - 128 ) + 1.4016868 * ( fv - 128 );
		fg = fy2 - 0.3436954 * ( fu - 128 ) - 0.7141690 * ( fv - 128 );
		fb = fy2 + 1.7721604 * ( fu - 128 ) + 0.0009902 * ( fv - 128 );

		*r = (unsigned char) ( fr > 255 ? 255 : ( fr < 0 ? 0 : fr ) );
		*g = (unsigned char) ( fg > 255 ? 255 : ( fg < 0 ? 0 : fg ) );
		*b = (unsigned char) ( fb > 255 ? 255 : ( fb < 0 ? 0 : fb ) );

		dest_offset += 3;
	}

}


// YUV420P to RGB code (next 2 functions) from:
// Peopletracking with an omnicamera
// Daniel Hammarin & Mihajlo Miladinovic

// !! Since this code is not native, no "raydium_" prefix is used !!

/* LIMIT: convert a 16.16 fixed-point value to a byte, with clipping. */
#define LIMIT(x) ((x)>0xffffff?0xff: ((x)<=0xffff?0:((x)>>16)))
void
v4l_copy_420_block (int yTL, int yTR, int yBL, int yBR,
                    int u, int v, int rowPixels, unsigned char *rgb, int bits)
{
  const int rvScale = 91881;
  const int guScale = -22553;
  const int gvScale = -46801;
  const int buScale = 116129;
  const int yScale = 65536;
  int r, g, b;
  g = guScale * u + gvScale * v;
  b = rvScale * v;
  r = buScale * u;
  yTL *= yScale;
  yTR *= yScale;
  yBL *= yScale;
  yBR *= yScale;
  if (bits == 24)
    {
/* Write out top two pixels */
      rgb[0] = LIMIT (b + yTL);
      rgb[1] = LIMIT (g + yTL);
      rgb[2] = LIMIT (r + yTL);
      rgb[3] = LIMIT (b + yTR);
      rgb[4] = LIMIT (g + yTR);
      rgb[5] = LIMIT (r + yTR);
/* Skip down to next line to write out bottom two pixels */
      rgb += 3 * rowPixels;
      rgb[0] = LIMIT (b + yBL);
      rgb[1] = LIMIT (g + yBL);
      rgb[2] = LIMIT (r + yBL);
      rgb[3] = LIMIT (b + yBR);
      rgb[4] = LIMIT (g + yBR);
      rgb[5] = LIMIT (r + yBR);
    }
  else if (bits == 16)
    {
/* Write out top two pixels */
      rgb[0] =
        ((LIMIT (b + yTL) >> 3) & 0x1F) | ((LIMIT (g + yTL) << 3) & 0xE0);
      rgb[1] = ((LIMIT (g + yTL) >> 5) & 0x07) | (LIMIT (r + yTL) & 0xF8);
      rgb[2] =
        ((LIMIT (b + yTR) >> 3) & 0x1F) | ((LIMIT (g + yTR) << 3) & 0xE0);
      rgb[3] = ((LIMIT (g + yTR) >> 5) & 0x07) | (LIMIT (r + yTR) & 0xF8);
/* Skip down to next line to write out bottom two pixels */
      rgb += 2 * rowPixels;
      rgb[0] =
        ((LIMIT (b + yBL) >> 3) & 0x1F) | ((LIMIT (g + yBL) << 3) & 0xE0);
      rgb[1] = ((LIMIT (g + yBL) >> 5) & 0x07) | (LIMIT (r + yBL) & 0xF8);
      rgb[2] =
        ((LIMIT (b + yBR) >> 3) & 0x1F) | ((LIMIT (g + yBR) << 3) & 0xE0);
      rgb[3] = ((LIMIT (g + yBR) >> 5) & 0x07) | (LIMIT (r + yBR) & 0xF8);
    }
}

int
v4l_yuv420p2rgb (unsigned char *rgb_out, unsigned char *yuv_in, int width, int
                 height, int bits)
{
  const int numpix = width * height;
  const unsigned int bytes = bits >> 3;
  int h, w, y00, y01, y10, y11, u, v;
  unsigned char *pY = yuv_in;
  unsigned char *pU = pY + numpix;
  unsigned char *pV = pU + numpix / 4;
  unsigned char *pOut = rgb_out;
  if (!rgb_out || !yuv_in)
    return -1;
  for (h = 0; h <= height - 2; h += 2)
    {
      for (w = 0; w <= width - 2; w += 2)
        {
          y00 = *(pY);
          y01 = *(pY + 1);
          y10 = *(pY + width);
          y11 = *(pY + width + 1);
          u = (*pU++) - 128;
          v = (*pV++) - 128;
          v4l_copy_420_block (y00, y01, y10, y11, u, v, width, pOut, bits);
          pY += 2;
          pOut += bytes << 1;
        }
      pY += width;
      pOut += width * bytes;
    }
  return 0;
}


#ifndef WIN32
#ifndef APPLE
static int xioctl(int fh, int request, void *arg)
{
int r;

do {
    r = ioctl(fh, request, arg);
} while (-1 == r && EINTR == errno);

return r;
}
#endif
#endif


/////// video (devices) part


signed char raydium_live_video_isvalid(int i)
{
if(i>=0 && i<RAYDIUM_MAX_VIDEO_DEVICES &&
    raydium_live_device[i].capture_style!=RAYDIUM_LIVE_FREE)
        return 1;
return 0;
}


int raydium_live_video_find_free(void)
{
int i;

for(i=0;i<RAYDIUM_MAX_VIDEO_DEVICES;i++)
    if(raydium_live_device[i].capture_style==RAYDIUM_LIVE_FREE)
        return i;
return -1;
}

// Windows call back, called after each frame, work only for one source
// Don't have solution for multiple source
// Work with 24bpp images.
#ifdef WIN32
LRESULT CALLBACK Frame_CallBack(HWND hWnd, LPVIDEOHDR lpVHdr )
{
    int i,j;
    BYTE * pin , * pout;
    raydium_live_Device *dev;
//    BITMAPINFO * bmi_in;
    pin = lpVHdr->lpData;
    dev = (raydium_live_Device *) capGetUserData(hWnd);

//    bmi_in = &dev->capture_video_format_original;
    ICDecompressBegin( dev->compressor, /*(LPBITMAPINFOHEADER)*/ &dev->capture_video_format_original, /*(LPBITMAPINFOHEADER)*/ &dev->capture_video_format);
    ICDecompress( dev->compressor,ICDECOMPRESS_HURRYUP,(LPBITMAPINFOHEADER) &dev->capture_video_format_original, lpVHdr->lpData, (LPBITMAPINFOHEADER)  &dev->capture_video_format, dev->buffer);
    ICDecompressEnd( dev->compressor );

    pin = dev->buffer;
    pin +=dev->win.width*dev->win.height*(dev->vpic.depth/8);
    pout = dev->buffer2;
    for (i=0;i<dev->win.height;i++)
    {
        pin-=dev->win.width*3;
        for (j=0;j<dev->win.width;j++)
        {
            *pout++=*(pin+2);
            *pout++=*(pin+1);
            *pout++=*(pin+0);
            pin+=3;
        }
        pin-=dev->win.width*3;
    }
    return 0;
}
#endif

int raydium_live_video_open(char *device, int sizex, int sizey)
{

#ifndef APPLE
char *default_device=RAYDIUM_LIVE_DEVICE_DEFAULT;
int id;
int i;
raydium_live_Device *dev;
char cli_device[RAYDIUM_MAX_NAME_LEN];

#ifndef WIN32
struct v4l2_requestbuffers req;
struct v4l2_buffer buf;
enum v4l2_buf_type type;
#endif

if(!device)
    {
    raydium_init_cli_option_default("video-device",cli_device,default_device);
    device=cli_device;
    }

id=raydium_live_video_find_free();
if(id<0)
    {
    raydium_log("live: ERROR: no more free device slot available (max: %i)",RAYDIUM_MAX_VIDEO_DEVICES);
    return -1;
    }

dev=&raydium_live_device[id];

#ifndef WIN32
dev->fd=open(device, O_RDWR | O_NONBLOCK, 0);;

if (dev->fd<0)
    {
    perror("open");
    raydium_log("live: ERROR: cannot open device '%s'",device);
    return -1;
    }
#else
    {
        HWND hWnd;
//        HDC hDC;
        char webcam_name_list[RAYDIUM_MAX_NAME_LEN];
        char webcam_drv_list[RAYDIUM_MAX_NAME_LEN];



        hWnd = GetDesktopWindow();
//        hDC = GetDC(hWnd);

        if (device)
            id = atoi(device);
        else
            id = 0;

        //Capture Frame create
        dev->hWnd_WC = capCreateCaptureWindow("WebCam", WS_CHILD, 0, 0, 1, 1, hWnd, 0);

        for(i=0;i<3;i++)
            { //Need to try several times, first one often fail ...
            if(capDriverConnect(dev->hWnd_WC, 0))
                break;
            else
                {
                raydium_log("live: WARNING: creating capture context attempt %d fail, keep trying...",i);
                Sleep(100);
                }
            }
        if(i>=3)
            {
            raydium_log("live: ERROR: Cannot Create capture context.");
            return -1;
            }

        capGetDriverDescription (id, webcam_name_list, sizeof(webcam_name_list), webcam_drv_list,sizeof(webcam_drv_list));
        raydium_log ("live: Found WebCam: %d %s",id,webcam_name_list);
        strcpy(dev->name,webcam_name_list);

    // Perhaps needed config dialog.
    //  capDlgVideoDisplay( dev->hWnd_WC );
    //  capDlgVideoFormat ( dev->hWnd_WC );
    //  capDlgVideoSource ( dev->hWnd_WC );
        dev->hDC_WC = GetDC(dev->hWnd_WC);
    }

#endif

#ifndef WIN32
if (xioctl(dev->fd, VIDIOC_QUERYCAP, &dev->cap) < 0)
    {
    perror("VIDIOC_QUERYCAP");
    raydium_log("live: ERROR: not a v4l2 device '%s'",device);
    close(dev->fd);
    return -1;
    }
if (!(dev->cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
    {
    raydium_log("live: ERROR: not a capture device '%s', can't use with LiveAPI",device);
    close(dev->fd);
    return -1;
    }

#else // Get driver capability
if (!capDriverGetCaps (dev->hWnd_WC,&dev->capdriver_caps,sizeof(CAPDRIVERCAPS))){
    raydium_log("live: ERROR: not a capDriverGetCaps Failed device '%s'",device);
    capDriverDisconnect(dev->hWnd_WC);
    DestroyWindow(dev->hWnd_WC);
    return -1;
}
// Get capture params
if (!capCaptureGetSetup(dev->hWnd_WC,&dev->capture_param,sizeof(CAPTUREPARMS))){
    raydium_log("live: ERROR: not a CaptureSetup Failed device '%s'",device);
    capDriverDisconnect(dev->hWnd_WC);
    DestroyWindow(dev->hWnd_WC);
    return -1;
}
 // Get capture status
if (!capGetStatus(dev->hWnd_WC,&dev->capture_status,sizeof(CAPSTATUS))){
    raydium_log("live: ERROR: not a capGetStatus Failed device '%s'",device);
    capDriverDisconnect(dev->hWnd_WC);
    DestroyWindow(dev->hWnd_WC);
    return -1;
    }
// Get image info
capGetVideoFormat(dev->hWnd_WC,&dev->capture_video_format_original,sizeof(BITMAPINFO));
/*
if (!capGetVideoFormat(dev->hWnd_WC,&dev->capture_video_format,sizeof(BITMAPINFO))){
    raydium_log("live: ERROR: not a capGetVideoFormat Failed device '%s'",device);
    capDriverDisconnect(dev->hWnd_WC);
    DestroyWindow(dev->hWnd_WC);
    return -1;
    }
*/
#endif

#ifndef WIN32
raydium_log("live: device '%s' (%s)",dev->cap.card,device);
#else
raydium_log("live: device '%s' (%s)",dev->name,device);
raydium_log("live: Drv default image size %ix%i, Image %ix%i",
dev->capture_status.uiImageWidth,dev->capture_status.uiImageHeight,
dev->capture_video_format_original.bmiHeader.biWidth,dev->capture_video_format_original.bmiHeader.biHeight);
#endif


if(sizex<0 || sizey<0)
    {
    char s[RAYDIUM_MAX_NAME_LEN];
    char sx[RAYDIUM_MAX_NAME_LEN];
    char sy[RAYDIUM_MAX_NAME_LEN];

#ifndef WIN32
    dev->win.width=RAYDIUM_LIVE_SIZEX_DEFAULT;
    dev->win.height=RAYDIUM_LIVE_SIZEY_DEFAULT;
#else
    dev->win.width=dev->capture_status.uiImageWidth;
    dev->win.height=dev->capture_status.uiImageHeight;
#endif

    if(raydium_init_cli_option("video-size",s))
        {
        if(!raydium_parser_cut(s,sx,sy,'x'))
            {
            // apply to every video device ... :/
            raydium_log("live: ERROR: --video-size format invalid (ex: 352x288)");
            }
        else
            {
            dev->win.width=atoi(sx);
            dev->win.height=atoi(sy);
            }
        }
    }
else
    {
    dev->win.width=sizex;
    dev->win.height=sizey;
    }


#ifndef WIN32

// Reset cropping/scaling for this device (it that useful ?)
memset(&dev->cropcap,0,sizeof(struct v4l2_cropcap));
dev->cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

if (0 == xioctl(dev->fd, VIDIOC_CROPCAP, &dev->cropcap))
    {
    dev->crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    dev->crop.c = dev->cropcap.defrect; /* reset to default */

    if (-1 == xioctl(dev->fd, VIDIOC_S_CROP, &dev->crop))
        raydium_log("live: WARNING: cropping not supported");
    }
else
    raydium_log("live: WARNING: no cropping abilities");


memset(&dev->format_wanted,0,sizeof(struct v4l2_format));
memset(&dev->format_hardware,0,sizeof(struct v4l2_format));

// Request format (what we want)
dev->format_wanted.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
dev->format_wanted.fmt.pix.width       = dev->win.width;
dev->format_wanted.fmt.pix.height      = dev->win.height;
dev->format_wanted.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
dev->format_wanted.fmt.pix.field       = V4L2_FIELD_INTERLACED;

dev->v4lconvert_data = v4lconvert_create(dev->fd);
if (dev->v4lconvert_data == NULL)
    {
    perror("v4lconvert_create");
    raydium_log("live: ERROR: can't create v4lconvert data");
    close(dev->fd);
    return -1;
    }

raydium_log("live: %ix%i %c%c%c%c (requesting)",
     dev->format_wanted.fmt.pix.width,
     dev->format_wanted.fmt.pix.height,
     dev->format_wanted.fmt.pix.pixelformat & 0xff,
    (dev->format_wanted.fmt.pix.pixelformat >> 8) & 0xff,
    (dev->format_wanted.fmt.pix.pixelformat >> 16) & 0xff,
    (dev->format_wanted.fmt.pix.pixelformat >> 24) & 0xff);

if (v4lconvert_try_format(dev->v4lconvert_data, &dev->format_wanted, &dev->format_hardware) != 0)
    {
    perror("v4lconvert_try_format");
    raydium_log("live: ERROR: can't find any compatible format (should be a rare case)");
    close(dev->fd);
    return -1;
    }

raydium_log("live: %ix%i %c%c%c%c (found)",
     dev->format_hardware.fmt.pix.width,
     dev->format_hardware.fmt.pix.height,
     dev->format_hardware.fmt.pix.pixelformat & 0xff,
    (dev->format_hardware.fmt.pix.pixelformat >> 8) & 0xff,
    (dev->format_hardware.fmt.pix.pixelformat >> 16) & 0xff,
    (dev->format_hardware.fmt.pix.pixelformat >> 24) & 0xff);

if (-1 == xioctl(dev->fd, VIDIOC_S_FMT, &dev->format_hardware))
    {
    perror("VIDIOC_S_FMT");
    raydium_log("live: ERROR: video format refused by device (?!)");
    close(dev->fd);
    return -1;
    }

dev->win.width = dev->format_wanted.fmt.pix.width;
dev->win.height = dev->format_wanted.fmt.pix.height;

#else

// Try to set good frame format.
// First direct try with hardware drive
    dev->capture_video_format.bmiHeader.biWidth = dev->win.width;
    dev->capture_video_format.bmiHeader.biHeight = dev->win.height;
    dev->capture_video_format.bmiHeader.biBitCount = 24; // Support only 24bpp format
    dev->capture_video_format.bmiHeader.biSizeImage = dev->capture_video_format.bmiHeader.biWidth * dev->capture_video_format.bmiHeader.biHeight * dev->capture_video_format.bmiHeader.biBitCount/8;
    dev->capture_video_format.bmiHeader.biPlanes = 1;
    dev->capture_video_format.bmiHeader.biSize = sizeof(BITMAPINFO);

    if (!capSetVideoFormat(dev->hWnd_WC,&dev->capture_video_format,sizeof(BITMAPINFO)))
        raydium_log("live: WARNING: capSetVideoFormat to RGB 24bpp Failed device '%s'",device);

//Try to find good decompressor, 3 attempts ...
    i=0;
    while(i++<3)
        {
        // Get original video format
        if(!capGetVideoFormat(dev->hWnd_WC,&dev->capture_video_format_original,sizeof(BITMAPINFO)))
            {
            raydium_log("live: fail to read actual format, live api not avaible");
            capDriverDisconnect(dev->hWnd_WC);
            DestroyWindow(dev->hWnd_WC);
            return -1;
            }

// Try to find correct decompressor
// Correct image format 24Bpp RGB
        dev->capture_video_format.bmiHeader.biWidth = dev->win.width;
        dev->capture_video_format.bmiHeader.biHeight = dev->win.height;
        dev->capture_video_format.bmiHeader.biBitCount = 24; // Support only 24bpp format
        dev->capture_video_format.bmiHeader.biSizeImage = dev->capture_video_format.bmiHeader.biWidth * dev->capture_video_format.bmiHeader.biHeight * dev->capture_video_format.bmiHeader.biBitCount/8;
        dev->capture_video_format.bmiHeader.biPlanes = 1;
        dev->capture_video_format.bmiHeader.biClrImportant =0;
        dev->capture_video_format.bmiHeader.biClrUsed=0;
        dev->capture_video_format.bmiHeader.biCompression = BI_RGB;

        dev->compressor=NULL;
//Is there a compressor for this ?
        dev->compressor = ICLocate(ICTYPE_VIDEO, (DWORD) NULL,(LPBITMAPINFOHEADER) &dev->capture_video_format_original,(LPBITMAPINFOHEADER) &dev->capture_video_format,ICMODE_DECOMPRESS );
        if (dev->compressor)
            {
            raydium_log("Live: Found correct decompressor, can process frames");
            break;
            }

        raydium_log("live: Opening Parameter dialog. Please set 24 bts RGB format and correct size, or other format ...");
        capDlgVideoFormat ( dev->hWnd_WC );
        }
    if (i>=3)
        {
        raydium_log("live: ERROR: Cannot find decompressor. WIN32 Live Api unavaible.");
        return -1;
        }
//Remaning configuration
    dev->capture_param.fYield =TRUE;
    dev->capture_param.fAbortLeftMouse=FALSE;
    dev->capture_param.fAbortRightMouse=FALSE;
    capCaptureSetSetup(dev->hWnd_WC,&dev->capture_param,sizeof(CAPTUREPARMS));
    dev->capture_style=RAYDIUM_LIVE_CAPTURE_READ;
#endif

#ifndef WIN32
    // thanks to libv4lconvert, we always get RGB24 :)
    dev->vpic.depth=24;
#else
    dev->vpic.depth=dev->capture_video_format.bmiHeader.biBitCount;
#endif

#ifndef WIN32
dev->buffer2  = malloc(dev->win.width * dev->win.height * dev->vpic.depth/8);
#else
dev->buffer  = malloc((dev->win.width * dev->win.height) * (dev->vpic.depth/8));
dev->buffer2  = malloc((dev->win.width * dev->win.height) * (dev->vpic.depth/8));
#endif

if (!dev->buffer2)
    {
    raydium_log("live: ERROR: buffer2: out of memory (!?)");
#ifndef WIN32
    close(dev->fd);
    return -1;
#else
    capDriverDisconnect(dev->hWnd_WC);
    DestroyWindow(dev->hWnd_WC);
    return -1;
#endif
    }

#ifndef WIN32


// RAYDIUM_LIVE_CAPTURE_READ is only supported by Raydium for
// old v4l1 API, currently :( We just try MMAP ...
// (V4L2_MEMORY_USERPTR is not supported either)
dev->capture_style=RAYDIUM_LIVE_CAPTURE_MMAP;

if (!(dev->cap.capabilities & V4L2_CAP_STREAMING))
    {
    raydium_log("live: ERROR: I/O streaming not supported, can't use MMAP for this device");
    close(dev->fd);
    return -1;
    }

// init MMAP

memset(&req,0,sizeof(struct v4l2_requestbuffers));

req.count = 4;
req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
req.memory = V4L2_MEMORY_MMAP;

// request buffers
if(xioctl(dev->fd, VIDIOC_REQBUFS, &req) < 0)
    {
    perror("VIDIOC_REQBUFS");
    raydium_log("live: ERROR: can't init MMAP for this device");
    close(dev->fd);
    return -1;
    }

if(req.count < 2)
    {
    raydium_log("live: ERROR: not enough buffers with this device (needed minimum is 2)");
    close(dev->fd);
    return -1;
    }

dev->buffers=calloc(req.count, sizeof(raydium_live_Buffer));
dev->n_buffers=req.count;

if(!dev->buffers)
    {
    raydium_log("live: ERROR: can't allocate memory for buffers !");
    close(dev->fd);
    return -1;
    }

// init buffers
for(i=0;i<dev->n_buffers;i++)
    {
    memset(&buf,0,sizeof(struct v4l2_buffer));

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = i;

    if(xioctl(dev->fd, VIDIOC_QUERYBUF, &buf) < 0)
        {
        raydium_log("live: ERROR: can't init buffer %i of %i",i+1,dev->n_buffers);
        close(dev->fd);
        return -1;
        }

    dev->buffers[i].length = buf.length;
    dev->buffers[i].start = mmap(NULL,buf.length,
                                 PROT_READ|PROT_WRITE,
                                 MAP_SHARED,dev->fd, buf.m.offset);

    if(dev->buffers[i].start==MAP_FAILED)
        {
        raydium_log("live: ERROR: mmap failded for buffer %i of %i",i+1,dev->n_buffers);
        close(dev->fd);
        return -1;
        }

    if(xioctl(dev->fd, VIDIOC_QBUF, &buf) < 0)
        {
        raydium_log("live: ERROR: can't queue buffer %i of %i",i+1,dev->n_buffers);
        close(dev->fd);
        return -1;
        }

    }

// start capture
type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
if(xioctl(dev->fd, VIDIOC_STREAMON, &type) < 0)
    {
    raydium_log("live: ERROR: can't start capture");
    close(dev->fd);
    return -1;
    }

strcpy(dev->name,device);
raydium_log("live: video init for this device is ok");
return id;
#else
    {


    capSetUserData(dev->hWnd_WC,dev);
    capSetCallbackOnVideoStream(dev->hWnd_WC,Frame_CallBack);
//      capSetCallbackOnFrame(dev->hWnd_WC, Frame_CallBack);
/*      capPreviewRate(dev->hWnd_WC,100);
        capPreview(dev->hWnd_WC,TRUE);*/
    capCaptureSequenceNoFile(dev->hWnd_WC);

    }
raydium_log("live: Win32 support");
return id;
#endif
#endif
}


int raydium_live_video_open_auto(void)
{
return raydium_live_video_open(RAYDIUM_LIVE_DEVICE_AUTO,RAYDIUM_LIVE_SIZE_AUTO,RAYDIUM_LIVE_SIZE_AUTO);
}


int raydium_live_video_read(raydium_live_Device *dev)
{
#ifndef APPLE
#ifndef WIN32

struct v4l2_buffer buf;
fd_set fds;
struct timeval tv;
int r;


if(dev->capture_style==RAYDIUM_LIVE_FREE)
    {
    raydium_log("live: ERROR: read failed: invalid device");
    return -1;
    }

// polling
FD_ZERO (&fds);
FD_SET (dev->fd, &fds);
tv.tv_sec=0;
tv.tv_usec=0;
r = select(dev->fd + 1, &fds, NULL, NULL, &tv);

// nothing to read, we'll try later (next frame)
if(r<=0)
    return 0;

// let's read frame
memset(&buf,0,sizeof(struct v4l2_buffer));
buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
buf.memory = V4L2_MEMORY_MMAP;

if(xioctl(dev->fd, VIDIOC_DQBUF, &buf) < 0)
    return -1; // can't dequeue buffer

if(buf.index>dev->n_buffers)
    return -1; // someone is crazy somewhere ...

if(v4lconvert_convert(dev->v4lconvert_data,
                      &dev->format_hardware,
                      &dev->format_wanted,
                      dev->buffers[buf.index].start,
                      buf.bytesused,
                      dev->buffer2,
                      dev->format_wanted.fmt.pix.sizeimage // should match buffer2 alloc, I think
                      )<0)
    return -1; // conv failed :(


if(xioctl(dev->fd, VIDIOC_QBUF, &buf) < 0)
    return -1; // can't requeue buffer

#endif
#endif
return 1;

}


void raydium_internal_live_video_callback(void)
{
int i;

for(i=0;i<RAYDIUM_MAX_VIDEO_DEVICES;i++)
 if(raydium_live_device[i].capture_style!=RAYDIUM_LIVE_FREE)
    if(raydium_live_video_read(&raydium_live_device[i]))
        raydium_live_texture_refresh(i);
}


/////////////// live API core

void raydium_internal_live_close(void)
{
#ifndef APPLE
int i;

for(i=0;i<RAYDIUM_MAX_VIDEO_DEVICES;i++)
    {
#ifndef WIN32
    if(raydium_live_device[i].capture_style!=RAYDIUM_LIVE_FREE)
        {
        for(i=0;i<raydium_live_device[i].n_buffers;i++)
            munmap(raydium_live_device[i].buffers[i].start, raydium_live_device[i].buffers[i].length);

        close(raydium_live_device[i].fd);
        free(raydium_live_device[i].buffer2);
        free(raydium_live_device[i].buffers);
        }
#else
    capDriverDisconnect(raydium_live_device[i].hWnd_WC);
    DestroyWindow(raydium_live_device[i].hWnd_WC);
    free(raydium_live_device[i].buffer);
    free(raydium_live_device[i].buffer2);
#endif
    }
#endif
}

void raydium_live_init(void)
{
int i;

for(i=0;i<RAYDIUM_MAX_VIDEO_DEVICES;i++)
    {
    raydium_live_device[i].capture_style=RAYDIUM_LIVE_FREE;
    raydium_live_device[i].buffer=NULL;
    raydium_live_device[i].frame=0;
    }

for(i=0;i<RAYDIUM_MAX_LIVE_TEXTURES;i++)
    {
    raydium_live_texture[i].state=0;
    raydium_live_texture[i].device=NULL;
    raydium_live_texture[i].OnRefresh=NULL;
    raydium_live_texture[i].data_source=NULL;
    }

raydium_log("video (live): OK");
}



signed char raydium_live_texture_isvalid(int i)
{
if(i>=0 && i<RAYDIUM_MAX_LIVE_TEXTURES && raydium_live_texture[i].state)
        return 1;
return 0;
}

int raydium_live_texture_find_free(void)
{
int i;

for(i=0;i<RAYDIUM_MAX_LIVE_TEXTURES;i++)
    if(!raydium_live_texture[i].state)
        return i;
return -1;
}

int raydium_live_texture_find(int original_texture)
{
int i;

for(i=0;i<RAYDIUM_MAX_LIVE_TEXTURES;i++)
    if(raydium_live_texture[i].state &&
       raydium_live_texture[i].texture==original_texture)
            return i;
return -1;
}


int raydium_live_texture_video(int device_id, char *as)
{
#ifndef APPLE
//#ifndef WIN32
int id;
raydium_live_Device *dev;
raydium_live_Texture *tex;

if(!raydium_live_video_isvalid(device_id))
    {
    //raydium_log("live: ERROR: invalid device id, cannot create live source");
    return -1;
    }

dev=&raydium_live_device[device_id];
id=raydium_live_texture_find_free();

if(id<0)
    {
    raydium_log("live: ERROR: no more free live texture slot available (max: %i)",RAYDIUM_MAX_LIVE_TEXTURES);
    return -1;
    }

tex=&raydium_live_texture[id];

tex->tx=dev->win.width;
tex->ty=dev->win.height;
tex->hardware_tx=raydium_math_pow2_next(dev->win.width);
tex->hardware_ty=raydium_math_pow2_next(dev->win.height);
tex->bpp=dev->vpic.depth;
tex->texture=raydium_texture_load_internal("not needed :)",as,1,0,0,0,id);


if(tex->texture<=0)
    {
    raydium_log("live: ERROR: cannot create 'faked' texture (see above)");
    return -1;
    }

tex->device=dev;
tex->data_source=dev->buffer2;
tex->state=1;
raydium_log("live: %s linked to %s (live)",dev->name,as);
return id;
/*#else
raydium_log("live: Live API is not supported under win32 yet");
return -1;
#endif*/
#else
    raydium_log("live: Live API is not yet supported under Mac OS X.");
    return -1;
#endif
}


void raydium_live_texture_refresh(int livetex)
{
raydium_live_Texture *tex;
int (*f)(unsigned char *data, int tx, int ty, int bpp);

if(!raydium_live_texture_isvalid(livetex))
    {
    raydium_log("live: cannot refresh live texture: wrong name or id");
    return;
    }

tex=&raydium_live_texture[livetex];

f=tex->OnRefresh;
if(f && !f(tex->data_source,tex->tx,tex->ty,tex->bpp))
    return;

//printf("%i - %ix%i %i bpp\n",tex->texture,tex->tx,tex->ty,tex->bpp);

glEnable(GL_TEXTURE_2D);
glBindTexture(GL_TEXTURE_2D,tex->texture);

// We'll use glTexImage2D() only if texture sizes are power of two
if(tex->tx != tex->hardware_tx || tex->ty != tex->hardware_ty)
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,
                    tex->tx,
                    tex->ty,
                    (tex->bpp==24?GL_RGB:GL_RGBA),
                    GL_UNSIGNED_BYTE,
                    tex->data_source);
else
    glTexImage2D(GL_TEXTURE_2D,0,(tex->bpp==24?GL_RGB:GL_RGBA),
                 tex->tx,tex->ty,
                 0,(tex->bpp==24?GL_RGB:GL_RGBA),
                 GL_UNSIGNED_BYTE,tex->data_source);
}



void raydium_live_texture_refresh_name(char *texture)
{
raydium_live_texture_refresh(raydium_live_texture_find(raydium_texture_find_by_name(texture)));
}


int raydium_live_texture_create(char *as, unsigned char *data_source, int tx, int ty, int bpp)
{
int id;
unsigned int i;
raydium_live_Texture *tex;

id=raydium_live_texture_find_free();

if(id<0)
    {
    raydium_log("live: ERROR: no more free live texture slot available (max: %i)",RAYDIUM_MAX_LIVE_TEXTURES);
    return -1;
    }

if(bpp!=24 && bpp!=32)
    {
    raydium_log("live: ERROR: live textures are limited to 24 or 32 bpp color depth only, for now");
    return -1;
    }


// duplicated ?
//for(i=0;i<raydium_texture_index;i++)
for(i=1;i<RAYDIUM_MAX_TEXTURES;i++)
if(raydium_texture_used[i])
{
    if(!strcmp(raydium_texture_name[i],as))
    {
    raydium_log("live: WARNING ! %s is duplicated",as);
    // this is the right answer only if duplicated texture
    // is already a "live" one ! (to fix ?)

    id=raydium_live_texture_find(i);
    // ... but reset/update some values
    tex=&raydium_live_texture[id];
    tex->device=NULL;
    tex->data_source=data_source;

    raydium_live_texture_refresh(id);
    return id;
    }
}

tex=&raydium_live_texture[id];
tex->tx=tx;
tex->ty=ty;
tex->hardware_tx=raydium_math_pow2_next(tex->tx);
tex->hardware_ty=raydium_math_pow2_next(tex->ty);
tex->bpp=bpp;
tex->texture=raydium_texture_load_internal("not needed :)",as,1,0,0,0,id);

if(tex->texture<=0)
    {
    raydium_log("live: ERROR: cannot create 'faked' texture (see above)");
    return -1;
    }

tex->device=NULL;
tex->data_source=data_source;
tex->state=1;
raydium_live_texture_refresh(id);
raydium_log("live: texture '%s' created",as);
return id;
}

void raydium_live_texture_mask(int livetex, GLfloat alpha)
{
raydium_live_Texture *tex;
GLfloat u,v;

if(!raydium_live_texture_isvalid(livetex))
    {
    //raydium_log("live: cannot draw live mask: wrong name or id");
    return;
    }

tex=&raydium_live_texture[livetex];

u=tex->tx/(float)tex->hardware_tx;
v=tex->ty/(float)tex->hardware_ty;

//raydium_osd_mask_texture_clip(tex->texture,alpha,u*100,v*100,0,0);
raydium_osd_mask_texture_clip(tex->texture,alpha,0,v*100,u*100,0);
}


void raydium_live_texture_mask_name(char *texture, GLfloat alpha)
{
raydium_live_texture_mask(raydium_live_texture_find(raydium_texture_find_by_name(texture)),alpha);
}


// (the current code of this function is quiet a hack ...)
void raydium_live_texture_draw(int livetex, GLfloat alpha,GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
raydium_live_Texture *tex;
GLfloat u,v;

if(!raydium_live_texture_isvalid(livetex))
    {
    raydium_log("live: cannot draw live mask: wrong name or id");
    return;
    }

tex=&raydium_live_texture[livetex];

u=tex->tx/(float)tex->hardware_tx;
v=tex->ty/(float)tex->hardware_ty;

raydium_osd_start();

raydium_texture_current_set(tex->texture);
raydium_rendering_internal_prepare_texture_render(tex->texture);
glColor4f(1,1,1,alpha);
glEnable(GL_BLEND);
glDepthMask(GL_FALSE);
glBegin(GL_QUADS);
glTexCoord2f(0,v);
glVertex3f(x1,y1,0);
glTexCoord2f(u,v);
glVertex3f(x2,y1,0);
glTexCoord2f(u,0);
glVertex3f(x2,y2,0);
glTexCoord2f(0,0);
glVertex3f(x1,y2,0);
glEnd();
raydium_rendering_internal_restore_render_state();

raydium_osd_stop();
}


void raydium_live_texture_draw_name(char *texture, GLfloat alpha,GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
raydium_live_texture_draw(raydium_live_texture_find(raydium_texture_find_by_name(texture)),alpha,x1,y1,x2,y2);
}

void raydium_live_texture_refresh_callback_set(int livetex, void *callback)
{
raydium_live_Texture *tex;

if(!raydium_live_texture_isvalid(livetex))
    {
    raydium_log("live: cannot set OnRefresh callback: wrong name or id");
    return;
    }

tex=&raydium_live_texture[livetex];
tex->OnRefresh=callback;
}

void raydium_live_texture_refresh_callback_set_name(char *texture, void *callback)
{
raydium_live_texture_refresh_callback_set(raydium_live_texture_find(raydium_texture_find_by_name(texture)),callback);
}


