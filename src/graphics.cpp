#include <stdlib.h>
#include <malloc.h>
#include <png.h>

#include <gsKit.h>
#include <dmaKit.h>


extern "C" {
#include <jpeglib.h>
#include <jerror.h>
}

#include "graphics.h"

#define INPUT_BUF_SIZE  4096	/* choose an efficiently fread'able size */


/* gsKit Tex coords 
 */
static float x_center;
static float y_center;
static float w_ratio;
static float h_ratio;
static int   texture_size;


extern u8 msx[];

static int initialized = 0;

static int getNextPower2(int width)
{
	int b = width;
	int n;
	for (n = 0; b != 0; n++) b >>= 1;
	b = 1 << n;
	if (b == 2 * width) b >>= 1;
	return b;
}


void user_warning_fn(png_structp png_ptr, png_const_charp warning_msg)
{
}

static bool isJpegFile(const char* filename)
{
	char* suffix = strrchr(filename, '.');
	if (suffix) {
		if (stricmp(suffix, ".jpg") == 0 || stricmp(suffix, ".jpeg") == 0) return true;
	}
	return false;
}

Image* loadImage(const char* filename)
{

        unsigned char *data;
        Image * image;
        int size;
        FILE *fp;
        
        fp = fopen(filename, "r");
        if (fp == NULL) return NULL;
       
        // compute file size
        fseek(fp, 0, SEEK_END);
        size = ftell(fp) + 1;
        fseek(fp, 0, SEEK_SET);
       
        // load file content into our temporary buffer
        data = (unsigned char *)memalign(64, size);
        fread(data, size, 1, fp);
        fclose(fp);
        
        image = loadImageFromMemory(data, size);
        
        free (data);
        data = NULL;
        
      
        return image;

        /*
        
	if (isJpegFile(filename)) {
		return loadJpegImage(filename);
	} else {
		return loadPngImage(filename);
	}
	
	*/
	
}

Image* loadPngImageImpl(png_structp png_ptr)
{
	unsigned int sig_read = 0;
	png_uint_32 width, height, x, y;
	int bit_depth, color_type, interlace_type;
	u32* line;
	png_infop info_ptr;
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return NULL;
	}
	png_set_sig_bytes(png_ptr, sig_read);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, int_p_NULL, int_p_NULL);
	if (width > 512 || height > 512) {
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return NULL;
	}
	Image* image = (Image*) malloc(sizeof(Image));
	image->imageWidth = width;
	image->imageHeight = height;
	image->textureWidth = getNextPower2(width);
	image->textureHeight = getNextPower2(height);
	png_set_strip_16(png_ptr);
	png_set_packing(png_ptr);
	if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_gray_1_2_4_to_8(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);
	png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
	image->data = (Color*) memalign(128, image->textureWidth * image->textureHeight * sizeof(Color));
	if (!image->data) {
		free(image);
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return NULL;
	}
	line = (u32*) memalign(128,width * 4);
	if (!line) {
		free(image->data);
		free(image);
		png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
		return NULL;
	}
	

	for (y = 0; y < height; y++) {
		png_read_row(png_ptr, (u8*) line, png_bytep_NULL);
		// some little optimization :)
		for (x = width; x--;) {
			image->data[x + y * image->textureWidth] =  (u32)line[x];
		}
	}
	free(line);

	png_read_end(png_ptr, info_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
	return image;
}

Image* loadPngImage(const char* filename)
{
	png_structp png_ptr;
	FILE *fp;

	if ((fp = fopen(filename, "rb")) == NULL) return NULL;
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fclose(fp);
		return NULL;;
	}
	png_init_io(png_ptr, fp);
	Image* image = loadPngImageImpl(png_ptr);
	fclose(fp);
	return image;
}

