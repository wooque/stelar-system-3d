#include "glaux.h"

#include <iostream>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <stdexcept>

using std::string;
using std::cout;
using std::endl;

AUX_RGBImageRec *auxDIBImageLoad(string imagepath)
{
    // Data read from the header of the BMP file
    unsigned char header[54]; // Each BMP file begins by a 54-bytes header
    unsigned int dataPos;     // Position in the file where the actual data begins
    unsigned int width, height;
    unsigned int imageSize;   // = width*height*3
    // Actual RGB data
    unsigned char * data;

    // Open the file
    FILE * file = fopen(imagepath.c_str(), "rb");
    if (!file) 
    {
        cout << "Image could not be opened" << endl;;
        return 0;
    }
    
    if (fread(header, 1, 54, file) != 54)
    { 
        // If not 54 bytes read : problem
        cout << "Not a correct BMP file" << endl;
        return 0;
    }
    
    if (header[0]!='B' || header[1]!='M')
    {
        cout << "Not a correct BMP file" << endl;
        return 0;
    }
    
    // Read ints from the byte array
    dataPos    = *(int*)&(header[0x0A]);
    imageSize  = *(int*)&(header[0x22]);
    width      = *(int*)&(header[0x12]);
    height     = *(int*)&(header[0x16]);
    
    // Some BMP files are misformatted, guess missing information
    if (imageSize == 0)
    {
        imageSize = width*height*3; // 3 : one byte for each Red, Green and Blue component
    }
    if (dataPos == 0)
    {
        dataPos = 54; // The BMP header is done that way
    }
    
    // Create a buffer
    data = (unsigned char *)malloc(imageSize);

    // Read the actual data from the file into the buffer
    fread(data, 1, imageSize, file);

    //Everything is in memory now, the file can be closed
    fclose(file);
    
    AUX_RGBImageRec *ret = (AUX_RGBImageRec *)malloc(sizeof(AUX_RGBImageRec));
    ret->sizeX = width;
    ret->sizeY = height;
    ret->data = data;
    return ret;
}

AUX_RGBImageRec *auxDIBImageLoad2(string FilePath)
{
    int width = 0;
    int height = 0;
    short BitsPerPixel = 0;
    std::vector<unsigned char> Pixels;
    
    std::fstream hFile(FilePath, std::ios::in | std::ios::binary);
    if (!hFile.is_open()) throw std::invalid_argument("Error: File Not Found.");

    hFile.seekg(0, std::ios::end);
    int Length = hFile.tellg();
    hFile.seekg(0, std::ios::beg);
    std::vector<std::uint8_t> FileInfo(Length);
    hFile.read(reinterpret_cast<char*>(FileInfo.data()), 54);

    if(FileInfo[0] != 'B' && FileInfo[1] != 'M')
    {
        hFile.close();
        throw std::invalid_argument("Error: Invalid File Format. Bitmap Required.");
    }

    if (FileInfo[28] != 24 && FileInfo[28] != 32)
    {
        hFile.close();
        throw std::invalid_argument("Error: Invalid File Format. 24 or 32 bit Image Required.");
    }

    BitsPerPixel = FileInfo[28];
    width = FileInfo[18] + (FileInfo[19] << 8);
    height = FileInfo[22] + (FileInfo[23] << 8);
    std::uint32_t PixelsOffset = FileInfo[10] + (FileInfo[11] << 8);
    std::uint32_t size = ((width * BitsPerPixel + 31) / 32) * 4 * height;
    Pixels.resize(size);

    hFile.seekg (PixelsOffset, std::ios::beg);
    hFile.read(reinterpret_cast<char*>(Pixels.data()), size);
    hFile.close();
    
    AUX_RGBImageRec *ret = new AUX_RGBImageRec();
    ret->sizeX = width;
    ret->sizeY = height;
    ret->data = new unsigned char[width*height*3];
    for(int i = 0; i < width*height*3; i++)
    {
        ret->data[i] = Pixels[i];
    }
    return ret;
}
