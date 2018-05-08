#ifndef BMPLOADER_H
#define BMPLOADER_H

#include<windows.h>

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