Image* loadJpegImageImpl(struct jpeg_decompress_struct dinfo)
{
	jpeg_read_header(&dinfo, TRUE);
	int width = dinfo.image_width;
	int height = dinfo.image_height;
	jpeg_start_decompress(&dinfo);
	Image* image = (Image*) malloc(sizeof(Image));
	if (!image) {
		jpeg_destroy_decompress(&dinfo);
		return NULL;
	}
	if (width > 512 || height > 512) {
		jpeg_destroy_decompress(&dinfo);
		return NULL;
	}
	image->imageWidth = width;
	image->imageHeight = height;
	image->textureWidth = getNextPower2(width);
	image->textureHeight = getNextPower2(height);
	image->data = (Color*) memalign(128, image->textureWidth * image->textureHeight * sizeof(Color));
	u8* line = (u8*) malloc(width * 3);
	if (!line) {
		jpeg_destroy_decompress(&dinfo);
		return NULL;
	}
	if (dinfo.jpeg_color_space == JCS_GRAYSCALE) {
		while (dinfo.output_scanline < dinfo.output_height) {
			int y = dinfo.output_scanline;
			jpeg_read_scanlines(&dinfo, &line, 1);
			for (int x = 0; x < width; x++) {
				Color c = line[x];
				image->data[x + image->textureWidth * y] = c | (c << 8) | (c << 16) | 0xff000000;;
			}
		}
	} else {
		while (dinfo.output_scanline < dinfo.output_height) {
			int y = dinfo.output_scanline;
			jpeg_read_scanlines(&dinfo, &line, 1);
			u8* linePointer = line;
			for (int x = 0; x < width; x++) {
				Color c = *(linePointer++);
				c |= (*(linePointer++)) << 8;
				c |= (*(linePointer++)) << 16;
				image->data[x + image->textureWidth * y] = c | 0xff000000;
			}
		}
	}
	jpeg_finish_decompress(&dinfo);
	jpeg_destroy_decompress(&dinfo);
	free(line);
	return image;
}

Image* loadJpegImage(const char* filename)
{
	struct jpeg_decompress_struct dinfo;
	struct jpeg_error_mgr jerr;
	dinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&dinfo);
	FILE* inFile = fopen(filename, "rb");
	if (!inFile) {
		jpeg_destroy_decompress(&dinfo);
		return NULL;
	}
	jpeg_stdio_src(&dinfo, inFile);
	Image* image = loadJpegImageImpl(dinfo);
	fclose(inFile);
	return image;
}


// code for jpeg memory source
typedef struct {
	struct jpeg_source_mgr pub;	/* public fields */

	const unsigned char* membuff;	/* The input buffer */
	int location;			/* Current location in buffer */ 
	int membufflength;            /* The length of the input buffer */
	JOCTET * buffer;		/* start of buffer */
	boolean start_of_buff;	/* have we gotten any data yet? */
} mem_source_mgr;

typedef mem_source_mgr* mem_src_ptr;




METHODDEF(void) mem_init_source (j_decompress_ptr cinfo) {
	mem_src_ptr src;

	src = (mem_src_ptr) cinfo->src;

	/* We reset the empty-input-file flag for each image,
	* but we don't clear the input buffer.
	* This is correct behavior for reading a series of images from one source.
	*/
	src->location = 0;
	src->start_of_buff = TRUE;
}


METHODDEF(boolean) mem_fill_input_buffer (j_decompress_ptr cinfo) {
	mem_src_ptr src;
	size_t bytes_to_read;
	size_t nbytes;

	src = (mem_src_ptr) cinfo->src;

	if((src->location)+INPUT_BUF_SIZE >= src->membufflength)
		bytes_to_read = src->membufflength - src->location;
	else
		bytes_to_read = INPUT_BUF_SIZE;

	memcpy(src->buffer, (src->membuff)+(src->location), bytes_to_read);
	nbytes = bytes_to_read;
	src->location += (int) bytes_to_read;

	if (nbytes <= 0) {
		if (src->start_of_buff)	/* Treat empty input file as fatal error */
			ERREXIT(cinfo, JERR_INPUT_EMPTY);
		WARNMS(cinfo, JWRN_JPEG_EOF);
		/* Insert a fake EOI marker */
		src->buffer[0] = (JOCTET) 0xFF;
		src->buffer[1] = (JOCTET) JPEG_EOI;
		nbytes = 2;
	}

	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = nbytes;
	src->start_of_buff = FALSE;

	return TRUE;
}


METHODDEF(void) mem_skip_input_data (j_decompress_ptr cinfo, long num_bytes) {
	mem_src_ptr src;

	src = (mem_src_ptr) cinfo->src;

	if (num_bytes > 0) {
		while (num_bytes > (long) src->pub.bytes_in_buffer) {
			num_bytes -= (long) src->pub.bytes_in_buffer;
			mem_fill_input_buffer(cinfo);
		}
		src->pub.next_input_byte += (size_t) num_bytes;
		src->pub.bytes_in_buffer -= (size_t) num_bytes;
	}
}


