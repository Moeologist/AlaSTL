#include <stdio.h>

#pragma pack(push)
#pragma pack(2)

typedef struct {
    unsigned short bfType;
    unsigned bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned bfOffBits;
} BITMAPFILEHEADER;

typedef struct {
    unsigned biSize;
    int biWidth;
    int biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned biCompression;
    unsigned biSizeImage;
    int biXPelsPerMeter;
    int biYPelsPerMeter;
    unsigned biClrUsed;
    unsigned biClrImportant;
} BITMAPINFOHEADER;

#pragma pack(pop)

int write_bmp(const void *buffer, const char *filename, int width, int height,
              int withalpha) {
    if (width < 1 || height < 1 || !buffer || !filename)
        return -1;
    unsigned channels = 3;
    if (withalpha)
        channels = 4;
    BITMAPFILEHEADER bf = {0x4d42, 14 + 40 + width * height * channels, 0, 0,
                           14 + 40};
    BITMAPINFOHEADER bi = {40,
                           width,
                           height,
                           1,
                           (unsigned short)(channels * 8),
                           0,
                           width * height * channels,
                           72,
                           72,
                           0,
                           0};
    FILE *f = fopen(filename, "wb");
    if (!f)
        return 0;
    fwrite(&bf, 14, 1, f);
    fwrite(&bi, 40, 1, f);
    for (int i = height - 1; i >= 0; i--) {
        fwrite((unsigned char *)buffer + i * width * channels, 1,
               width * channels, f);
        int fill = width * channels % 4;
        if (fill) {
            unsigned char fillzero[4] = {0};
            fwrite(fillzero, 1, 4 - fill, f);
        }
    }
    fclose(f);
    return 1;
}

int read_bmp(const void *buffer, const char *filename, int *width, int *height,
             int *withalpha) {
    if (!width || !height || !buffer || !filename)
        return 0;
    FILE *f = fopen(filename, "rb");
    if (!f)
        return 0;
    BITMAPFILEHEADER bf;
    BITMAPINFOHEADER bi;
    fread(&bf, 14, 1, f);
    fread(&bi, bf.bfOffBits - 14, 1, f);
    unsigned channels = bi.biBitCount / 8;
    if (withalpha) {
        if (channels == 4)
            *withalpha = 1;
        else
            *withalpha = 0;
    }
    *width = bi.biWidth, *height = bi.biHeight;
    for (int i = *height - 1; i >= 0; i--) {
        fread((unsigned char *)buffer + i * *width * channels, 1,
              *width * channels, f);
        int fill = *width * channels % 4;
        if (fill) {
            unsigned char fillzero[4] = {0};
            fread(fillzero, 1, 4 - fill, f);
        }
    }
    fclose(f);
    return 1;
}

#define write_bmp(buffer, width, height, filename) \
    write_bmp(buffer, width, height, filename, 0)
#define read_bmp(buffer, width, height, filename) \
    read_bmp(buffer, width, height, filename, NULL)