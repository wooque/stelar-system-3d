#pragma once

#include <GL/gl.h>
#include <GL/glu.h>
#include <string>

typedef struct _AUX_RGBImageRec {
    GLint sizeX, sizeY;
    unsigned char *data;
} AUX_RGBImageRec;

AUX_RGBImageRec *auxDIBImageLoad(std::string imagepath);
