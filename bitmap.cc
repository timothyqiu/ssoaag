#include "bitmap.h"
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <vector>

#pragma pack (push)
#pragma pack (2)

// Bitmap File Header (14 bytes)
// To store general information about the bitmap image file.
// Not needed after the file is loaded in memory.
struct BITMAPFILEHEADER {
    uint16_t bfType;    // BM/BA/CI/CP/IC/PT
    uint32_t bfSize;    // the size of the BMP file in bytes
    uint16_t bfReserved1;
	uint16_t bfReserved2;
    uint32_t bfOffBits; // the offset of the bitmap image data
};

// DIB Header (Fixed-size 7 versions exist, just use BITMAPINFOHEADER)
// To store detailed information about the bitmap image and
// define the pixel format.
// Immediately follows the Bitmap File Header
struct BITMAPINFOHEADER {
    uint32_t biSize;            // size of this header (40 bytes)
     int32_t biWidth;           // bitmap width in pixels
     int32_t biHeight;          // bitmap height in pixels
    uint16_t biPlanes;          // number of color planes being used (1)
    uint16_t biBitCount;        // number of bits per pixel (1/4/8/16/24/32)
    uint32_t biCompression;     // compression method
    uint32_t biSizeImage;       // image size of raw bitmap data
     int32_t biXPelsPerMeter;   // horizontal resolution. pixel per meter
     int32_t biYPelsPerMeter;   // vertical resolution. pixel per meter
    uint32_t biClrUsed;         // number of colors in the color palette, 0=2n
    uint32_t biClrImportant;    // number of important colors, 0=every
};

#pragma pack (pop)

// Compression (bytes #30-33)
#define BI_RGB      (0) // uncompressed
#define BI_RLE8     (1) // RLE 8-bit/pixel
#define BI_RLE4     (2) // RLE 4-bit/pixel


Bitmap::Bitmap()
    : width_(0), height_(0), pixels_(nullptr)
{
}

Bitmap::Bitmap(Bitmap const& other)
    : width_(0), height_(0), pixels_(nullptr)
{
    this->operator=(other);
}

Bitmap::Bitmap(Bitmap&& other)
    : width_(other.width_), height_(other.height_), pixels_(other.pixels_)
{
    other.width_  = 0;
    other.height_ = 0;
    other.pixels_ = nullptr;
}

Bitmap::~Bitmap()
{
    this->Free();
}

Bitmap& Bitmap::operator=(Bitmap const& rhs)
{
    if (this != &rhs) {
        this->Free();
        auto size = rhs.width_ * rhs.height_;
        this->width_  = rhs.width_;
        this->height_ = rhs.height_;
        this->pixels_ = new Color[size];
        memcpy(this->pixels_, rhs.pixels_, size);
    }
    return *this;
}

// TODO: find a way to tell user why we failed the loading process
bool Bitmap::Load(std::string const& filename)
{
    this->Free();

    std::ifstream file(filename, std::ios::binary | std::ios::in);

    if (file.fail())
        return false;

    file.seekg(0, std::ios::end);
    int length = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer;

    buffer.resize(length);
    file.read(buffer.data(), length);
    if (!file.good())
        return false;

    file.close();

    return this->LoadData(buffer.data(), length);
}

void Bitmap::Free()
{
    this->width_  = 0;
    this->height_ = 0;

    if (this->pixels_) {
        delete[] this->pixels_;
        this->pixels_ = nullptr;
    }
}

static bool HasBitmapSignature(char const *data, size_t size)
{
    if (size < 2)
        return false;

    // http://en.wikipedia.org/wiki/BMP_file_format#Bitmap_file_header
    return (memcmp(data, "BM", 2) == 0 ||   // Windows 3.1x, 95, NT, ... etc.
            memcmp(data, "BA", 2) == 0 ||   // OS/2 struct Bitmap Array
            memcmp(data, "CI", 2) == 0 ||   // OS/2 struct Color Icon
            memcmp(data, "CP", 2) == 0 ||   // OS/2 const Color Pointer
            memcmp(data, "IC", 2) == 0 ||   // OS/2 struct Icon
            memcmp(data, "PT", 2) == 0);    // OS/2 Pointer
}

static bool IsBitmap(char const *data, size_t size)
{
    // header field signatures
    if (!HasBitmapSignature(data, size))
        return false;

    // there must be a file header
    if (size < sizeof(BITMAPFILEHEADER))
        return false;

    auto *bf = reinterpret_cast<BITMAPFILEHEADER const *>(data);

    // total file size
    if (bf->bfSize != size)
        return false;

    auto *bi = reinterpret_cast<BITMAPINFOHEADER const *>(bf + 1);

    // number of planes must be one
    if (bi->biPlanes != 1)
        return false;

    // some compression method requires bits per pixel to be a specific value
    if (bi->biCompression == BI_RLE8 && bi->biBitCount != 8)
        return false;
    if (bi->biCompression == BI_RLE4 && bi->biBitCount != 4)
        return false;

    return true;
}