METHODDEF(void) mem_term_source (j_decompress_ptr cinfo) {
}


GLOBAL(void) jpeg_mem_src (j_decompress_ptr cinfo, const unsigned char *mbuff, int mbufflen) {
	mem_src_ptr src;

	if (cinfo->src == NULL) {	/* first time for this JPEG object? */
		cinfo->src = (struct jpeg_source_mgr *)
			(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
			sizeof(mem_source_mgr));
		src = (mem_src_ptr) cinfo->src;
		src->buffer = (JOCTET *)
			(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
			INPUT_BUF_SIZE * sizeof(JOCTET));
	}

	src = (mem_src_ptr) cinfo->src;
	src->pub.init_source = mem_init_source;
	src->pub.fill_input_buffer = mem_fill_input_buffer;
	src->pub.skip_input_data = mem_skip_input_data;
	src->pub.resync_to_restart = jpeg_resync_to_restart;
	src->pub.term_source = mem_term_source;
	src->membuff = mbuff;
	src->membufflength = mbufflen;
	src->pub.bytes_in_buffer = 0;    /* forces fill_input_buffer on first read */
	src->pub.next_input_byte = NULL; /* until buffer loaded */
}

typedef struct {
	const unsigned char *data;
	png_size_t size;
	png_size_t seek;
} PngData;
	
static void ReadPngData(png_structp png_ptr, png_bytep data, png_size_t length)
{
	PngData *pngData = (PngData*) png_get_io_ptr(png_ptr);
	if (pngData) {
		for (png_size_t i = 0; i < length; i++) {
			if (pngData->seek >= pngData->size) break;
			data[i] = pngData->data[pngData->seek++];
		}
	}
}

Image* loadImageFromMemory(const unsigned char* data, int len)
{
	if (len < 8) return NULL;
	
	// test for PNG
	if (data[0] == 137 && data[1] == 80 && data[2] == 78 && data[3] == 71
		&& data[4] == 13 && data[5] == 10 && data[6] == 26 && data[7] ==10)
	{
		png_structp png_ptr;
		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (png_ptr == NULL) {
			return NULL;;
		}
		PngData pngData;
		
	        pngData.data = data;
		  
		pngData.size = len;
		pngData.seek = 0;
		png_set_read_fn(png_ptr, (void *) &pngData, ReadPngData);
		Image* image = loadPngImageImpl(png_ptr);
		return image;
	} else {
		// assume JPG
		struct jpeg_decompress_struct dinfo;
		struct jpeg_error_mgr jerr;
		dinfo.err = jpeg_std_error(&jerr);
		jpeg_create_decompress(&dinfo);
		jpeg_mem_src(&dinfo, data, len);
		Image* image = loadJpegImageImpl(dinfo);
		return image;
	}
}

void blitImageToImage(int sx, int sy, int width, int height, Image* source, int dx, int dy, Image* destination)
{
	Color* destinationData = &destination->data[destination->textureWidth * dy + dx];
	int destinationSkipX = destination->textureWidth - width;
	Color* sourceData = &source->data[source->textureWidth * sy + sx];
	int sourceSkipX = source->textureWidth - width;
	int x, y;
	for (y = 0; y < height; y++, destinationData += destinationSkipX, sourceData += sourceSkipX) {
		for (x = 0; x < width; x++, destinationData++, sourceData++) {
			*destinationData = *sourceData;
		}
	}
}



