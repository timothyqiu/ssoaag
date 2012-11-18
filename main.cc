#include "html_writer.h"
#include <stdio.h>

#define DEFAULT_IMAGE   ("data/sample-24bit.bmp")
#define MOSAIC_OUTPUT   ("mosaic.html")
#define ASCII_OUTPUT    ("ascii.html")

int main(int argc, char *argv[])
{
    char const *filename = nullptr;

    if (argc < 2) {
        printf("Some Sort Of Ascii Art Generator\n");
        printf("usage: %s <bitmap filename>\n", argv[0]);
        printf("using default image: %s\n", DEFAULT_IMAGE);

        filename = DEFAULT_IMAGE;
    } else {
        filename = argv[1];
    }

    ImageWriter    image_writer(filename);
    image_writer.WriteFile(MOSAIC_OUTPUT);

    AsciiArtWriter ascii_writer(filename);
    ascii_writer.WriteFile(ASCII_OUTPUT);
}

