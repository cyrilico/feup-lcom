#include "bitmap.h"
#include "stdio.h"
#include "video_gr.h"



Bitmap* loadBitmap(const char* filename, int x, int y) {
    // allocating necessary size
    Bitmap* bmp = (Bitmap*) malloc(sizeof(Bitmap));

    // open filename in read binary mode
    FILE *filePtr;
    filePtr = fopen(filename, "rb");
    if (filePtr == NULL)
        return NULL;

    // read the bitmap file header
    BitmapFileHeader bitmapFileHeader;
    fread(&bitmapFileHeader, 2, 1, filePtr);

    bmp->x = 0;
    bmp->y = 0;

    // verify that this is a bmp file by check bitmap id
    if (bitmapFileHeader.type != 0x4D42) {
        fclose(filePtr);
        return NULL;
    }

    int rd;
    do {
        if ((rd = fread(&bitmapFileHeader.size, 4, 1, filePtr)) != 1)
            break;
        if ((rd = fread(&bitmapFileHeader.reserved, 4, 1, filePtr)) != 1)
            break;
        if ((rd = fread(&bitmapFileHeader.offset, 4, 1, filePtr)) != 1)
            break;
    } while (0);

    if (rd = !1) {
        fprintf(stderr, "Error reading file\n");
        exit(-1);
    }

    // read the bitmap info header
    BitmapInfoHeader bitmapInfoHeader;
    fread(&bitmapInfoHeader, sizeof(BitmapInfoHeader), 1, filePtr);

    // move file pointer to the begining of bitmap data
    fseek(filePtr, bitmapFileHeader.offset, SEEK_SET);

    // allocate enough memory for the bitmap image data
    unsigned char* bitmapImage = (unsigned char*) malloc(
            bitmapInfoHeader.imageSize);

    // verify memory allocation
    if (!bitmapImage) {
        free(bitmapImage);
        fclose(filePtr);
        return NULL;
    }

    // read in the bitmap image data
    fread(bitmapImage, bitmapInfoHeader.imageSize, 1, filePtr);

    // make sure bitmap image data was read
    if (bitmapImage == NULL) {
        fclose(filePtr);
        return NULL;
    }

    // close file and return bitmap image data
    fclose(filePtr);

    bmp->bitmapData = bitmapImage;
    bmp->bitmapInfoHeader = bitmapInfoHeader;
    bmp->x = x;
    bmp->y = y;

    return bmp;
}

void drawBitmap(Bitmap* bmp, char* buffer, Alignment alignment) {
    if (bmp == NULL)
        return;

    int width = bmp->bitmapInfoHeader.width;
    int drawWidth = width;
    int height = bmp->bitmapInfoHeader.height;

    if (alignment == ALIGN_CENTER)
        bmp->x -= width / 2;
    else if (alignment == ALIGN_RIGHT)
        bmp->x -= width;

    if (bmp->x + width < 0 || bmp->x > vg_get_h_res() || bmp->y + height < 0
            || bmp->y > vg_get_v_res())
        return;

    int xCorrection = 0;
    if (bmp->x < 0) {
        xCorrection = -bmp->x;
        drawWidth -= xCorrection;
        bmp->x = 0;

        if (drawWidth > vg_get_h_res())
            drawWidth = vg_get_h_res();
    } else if (bmp->x + drawWidth >= vg_get_h_res()) {
        drawWidth = vg_get_h_res() - bmp->x;
    }

    char* bufferStartPos;
    char* imgStartPos;

/*
    int i, j;
     for (i = 0; i < height; i++) {
        int pos = bmp->y + height - 1 - i;
        if (pos < 0 || pos >= vg_get_v_res())
            continue;
        for (j = 0; j < width; j++) {
            bufferStartPos = buffer;
            bufferStartPos += bmp->x * 2 + pos * vg_get_h_res() * 2 + 2*j;
            imgStartPos = bmp->bitmapData + xCorrection * 2 + i * width * 2 + 2*j;
            if ( (*imgStartPos | (*(imgStartPos+1) << 8)) != PINK)
                memcpy(bufferStartPos, imgStartPos, 2);
        }
    }
*/

    int i;
    for (i = 0; i < height; i++) {

    	int pos = bmp->y + height - 1 - i;

        if (pos < 0 || pos >= vg_get_v_res())
            continue;

        bufferStartPos = buffer;
        bufferStartPos += bmp->x * 2 + pos * vg_get_h_res() * 2;

        imgStartPos = bmp->bitmapData + xCorrection * 2 + i * width * 2;

        int j = 0;
        while(j < width){
            if((*imgStartPos << 8) | (*imgStartPos+1) != PINK) {
            	*bufferStartPos = *imgStartPos;
            	*(bufferStartPos+1) = *(imgStartPos+1);
            }
            bufferStartPos+= 2;
            imgStartPos+= 2;
            j++;
        }
    }
}

void deleteBitmap(Bitmap* bmp) {
    if (bmp == NULL)
        return;

    free(bmp->bitmapData);
    free(bmp);
}