void blitAlphaImageToImage(int sx, int sy, int width, int height, Image* source, int dx, int dy, Image* destination)
{
	Color* destinationData = &destination->data[destination->textureWidth * dy + dx];
	int destinationSkipX = destination->textureWidth - width;
	Color* sourceData = &source->data[source->textureWidth * sy + sx];
	int sourceSkipX = source->textureWidth - width;
	int x, y;
	s32 rcolorc, gcolorc, bcolorc, acolorc,rcolord, gcolord, bcolord, acolord;
	for (y = 0; y < height; y++, destinationData += destinationSkipX, sourceData += sourceSkipX) {
		for (x = 0; x < width; x++, destinationData++, sourceData++) {
			Color color = *sourceData;
			if (!IS_ALPHA(color)) {
				*destinationData = color;
			} else {
				rcolorc = color & 0xff;
				gcolorc = (color >> 8) & 0xff;
				bcolorc = (color >> 16) & 0xff;
				acolorc = (color >> 24) & 0xff;
				rcolord = *destinationData & 0xff;
				gcolord = (*destinationData >> 8) & 0xff;
				bcolord = (*destinationData >> 16) & 0xff;
				acolord = (*destinationData >> 24) & 0xff;
				
				rcolorc = ((acolorc*rcolorc)>>8) + (((255-acolorc) * rcolord)>>8);
				if (rcolorc > 255) rcolorc = 255;
				gcolorc = ((acolorc*gcolorc)>>8) + (((255-acolorc) * gcolord)>>8);
				if (gcolorc > 255) gcolorc = 255;
				bcolorc = ((acolorc*bcolorc)>>8) + (((255-acolorc) * bcolord)>>8);
				if (bcolorc > 255) bcolorc = 255;
				if (acolord + acolorc < 255) {
					acolorc = acolord+acolorc;
				} else {
					acolorc = 255;
				}
				*destinationData = rcolorc | (gcolorc << 8) | (bcolorc << 16) | (acolorc << 24);
			}
		}
	}
}



Image* createImage(int width, int height)
{
	Image* image = (Image*) malloc(sizeof(Image));
	if (image==NULL) return NULL;
	image->imageWidth = width;
	image->imageHeight = height;
	image->textureWidth = getNextPower2(width);
	image->textureHeight = getNextPower2(height);
	image->data = (Color*) memalign(128, image->textureWidth * image->textureHeight * sizeof(Color));
	if (!image->data) return NULL;
	memset(image->data, 0, image->textureWidth * image->textureHeight * sizeof(Color));
	return image;
}

void freeImage(Image* image)
{
	free(image->data);
	free(image);
}

void clearImage(Color color, Image* image)
{
	int i;
	int size = image->textureWidth * image->textureHeight;
	Color* data = image->data;
	for (i = 0; i < size; i++, data++) *data = color;
}



void fillImageRect(Color color, int x0, int y0, int width, int height, Image* image)
{
	int skipX = image->textureWidth - width;
	int x, y;
	Color* data = image->data + x0 + y0 * image->textureWidth;
	for (y = 0; y < height; y++, data += skipX) {
		for (x = 0; x < width; x++, data++) *data = color;
	}
}

void fillScreenRect(Color color, int x0, int y0, int width, int height)
{
	if (!initialized) return;
	
	int skipX = PS2_LINE_SIZE - width;
	int x, y;
	Color* data = getVramDrawBuffer() + x0 + y0 * PS2_LINE_SIZE;
	for (y = 0; y < height; y++, data += skipX) {
		for (x = 0; x < width; x++, data++) *data = color;
	}

}

void putPixelScreen(Color color, int x, int y)
{

	Color* vram = getVramDrawBuffer();
	vram[PS2_LINE_SIZE * y + x] = color;

}

void putPixelImage(Color color, int x, int y, Image* image)
{
	image->data[x + y * image->textureWidth] = color;
}

Color getPixelScreen(int x, int y)
{

	Color* vram = getVramDrawBuffer();
	return vram[PS2_LINE_SIZE * y + x];

}

Color getPixelImage(int x, int y, Image* image)
{
	return image->data[x + y * image->textureWidth];
}

void printTextScreen(int x, int y, const char* text, u32 color)
{
	int i, j, l;
	u8 *font;
	Color *vram_ptr;
	Color *vram;
	
	if (!initialized) return;

	for (size_t c = 0; c < strlen(text); c++) {
		if (x < 0 || x + 8 > SCREEN_WIDTH || y < 0 || y + 8 > SCREEN_HEIGHT) break;
		char ch = text[c];
		
		vram = getVramDrawBuffer() + x + y * PS2_LINE_SIZE;
		
		font = &msx[ (int)ch * 8];
		for (i = l = 0; i < 8; i++, l += 8, font++) {
			vram_ptr  = vram;
			for (j = 0; j < 8; j++) {
				if ((*font & (128 >> j))) *vram_ptr = color;
				vram_ptr++;
			}
			vram += PS2_LINE_SIZE;
		}
		x += 8;
	}
}

