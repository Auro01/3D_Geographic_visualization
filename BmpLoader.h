#ifndef BMPLOADER_H
#define BMPLOADER_H

#ifdef _WIN32
#include <windows.h>
#else
#include "bitmap.h"
#endif

class BmpLoader
{
    public:
        unsigned char* textureData;
        int iWidth ,iHeight;
        BmpLoader(const char*);
        ~BmpLoader();
    private:
        BITMAPFILEHEADER bmpFileHead;
        BITMAPINFOHEADER bmpInfoHead;
};

#endif // BMPLOADER_H
