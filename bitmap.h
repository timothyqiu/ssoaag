#ifndef BITMAP_H_
#define BITMAP_H_

#include <stdint.h>
#include <string>

/*
 * Bitmap
 * uncompressed 256colors/24bpp/32bpp bitmap (works on little-endian machine)
 * no alpha channel support
 */

typedef uint32_t Color; // 0xAARRGGBB format

class Bitmap
{
public:
    Bitmap();
    Bitmap(Bitmap const& other);
    Bitmap(Bitmap&& other); // other will go empty
    ~Bitmap();

    Bitmap& operator=(Bitmap const& rhs);

    bool Load(std::string const& filename);
    void Free();

    Color GetColor(int x, int y) const {
        // TODO: consider exception / assert on invalid position
        return this->pixels_[x + y * this->width_];
    }

    // returned image size won't be negative here
    int32_t GetWidth()  const { return width_;  }
    int32_t GetHeight() const { return height_; }

    static uint8_t GetR(Color c) { return (c >> 16) & 0xFF; }
    static uint8_t GetG(Color c) { return (c >>  8) & 0xFF; }
    static uint8_t GetB(Color c) { return (c >>  0) & 0xFF; }

    static Color MakeColor(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
        return static_cast<uint32_t>(a) << 24 |
               static_cast<uint32_t>(r) << 16 |
               static_cast<uint32_t>(g) <<  8 |
               static_cast<uint32_t>(b) <<  0 ;
    }

private:
    bool LoadData(char const *data);

    int32_t width_;
    int32_t height_;

    Color *pixels_;
};

#endif // BITMAP_H_

