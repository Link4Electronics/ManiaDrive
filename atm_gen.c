#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// "Improved Alpha-Tested Magnification" generator.
// http://www.valvesoftware.com/publications/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf

// Compilation can be done with:
// - gcc atm_gen.c -lm -o atm_gen
// - ... or regular tools (odyncomp, Raydium SDK, ...)

typedef struct TargaHeader {
	unsigned char IDLength;
	unsigned char ColormapType;
	unsigned char ImageType;
	unsigned char ColormapSpecification[5];
	unsigned short XOrigin;
	unsigned short YOrigin;
	unsigned short ImageWidth;
	unsigned short ImageHeight;
	unsigned char PixelDepth;
	unsigned char ImageDescriptor;
} TargaHeader;

typedef struct Image {
	int sx;
	int sy;
	unsigned char *data;
} Image;


#define min(a,b) ( (a) < (b) ? (a) : (b) )
#define max(a,b) ( (a) > (b) ? (a) : (b) )

Image load_tga(char *filename)
{
FILE *fp;
TargaHeader head;
unsigned char *data;
Image ret;

fp=fopen(filename,"rb");
if(!fp)
	{
	fprintf(stderr,"ERROR: Can't open '%s'\n",filename);
	exit(10);
	}

if(fread(&head,sizeof(TargaHeader),1,fp)!=1)
	{
	fprintf(stderr,"ERROR: Unable to read header from input file (%s)\n",filename);
	fclose(fp);
	exit(11);
	}

if(head.ImageType!=3 || head.PixelDepth!=8)
	{
	fprintf(stderr,"ERROR: Need a Greyscale 8 bits uncompress TGA file (%s)\n",filename);
	fclose(fp);
	exit(12);
	}

data=malloc(head.ImageHeight*head.ImageWidth);
if(!data)
	{
	fprintf(stderr,"ERROR: Unable to allocate memory to read input file (%s)\n",filename);
	fclose(fp);
	exit(13);
	}

if(fread(data,head.ImageHeight*head.ImageWidth,1,fp)!=1)
	{
	fprintf(stderr,"ERROR: Unable to read data from input file (%s)\n",filename);
	fclose(fp);
	exit(14);
	}

ret.sx=head.ImageWidth;
ret.sy=head.ImageHeight;
ret.data=data;
fclose(fp);
return ret;
}

signed char save_tga(char *filename, Image *img)
{
FILE *fp;
TargaHeader head;

fp=fopen(filename,"wb");
if(!fp)
	{
	fprintf(stderr,"ERROR: Can't open '%s'\n",filename);
	exit(20);
	}

head.IDLength=0;
head.ColormapType=0;
head.ImageType=3; // Uncompress greyscale
//head.ColormapSpecification[] * 5
head.XOrigin=0;
head.YOrigin=0;
head.ImageWidth=img->sx;
head.ImageHeight=img->sy;
head.PixelDepth=8;
head.ImageDescriptor=0;

if(fwrite(&head,sizeof(TargaHeader),1,fp)!=1)
	{
	fprintf(stderr,"ERROR: Unable to write header to output file (%s)\n",filename);
	fclose(fp);
	exit(21);
	}

if(fwrite(img->data,img->sx*img->sy,1,fp)!=1)
	{
	fprintf(stderr,"ERROR: Unable to write data to output file (%s)\n",filename);
	fclose(fp);
	exit(22);
	}

printf("%s saved.\n",filename);
fclose(fp);
return 1;
}

unsigned char img_get(Image *img, int x, int y)
{
return img->data[y*img->sx+x];
}

void img_set(Image *img, int x, int y, unsigned char value)
{
img->data[y*img->sx+x]=value;
}

signed char atm_calc(Image *in, Image *out, int out_sx, int out_sy, int distMax)
{
int x,y;

out->sx=out_sx;
out->sy=out_sy;
out->data=malloc(out_sx*out_sy);

if(!out->data)
	{
	fprintf(stderr,"ERROR: Unable to allocate memory to calc ATM\n");
	return 0;
	}

printf("Generating %ix%i distance field with maximum distance of %i ...\n",out_sx,out_sy,distMax);

for(x=0;x<out->sx;x++)
	{
	fprintf(stderr,"\r%.1f %%",(x*100)/(float)out->sx);
	for(y=0;y<out->sy;y++)
		{
		int sourceX,sourceY;
		int seekX,seekY;
		signed char sourceValue; // bool
		float squaredDistMin;
		float squaredDist;
		unsigned char destValue;

		sourceX = x * in->sx / out->sx;
		sourceY = y * in->sy / out->sy;

		sourceValue = img_get(in,sourceX,sourceY) > 127;
		squaredDistMin = distMax*distMax;
		for(seekX = max(sourceX - distMax, 0);seekX < min(sourceX + distMax, in->sx);seekX++)
			{
			for(seekY = max(sourceY - distMax, 0);seekY < min(sourceY + distMax, in->sy);seekY++)
				{
				signed char seekValue; // bool
				seekValue = img_get(in,seekX,seekY) > 127;
				if(seekValue != sourceValue)
					{
					squaredDist = (seekX - sourceX) * (seekX - sourceX) + (seekY - sourceY) * (seekY - sourceY);
					if(squaredDist < squaredDistMin)
						squaredDistMin = squaredDist;
					}
				}
			}

		if(sourceValue)
			destValue = (255 * (0.5f + sqrtf(squaredDistMin) / (2.0f * (distMax))));
		else
			destValue = (255 * (0.5f - sqrtf(squaredDistMin) / (2.0f * (distMax))));

		img_set(out,x,y,destValue);
		}
	}

fprintf(stderr,"\rFinished.\n");
return 1;
}

int main(int argc, char **argv)
{
Image inputimage;
Image outputimage;
int ox,oy;
int max=200;

if(argc!=5 && argc!=6)
	{
	printf("\"Improved Alpha-Tested Magnification\" distance field generator\n");
	printf("Usage: %s input.tga output.tga outputX outputY [maxDistance]\n",argv[0]);
	printf("Default maxDistance is %i.\n",max);
	printf("Reads and writes 8 bits uncompressed TGA files.\n");
	printf("Raydium supports 24 bits RGBA ATM files, so you may use output image as an alpha \"layer mask\" and paint on the RGB channels.\n");
	exit(1);
	}

ox=atoi(argv[3]);
oy=atoi(argv[4]);
if(argc==6)
	max=atoi(argv[5]);

inputimage=load_tga(argv[1]);
atm_calc(&inputimage,&outputimage,ox,oy,max);
save_tga(argv[2],&outputimage);
// should free inputfile->data & outputfile->data

return 0;
}
