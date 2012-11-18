#include "html_writer.h"
#include <stdio.h>

std::string const AsciiArtWriter::s_default_characters_ = "@O1ir:. ";

// HTMLWriter
// ------------------------------------

HTMLWriter::HTMLWriter()
    : file_(NULL), title_("HTML Writer")
{
}

HTMLWriter::~HTMLWriter()
{
    if (this->file_) {
        fclose(this->file_);
        this->file_ = NULL;
    }
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

// ImageWriter
// ------------------------------------

ImageWriter::ImageWriter(std::string const& filename)
    : filename_(filename)
{
    this->SetTitle("Image to HTML: " + this->filename_);
}

void ImageWriter::WriteStyle()
{
    fprintf(this->file_, "%s\n", "<style type=\"text/css\">");
    fprintf(this->file_, "%s\n", "*{margin:0;padding:0;line-height:5px;}");
    fprintf(this->file_, "%s\n", ".pixel{width:5px;height:5px;float:left;}");
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
        fprintf(this->file_, "<p>Failed open %s: %s</p>",
                             this->filename_.c_str(),
                             bitmap.GetError().c_str());
    }
    fprintf(this->file_, "%s\n", "</div>");
}

void ImageWriter::WritePixel(Color color)
{
    fprintf(this->file_, "<div class=\"pixel\" style=\"background-color:#%06x\">", color & 0xFFFFFF);
    fprintf(this->file_, "</div>");
}

// AsciiArtWriter
// ------------------------------------

AsciiArtWriter::AsciiArtWriter(std::string const& filename,
                               std::string const& characters)
    : ImageWriter(filename), characters_(characters)
{
    this->SetTitle("Image to Ascii Art: " + this->filename_);
}

void AsciiArtWriter::WriteStyle()
{
    fprintf(this->file_, "%s\n", "<style type=\"text/css\">");
    fprintf(this->file_, "%s\n", "body{margin:0;padding:0;font-size:6px;line-height:6px;letter-spacing:0px;font-family:monospace;}");
    fprintf(this->file_, "%s\n", "</style>");
}

void AsciiArtWriter::WritePixel(Color color)
{
    uint8_t grey = (Bitmap::GetR(color) + Bitmap::GetG(color) + Bitmap::GetB(color)) / 3;

    auto char_count = this->characters_.length();
    if (char_count != 0) {
        auto span = 0xFF / char_count;
        auto cidx = grey / span;
        if (cidx == char_count)
            cidx = cidx - 1;

        fprintf(this->file_, "%s", Escape(this->characters_.substr(cidx, 1)).c_str());
    }
}

