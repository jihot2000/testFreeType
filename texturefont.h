#ifndef TEXTUREFONT_H
#define TEXTUREFONT_H

#include <string>
#include <vector>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_TYPES_H
#include FT_OUTLINE_H
#include FT_RENDER_H

struct CharacterInfo
{
    unsigned int x;
    unsigned int y;
    unsigned int width;
    unsigned int height;
    unsigned int bitmap_left;
    unsigned int bitmap_top;
};

struct TextureCoord
{
    float left;
    float right;
    float top;
    float bottom;
};

class CharacterImage final
{
public:
    CharacterImage();
    CharacterImage(const CharacterImage& chimage);
    CharacterImage(CharacterImage&& chimage);
    CharacterImage& operator=(const CharacterImage& chimage);
    CharacterImage& operator=(CharacterImage&& chimage);
    ~CharacterImage();

    unsigned int width() const;
    unsigned int height() const;
    unsigned int bitmap_left() const;
    unsigned int bitmap_top() const;
    const unsigned char* image() const;

private:
    unsigned int m_width;
    unsigned int m_height;
    unsigned int m_bitmap_left;
    unsigned int m_bitmap_top;
    unsigned char* m_image;

    friend class TextureFont;
};

class TextureFont final
{
public:
    TextureFont(const char* fontFileName, unsigned int pt, unsigned int h_resolution, unsigned int v_resolution);
    TextureFont(const char* textureFontFileName);
    ~TextureFont();

    unsigned int pt() const;
    unsigned int characterTotalNum() const;
    unsigned int textureWidth() const;
    unsigned int textureHeight() const;
    CharacterInfo characterInfo(unsigned int unicode) const;
    TextureCoord textureCoord(unsigned int unicode) const;
    void saveToTextureFile(const char* textureFontFileName) const;
    const unsigned char* texture() const;
    CharacterImage characterImage(unsigned int unicode) const;

private:
    TextureFont& operator=(const TextureFont&) = delete;
    TextureFont& operator=(TextureFont&&) = delete;
    TextureFont(const TextureFont&) = delete;
    TextureFont(TextureFont &&) = delete;

private:
    FT_Library m_library;
    FT_Face m_face;
    unsigned char * m_texture;
    unsigned int m_textureWidth;   //in pixel
    unsigned int m_textureHeight;  //in pixel
    unsigned int m_characterTotalNum;
    unsigned int m_characterInfoInvalidIndex;
    unsigned int m_pt;  //in point
    std::vector<CharacterInfo> m_characterInfo;
    unsigned int * m_characterMap;
};

#endif // TEXTUREFONT_H
