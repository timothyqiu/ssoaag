Some Sort Of Ascii Art Generator...

There are two classes in `main.cc`, both can convert a bitmap to a web page:

* `ImageWriter` converts it to colored mosaics.
* `AsciiArtWriter` converts it to monochrome ascii art.

Currently, it can load bitmaps whose format is:

* Uncompressed
* 256 colors / 16bpp / 24bpp / 32bpp

and any alpha channel will be ignored :(

`bitmap.h` / `bitmap.cc` defines a `Bitmap` class for reading pixel data.

This project requries C++11 support. ;)

---

某种字符画生成器（？）

`main.cc` 中草草定义了两个有用的类，都会将位图转换为对应网页：

* `ImageWriter` 将位图转换为彩色马赛克形式。
* `AsciiArtWriter` 将位图转换为黑白字符画形式。

目前能够读取的位图格式只能是：

* 未压缩
* 256色/16位色/24位色/32位色

而且无视透明度 :(

`bitmap.h` / `bitmap.cc` 中定义了一个 `Bitmap` 类，用于读取像素颜色值
（本意就是想做这个来着）。

编译器需要支持 C++11 哦 ;)
