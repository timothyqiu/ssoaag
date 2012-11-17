#include <stdio.h>
#include <string.h>
#include <string>
#include "bitmap.h"

class HTMLWriter
{
public:
    HTMLWriter() : title_("HTML Writer") {}

    bool WriteFile(std::string const& filename);

    void SetTitle(std::string const& title) { title_ = title; }

    virtual void WriteStyle() {}
    virtual void WriteScript() {}
    virtual void WriteContent() {}

protected:
    std::string Escape(std::string const& input) const;

    FILE *file_;

private:
    void WriteHTML();
    void WriteHead();
    void WriteBody();

    std::string title_;
};

class ImageWriter : public HTMLWriter
{
public:
    ImageWriter(std::string const& image) : filename_(image) {
        this->SetTitle("Image to HTML: " + this->filename_);
    }

    virtual void WriteStyle();
    virtual void WriteContent();
    virtual void WritePixel(Color color);

protected:
    std::string filename_;
};

class AsciiArtWriter : public ImageWriter
{
public:
    AsciiArtWriter(std::string const& image) : ImageWriter(image) {
        this->SetTitle("Image to Ascii Art: " + this->filename_);
    }

    virtual void WritePixel(Color color);
};

int main()
{
    ImageWriter    image_writer("test-image.bmp");
    AsciiArtWriter ascii_writer("test-image.bmp");

    image_writer.WriteFile("mosaic.html");
    ascii_writer.WriteFile("ascii.html");
}

// Implementations
// ---------------

std::string HTMLWriter::Escape(std::string const& input) const
{
    std::string result;
    for (char c : input) {
        switch (c) {
        case ' ':   result.append("&nbsp;");    break;
        default:    result.push_back(c);
        }
    }
    return result;
}

bool HTMLWriter::WriteFile(std::string const& filename)
{
    this->file_ = fopen(filename.c_str(), "w");
    if (this->file_) {
        this->WriteHTML();
        fclose(this->file_);
    }
    return (this->file_ != NULL);
}

void HTMLWriter::WriteHTML()
{
    fprintf(this->file_, "%s\n", "<!DOCTYPE html>");
    fprintf(this->file_, "%s\n", "<html>");
    
    this->WriteHead();
    this->WriteBody();
    
    fprintf(this->file_, "%s\n", "</html>");
}

void HTMLWriter::WriteHead()
{
    fprintf(this->file_, "%s\n", "<head>");
    
    fprintf(this->file_, "%s\n", "<meta charset=\"utf-8\" />");
    fprintf(this->file_, "<title>%s</title>", this->title_.c_str());
    this->WriteStyle();
    this->WriteScript();
    
    fprintf(this->file_, "%s\n", "</head>");
}

void HTMLWriter::WriteBody()
{
    fprintf(this->file_, "%s\n", "<body>");
    this->WriteContent();
    fprintf(this->file_, "%s\n", "</body>");
}

void ImageWriter::WriteStyle()
{
    fprintf(this->file_, "%s\n", "<style type=\"text/css\">");
    fprintf(this->file_, "%s\n", "body{margin:0;padding:0;font-size:6px;line-height:6px;letter-spacing:0px;font-family:monospace;}");
    fprintf(this->file_, "%s\n", "</style>");
}

void ImageWriter::WriteContent()
{
    Bitmap bitmap;

    fprintf(this->file_, "%s\n", "<div class=\"image\">");
    if (bitmap.Load(this->filename_)) {
        auto w = bitmap.GetWidth();
        auto h = bitmap.GetHeight();
        for (auto y = 0; y < h; y++) {
            for (auto x = 0; x < w; x++) {
                Color color = bitmap.GetColor(x, y);
                this->WritePixel(color);
            }
            fprintf(this->file_, "%s\n", "<br />");
        }
    } else {
        fprintf(this->file_, "Failed open bitmap %s\n", this->filename_.c_str());
    }
    fprintf(this->file_, "%s\n", "</div>");
}

void ImageWriter::WritePixel(Color color)
{
    fprintf(this->file_, "<span style=\"color:#%06x\">", color & 0xFFFFFF);
    fprintf(this->file_, "%s", "■");
    fprintf(this->file_, "</span>");
}

void AsciiArtWriter::WritePixel(Color color)
{
    uint8_t grey = (Bitmap::GetR(color) + Bitmap::GetG(color) + Bitmap::GetB(color)) / 3;

    static std::string const s = "@O1ir:. ";

    auto span = 0xFF / s.length();
    auto cidx = grey / span;
    if (cidx == s.length())
        cidx = cidx - 1;

    fprintf(this->file_, "%s", Escape(s.substr(cidx, 1)).c_str());
}

