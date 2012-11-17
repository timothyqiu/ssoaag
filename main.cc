#include "html_writer.h"

int main()
{
    ImageWriter    image_writer("test-image.bmp");
    AsciiArtWriter ascii_writer("test-image.bmp");

    image_writer.WriteFile("mosaic.html");
    ascii_writer.WriteFile("ascii.html");
}