void printTextImage(int x, int y, const char* text, u32 color, Image* image)
{
	int i, j, l;
	u8 *font;
	Color *data_ptr;
	Color *data;
	
	if (!initialized) return;

	for (size_t c = 0; c < strlen(text); c++) {
		if (x < 0 || x + 8 > image->imageWidth || y < 0 || y + 8 > image->imageHeight) break;
		char ch = text[c];
		data = image->data + x + y * image->textureWidth;
		
		font = &msx[ (int)ch * 8];
		for (i = l = 0; i < 8; i++, l += 8, font++) {
			data_ptr  = data;
			for (j = 0; j < 8; j++) {
				if ((*font & (128 >> j))) *data_ptr = color;
				data_ptr++;
			}
			data += image->textureWidth;
		}
		x += 8;
	}
}

static void fontPrintTextImpl(FT_Bitmap* bitmap, int xofs, int yofs, Color color, Color* framebuffer, int width, int height, int lineSize)
{
	u8 rf = color & 0xff; 
	u8 gf = (color >> 8) & 0xff;
	u8 bf = (color >> 16) & 0xff;
	u8 af = (color >> 24) & 0xff;
	
	u8* line = bitmap->buffer;
	Color* fbLine = framebuffer + xofs + yofs * lineSize;
	for (int y = 0; y < bitmap->rows; y++) {
		u8* column = line;
		Color* fbColumn = fbLine;
		for (int x = 0; x < bitmap->width; x++) {
			if (x + xofs < width && x + xofs >= 0 && y + yofs < height && y + yofs >= 0) {
				u8 val = *column;
				color = *fbColumn;
				u8 r = color & 0xff; 
				u8 g = (color >> 8) & 0xff;
				u8 b = (color >> 16) & 0xff;
				u8 a = (color >> 24) & 0xff;
				r = rf * val / 255 + (255 - val) * r / 255;
				g = gf * val / 255 + (255 - val) * g / 255;
				b = bf * val / 255 + (255 - val) * b / 255;
				a = af * val / 255 + (255 - val) * a / 255;
				*fbColumn = r | (g << 8) | (b << 16) | (a << 24);
			}
			column++;
			fbColumn++;
		}
		line += bitmap->pitch;
		fbLine += lineSize;
	}
}

void fontPrintTextImage(FT_Bitmap* bitmap, int x, int y, Color color, Image* image)
{
	fontPrintTextImpl(bitmap, x, y, color, image->data, image->imageWidth, image->imageHeight, image->textureWidth);
}

void fontPrintTextScreen(FT_Bitmap* bitmap, int x, int y, Color color)
{
	fontPrintTextImpl(bitmap, x, y, color, getVramDrawBuffer(), SCREEN_WIDTH, SCREEN_HEIGHT, PS2_LINE_SIZE);
}

void saveImage(const char* filename, Color* data, int width, int height, int lineSize, int saveAlpha)
{
	if (isJpegFile(filename)) {
		saveJpegImage(filename, data, width, height, lineSize);
	} else {
		savePngImage(filename, data, width, height, lineSize, saveAlpha);
	}
}

void savePngImage(const char* filename, Color* data, int width, int height, int lineSize, int saveAlpha)
{
	png_structp png_ptr;
	png_infop info_ptr;
	FILE* fp;
	int i, x, y;
	u8* line;
	
	if ((fp = fopen(filename, "wb")) == NULL) return;
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) return;
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		return;
	}
	png_init_io(png_ptr, fp);
	png_set_IHDR(png_ptr, info_ptr, width, height, 8,
		saveAlpha ? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB,
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	png_write_info(png_ptr, info_ptr);
	line = (u8*) malloc(width * (saveAlpha ? 4 : 3));
	for (y = 0; y < height; y++) {
		for (i = 0, x = 0; x < width; x++) {
			Color color = data[x + y * lineSize];
			u8 r = color & 0xff; 
			u8 g = (color >> 8) & 0xff;
			u8 b = (color >> 16) & 0xff;
			u8 a = saveAlpha ? (color >> 24) & 0xff : 0xff;
			line[i++] = r;
			line[i++] = g;
			line[i++] = b;
			if (saveAlpha) line[i++] = a;
		}
		png_write_row(png_ptr, line);
	}
	free(line);
	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	fclose(fp);
}