bool Bitmap::LoadData(char const *data, size_t size)
{
    if (!IsBitmap(data, size))
        return false;

    auto *bf = reinterpret_cast<BITMAPFILEHEADER const *>(data);
    auto *bi = reinterpret_cast<BITMAPINFOHEADER const *>(bf + 1);

    int w = this->width_  = abs(bi->biWidth );
    int h = this->height_ = abs(bi->biHeight);

    bool flip_x = (w != bi->biWidth );
    bool flip_y = (h != bi->biHeight);

    this->pixels_ = new Color[w * h];

    if (bi->biSize < sizeof(BITMAPINFOHEADER))
        return false;   // mainly means no BITMAPCOREHEADER support

    if (bi->biCompression != BI_RGB)
        return false;   // unsupported

    // for BI_RGB, color table is immediately after the info header
    auto *palette = reinterpret_cast<uint32_t const *>(data + sizeof(BITMAPFILEHEADER) + bi->biSize);

    /*
     * BI_RGB Pixel Format (R.G.B.A.X)
     *
     *        before V3  after V3
     * 16bpp  5.5.5.0.0  5.5.5.[0-1].[0-1]
     * 24bpp  8.8.8.0.0  8.8.8.0.0
     * 32bpp  8.8.8.0.0  8.8.8.[0-8].[0-8]
     */

    // bitmap pixel data
    auto *raw = reinterpret_cast<uint8_t const *>(data + bf->bfOffBits);

    // echo row is rounded up to a multiple of 4 bytes by padding
    auto row_size = (bi->biBitCount * w + 31) / 32 * 4;

    // TODO: find a way to remove the switch..case
    switch (bi->biBitCount) {
    case 1: case 4: case 8: // these are all paletted image
        {
            auto ppb = 8 / bi->biBitCount;  // pixel per byte
            for (int y = 0; y < h; y++) {
                auto *line_r = raw + row_size * y;
                auto *line_w = this->pixels_ + w * (flip_y ? y : (h - 1 - y));
                for (int x = 0; x < w; x++) {
                    auto *pixel_r = line_r + x / ppb;
                    auto *pixel_w = line_w + (flip_x ? w - 1 - x : x);

                    // TODO: make the point clearer?
                    uint8_t mask = ((1 << bi->biBitCount) - 1);
                    auto shift = (ppb - 1 - (x % ppb)) * bi->biBitCount;
                    auto index = ((*pixel_r) >> shift) & mask;
                    auto *color = reinterpret_cast<uint8_t const *>(palette + index);

                    uint8_t b = color[0];
                    uint8_t g = color[1];
                    uint8_t r = color[2];

                    *pixel_w = Bitmap::MakeColor(0xFF, r, g, b);
                }
            }
        }
        break;

    case 16:    // 1555 for uncompressed. ignore any alpha channel(bit)
        for (int y = 0; y < h; y++) {
            auto *line_r = raw + row_size * y;
            auto *line_w = this->pixels_ + w * (flip_y ? y : (h - 1 - y));
            for (int x = 0; x < w; x++) {
                auto *pixel_r = line_r + x * bi->biBitCount / 8;
                auto *pixel_w = line_w + (flip_x ? w - 1 - x : x);

                auto *color = pixel_r;

                // TODO: fix the ugly code :(
                uint8_t b = (((color[0] & 0x1F) << 0)                         ) * 255 / 31;
                uint8_t g = (((color[1] & 0x03) << 3) | (color[0] >> 5 & 0x07)) * 255 / 31;
                uint8_t r = (                           (color[1] >> 2 & 0x1F)) * 255 / 31;

                *pixel_w = Bitmap::MakeColor(0xFF, r, g, b);
            }
        }
        break;

    case 24:
    case 32:    // alpha channel unsupported
        for (int y = 0; y < h; y++) {
            auto *line_r = raw + row_size * y;
            auto *line_w = this->pixels_ + w * (flip_y ? y : (h - 1 - y));
            for (int x = 0; x < w; x++) {
                auto *pixel_r = line_r + x * bi->biBitCount / 8;
                auto *pixel_w = line_w + (flip_x ? w - 1 - x : x);

                auto *color = pixel_r;

                uint8_t b = color[0];
                uint8_t g = color[1];
                uint8_t r = color[2];

                *pixel_w = Bitmap::MakeColor(0xFF, r, g, b);
            }
        }
        break;

    default:
        return false;
    }

    return true;
}


