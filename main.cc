#include "html_writer.h"
#include <stdio.h>

#define DEFAULT_IMAGE ("test-image.bmp")

int main(int argc, char *argv[])
{
    char const *filename = nullptr;

    if (argc < 2) {
        printf("usage: %s <bitmap filename>\n", argv[0]);
        printf("using default image: %s\n", DEFAULT_IMAGE);

        filename = DEFAULT_IMAGE;
    } else {
        filename = argv[1];
    }

    ImageWriter    image_writer(filename);
    AsciiArtWriter ascii_writer(filename);

    image_writer.WriteFile("mosaic.html");
    ascii_writer.WriteFile("ascii.html");
}