void saveJpegImage(const char* filename, Color* data, int width, int height, int lineSize)
{
	FILE* outFile = fopen(filename, "wb");
	if (!outFile) return;
	struct jpeg_error_mgr jerr;
	struct jpeg_compress_struct cinfo;
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, outFile);
	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, 100, TRUE);
	jpeg_start_compress(&cinfo, TRUE);
	u8* row = (u8*) malloc(width * 3);
	if (!row) return;
	for (int y = 0; y < height; y++) {
		u8* rowPointer = row;
		for (int x = 0; x < width; x++) {
			Color c = data[x + cinfo.next_scanline * lineSize];
			*(rowPointer++) = c & 0xff;
			*(rowPointer++) = (c >> 8) & 0xff;
			*(rowPointer++) = (c >> 16) & 0xff;
		}
		jpeg_write_scanlines(&cinfo, &row, 1);
	}
	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
	fclose(outFile);
	free(row);
}

static void drawLine(int x0, int y0, int x1, int y1, int color, Color* destination, int width)
{
	int dy = y1 - y0;
	int dx = x1 - x0;
	int stepx, stepy;
	
	if (dy < 0) { dy = -dy;  stepy = -width; } else { stepy = width; }
	if (dx < 0) { dx = -dx;  stepx = -1; } else { stepx = 1; }
	dy <<= 1;
	dx <<= 1;
	
	y0 *= width;
	y1 *= width;
	destination[x0+y0] = color;
	if (dx > dy) {
		int fraction = dy - (dx >> 1);
		while (x0 != x1) {
			if (fraction >= 0) {
				y0 += stepy;
				fraction -= dx;
			}
			x0 += stepx;
			fraction += dy;
			destination[x0+y0] = color;
		}
	} else {
		int fraction = dx - (dy >> 1);
		while (y0 != y1) {
			if (fraction >= 0) {
				x0 += stepx;
				fraction -= dy;
			}
			y0 += stepy;
			fraction += dx;
			destination[x0+y0] = color;
		}
	}
}

void drawLineScreen(int x0, int y0, int x1, int y1, Color color)
{
	drawLine(x0, y0, x1, y1, color, getVramDrawBuffer(), PS2_LINE_SIZE);
}

void drawLineImage(int x0, int y0, int x1, int y1, Color color, Image* image)
{
	drawLine(x0, y0, x1, y1, color, image->data, image->textureWidth);
}



/**
 * -------------------------------------------------------------------------------
 * PS2 dependent functions 
 * using gsKit
 * -------------------------------------------------------------------------------
 **/

/* definitions, variables
 */

#define RATIO 	      1.33

static GSGLOBAL *gsGlobal = NULL;
static GSTEXTURE gsTexture;

static const u64 BLACK_RGBAQ   = GS_SETREG_RGBAQ(0x00,0x00,0x00,0x00,0x00);
static const u64 TEXTURE_RGBAQ = GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x00);


/* functions
 */

Color* getVramDrawBuffer()
{
	return gsTexture.Mem;
}

void gsKit_clear_screens()
{
	int i;

	for (i=0; i<2; i++)
	{
		gsKit_clear(gsGlobal, BLACK_RGBAQ);
		gsKit_queue_exec(gsGlobal);
		gsKit_sync_flip(gsGlobal);
	}
}

void disableGraphics()
{
	initialized = 0;
}

void clearScreen(Color color)
{
	if (!initialized) return;
	
	Color* data = getVramDrawBuffer();
	for (int i = (texture_size / 4); i-- ; ) *data++ = color;
	
	
}


void blitImageToScreen(int sx, int sy, int width, int height, Image* source, int dx, int dy)
{
	if (!initialized) return;
	
	int x, y;
	Color* dest = getVramDrawBuffer();
	// This should be rewritten using hw function (gsKit)	
	for (y = height;y--;) {
		for (x = width; x--;) {
		        ((Color*)dest)[x + dx + (y + dy) * PS2_LINE_SIZE] = ((Color*)source->data)[x + sx + (y + sy) * source->textureWidth];
		}
	}

	
	
}

void blitAlphaImageToScreen(int sx, int sy, int width, int height, Image* source, int dx, int dy)
{
	if (!initialized) return;

	int x, y;
	Color* dest = getVramDrawBuffer();
	// This should be rewritten using hw function (gsKit)	
	for (y = height ;y--;) {
	        for (x = width ; x--;) {
	                Color color = ((Color*)source->data)[x + sx + (y + sy) * source->textureWidth];
			if (color & 0xFF000000) dest[x + dx + (y + dy) * PS2_LINE_SIZE] = color;
		}
	}
}


