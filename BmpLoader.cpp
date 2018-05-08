#include "BmpLoader.h"
#include <stdio.h>
#include <iostream>

BmpLoader::BmpLoader(const char* filename)
{
    FILE *file = 0;
    file = fopen(filename,"rb");
    if(!file){
        std::cout<<"File not found"<<std::endl;
    }
    fread(&bmpFileHead,sizeof(BITMAPFILEHEADER),1,file);
    if(bmpFileHead.bfType != 0x4d42){
        std::cout<<"Not a valid bitmap"<<std::endl;
    }
    fread(&bmpInfoHead,sizeof(BITMAPINFOHEADER),1,file);
    if(bmpInfoHead.biSizeImage==0){
        bmpInfoHead.biSizeImage=bmpInfoHead.biHeight*bmpInfoHead.biWidth*3;
    }
    textureData = new unsigned char[bmpInfoHead.biSizeImage];
    fseek(file,bmpFileHead.bfOffBits,SEEK_SET);
    fread(textureData,1,bmpInfoHead.biSizeImage,file);

    unsigned char temp;
    for(int i = 0; i<bmpInfoHead.biSizeImage;i+=3){
        temp = textureData[i];
        textureData[i] = textureData[i+2];
        textureData[i+2]=temp;
    }
    iWidth = bmpInfoHead.biWidth;
    iHeight = bmpInfoHead.biHeight;
    fclose(file);
}

BmpLoader::~BmpLoader(){
    delete [] textureData;
}
