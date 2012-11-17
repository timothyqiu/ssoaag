#ifndef HTML_WRITER_H_
#define HTML_WRITER_H_

#include <string>
#include "bitmap.h"

// writes an empty html page
class HTMLWriter
{
public:
    HTMLWriter();
    ~HTMLWriter();

    bool WriteFile(std::string const& filename);

    void SetTitle(std::string const& title) { title_ = title; }

    virtual void WriteStyle() {}
    virtual void WriteScript() {}
    virtual void WriteContent() {}

protected:
    std::string Escape(std::string const& input) const;

    FILE *file_;

private:
    HTMLWriter(HTMLWriter const&);
    HTMLWriter& operator=(HTMLWriter const&);

    void WriteHTML();
    void WriteHead();
    void WriteBody();

    std::string title_;
};

// writes mosiac version of an image
class ImageWriter : public HTMLWriter
{
public:
    ImageWriter(std::string const& filename);

    virtual void WriteStyle();
    virtual void WriteContent();
    virtual void WritePixel(Color color);

protected:
    std::string filename_;
};

// writes ascii art version of an image
class AsciiArtWriter : public ImageWriter
{
public:
    AsciiArtWriter(std::string const& filename);

    virtual void WritePixel(Color color);
};

#endif // HTML_WRITER_H_