void initGraphics()
{

	if (initialized) return;
	
	gsGlobal = gsKit_init_global(GS_MODE_AUTO);

	/* initialize the DMAC */
	dmaKit_init(D_CTRL_RELE_OFF, D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC, D_CTRL_STD_OFF, D_CTRL_RCYC_8);
	dmaKit_chan_init(DMA_CHANNEL_GIF);
	dmaKit_chan_init(DMA_CHANNEL_FROMSPR);
	dmaKit_chan_init(DMA_CHANNEL_TOSPR);
	
	/* disable zbuffer because we're only doing 2d now */
	gsGlobal->ZBuffering = GS_SETTING_OFF;

	gsGlobal->DoubleBuffering = GS_SETTING_ON;
	gsGlobal->PrimAAEnable = 0;
	gsGlobal->PrimAlphaEnable = 0;

	gsGlobal->PSM = GS_PSM_CT32;
	gsGlobal->Test->ZTE = 0;
	
	// force non interlaced mode
	/*
	gsGlobal->Interlace = GS_NONINTERLACED;
	gsGlobal->Field     = GS_FRAME;
	gsGlobal->StartY    = gsGlobal->StartY/2+1;
	*/

		
	printf("\nLuaGraphics: created video surface of (%d, %d)\n",
		gsGlobal->Width, gsGlobal->Height);

	gsKit_init_screen(gsGlobal);
	
	gsKit_mode_switch(gsGlobal, GS_ONESHOT);
	
	gsTexture.Width = PS2_LINE_SIZE;
	gsTexture.Height = SCREEN_HEIGHT;
	gsTexture.PSM = GS_PSM_CT32;
	
	gsTexture.Filter = GS_FILTER_LINEAR;//GS_FILTER_NEAREST;
	
	texture_size = gsKit_texture_size(gsTexture.Width , gsTexture.Height, GS_PSM_CT32);
	
	gsTexture.Mem = (u32 *)memalign(128, texture_size);
	gsTexture.Vram = gsKit_vram_alloc(gsGlobal, texture_size, GSKIT_ALLOC_USERBUFFER);
	
	memset((void *)gsTexture.Mem, '\0', texture_size);
	
	/* clear both frames */
	gsKit_clear_screens();
	
	
	w_ratio = gsTexture.Width * RATIO;
	h_ratio = gsTexture.Height * RATIO;
	
	x_center =  (gsGlobal->Width  - w_ratio) / 2;
	y_center =  (gsGlobal->Height - h_ratio) / 2;
	
	// "Powered by Lua!"	
	#ifndef DEBUG
	displaySplashScreen(gsGlobal);
	#endif
       
        gsKit_clear(gsGlobal, BLACK_RGBAQ);	
	
	
	initialized = 1;

}



void flipScreen()
{
	if (!initialized) return;

	//gsKit_clear(gsGlobal, BLACK_RGBAQ);
	
	/* upload new frame buffer */
	gsKit_texture_upload(gsGlobal, &gsTexture);

	gsKit_prim_sprite_striped_texture(gsGlobal, &gsTexture, 
					  x_center, y_center, // x1,y1
					  0.0f, 0.0f, // u1,v1
					  w_ratio + x_center, 
					  h_ratio + y_center, //x2,y2
					  gsTexture.Width,
					  gsTexture.Height, //u2,v2
					  1, TEXTURE_RGBAQ);	

	
 	// execute render queue
	gsKit_queue_exec(gsGlobal);
	
	/*
	// vsync and flip buffer
 	gsKit_sync_flip(gsGlobal);
 	
	*/

	if(!gsGlobal->FirstFrame)
	{
		//gsKit_vsync();

		if(gsGlobal->DoubleBuffering == GS_SETTING_ON)
		{
			GS_SET_DISPFB2( gsGlobal->ScreenBuffer[gsGlobal->ActiveBuffer & 1] / 8192,
				gsGlobal->Width / 64, gsGlobal->PSM, 0, 0 );
	 
			gsGlobal->ActiveBuffer ^= 1;
			gsGlobal->PrimContext ^= 1;
		}

	}

	if(gsGlobal->DoSubOffset == GS_SETTING_ON)
	{

		gsGlobal->EvenOrOdd=((GSREG*)GS_CSR)->FIELD;
	}

	gsKit_setactive(gsGlobal);

		
}

void graphicWaitVblankStart()
{
	   
	   //-- we are already waiting for vblank start in the flip function
	   //-- so no need to repeat it here
	   if(!gsGlobal->FirstFrame)
	     gsKit_vsync();
}
