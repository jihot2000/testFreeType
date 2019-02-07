#include "texturefont.h"
#include <iostream>
#include <iomanip>
#include <fstream>

#define TEXTURE_WIDTH 4096
#define BLANK_COLUMN 0
#define CHECK_FREETYPE_ERROR(expr) do { \
        if(FT_Error error = expr) { \
            std::cerr << "[FreeType Error 0x" << std::setbase(std::ios_base::hex) << error << std::setbase(std::ios_base::dec) << "] " << __FILE__ << ": Line " << __LINE__ << " "#expr << std::endl; \
            exit(1); \
        } \
    } \
    while(0)

CharacterImage::CharacterImage()
    :m_width(0)
    ,m_height(0)
    ,m_bitmap_left(0)
    ,m_bitmap_top(0)
    ,m_image(nullptr)
{

}

CharacterImage::CharacterImage(const CharacterImage& chimage)
    :m_width(chimage.m_width)
    ,m_height(chimage.m_height)
    ,m_bitmap_left(chimage.m_bitmap_left)
    ,m_bitmap_top(chimage.m_bitmap_top)
{
    m_image = new unsigned char[m_width * m_height];
    memcpy(m_image, chimage.m_image, m_width * m_height);
}

CharacterImage::CharacterImage(CharacterImage&& chimage)
    :m_width(chimage.m_width)
    ,m_height(chimage.m_height)
    ,m_bitmap_left(chimage.m_bitmap_left)
    ,m_bitmap_top(chimage.m_bitmap_top)
    ,m_image(chimage.m_image)
{
    chimage.m_image = nullptr;
}

CharacterImage& CharacterImage::operator=(const CharacterImage& chimage)
{
    m_width = chimage.m_width;
    m_height = chimage.m_height;
    m_bitmap_left = chimage.m_bitmap_left;
    m_bitmap_top = chimage.m_bitmap_top;
    m_image = new unsigned char[m_width * m_height];
    memcpy(m_image, chimage.m_image, m_width * m_height);
    return *this;
}

CharacterImage& CharacterImage::operator=(CharacterImage&& chimage)
{
    m_width = chimage.m_width;
    m_height = chimage.m_height;
    m_bitmap_left = chimage.m_bitmap_left;
    m_bitmap_top = chimage.m_bitmap_top;
    m_image = chimage.m_image;
    chimage.m_image = nullptr;
    return *this;
}

CharacterImage::~CharacterImage() {
    if(m_image)
    {
        delete [] m_image;
        m_image = nullptr;
    }
}

unsigned int CharacterImage::width() const
{
    return m_width;
}

unsigned int CharacterImage::height() const
{
    return m_height;
}

unsigned int CharacterImage::bitmap_left() const
{
    return m_bitmap_left;
}

unsigned int CharacterImage::bitmap_top() const
{
    return m_bitmap_top;
}

const unsigned char* CharacterImage::image() const
{
    return m_image;
}

TextureFont::TextureFont(const char* fontFileName, unsigned int pt, unsigned int h_resolution, unsigned int v_resolution)
    :m_texture(nullptr)
    ,m_textureWidth(0)
    ,m_textureHeight(0)
    ,m_characterTotalNum(0)
    ,m_characterInfoInvalidIndex(0)
    ,m_pt(0)
    ,m_characterMap(nullptr)
{
    CHECK_FREETYPE_ERROR(FT_Init_FreeType(&m_library));
    CHECK_FREETYPE_ERROR(FT_New_Face(m_library, fontFileName, 0, &m_face));
    CHECK_FREETYPE_ERROR(FT_Set_Char_Size(m_face, 0, pt * 64, h_resolution, v_resolution));
    m_pt = pt;
    m_characterTotalNum = m_face->num_glyphs;
    m_characterMap = new unsigned int[m_characterTotalNum];
    std::vector<std::vector<unsigned char> > tmpTexture;

    {
        unsigned int tmpLineWidth = 0;
        unsigned int tmpY = 0;
        unsigned int tmpMaxLineHeight = 0;
        bool firstInvalidGlyphIndex = true;
        unsigned int characterIndex = 0;

        for(unsigned int i = 0; i < m_characterTotalNum; i++)
        {
            FT_UInt glyph_index = FT_Get_Char_Index(m_face, i);
            if(glyph_index==0)
            {
                if(firstInvalidGlyphIndex)
                {
                    m_characterInfoInvalidIndex = i;
                    firstInvalidGlyphIndex = false;
                }
                else
                {
                    m_characterMap[i] = m_characterInfoInvalidIndex;
                    continue;
                }
            }

            CHECK_FREETYPE_ERROR(FT_Load_Glyph(m_face, glyph_index, FT_LOAD_DEFAULT));
            CHECK_FREETYPE_ERROR(FT_Render_Glyph(m_face->glyph, FT_RENDER_MODE_NORMAL));
            if(m_face->glyph->bitmap.width > TEXTURE_WIDTH) { \
                std::cerr << "[Error] " << __FILE__ << ": Line" << __LINE__ << " m_face->glyph->bitmap.width is greater than TEXTURE_WIDTH" << std::endl;
                exit(1);
            }
            CharacterInfo tmpInfo;
            tmpInfo.bitmap_left = m_face->glyph->bitmap_left;
            tmpInfo.bitmap_top = m_face->glyph->bitmap_top;
            tmpInfo.width = m_face->glyph->bitmap.width;
            tmpInfo.height = m_face->glyph->bitmap.rows;

            if(tmpLineWidth + m_face->glyph->bitmap.width + BLANK_COLUMN > TEXTURE_WIDTH)
            {
                for(unsigned int j = 0; j < tmpMaxLineHeight; j++)
                {
                    for(unsigned int k = 0; k < TEXTURE_WIDTH-tmpLineWidth; k++)
                    {
                        tmpTexture[tmpY+j].push_back(0);
                    }
                }
                tmpLineWidth = 0;
                tmpY += tmpMaxLineHeight;
                tmpMaxLineHeight = 0;
            }

            tmpInfo.x = tmpLineWidth + BLANK_COLUMN;
            tmpInfo.y = tmpY;
            m_characterInfo.push_back(tmpInfo);

            if(tmpMaxLineHeight < m_face->glyph->bitmap.rows)
            {
                for(unsigned int j = 0; j < m_face->glyph->bitmap.rows - tmpMaxLineHeight; j++)
                {
                    tmpTexture.push_back(std::vector<unsigned char>());
                    for(unsigned int k = 0; k < tmpLineWidth; k++)
                    {
                        tmpTexture[tmpY+tmpMaxLineHeight+j].push_back(0);
                    }
                }
                tmpMaxLineHeight = m_face->glyph->bitmap.rows;
            }

            for(unsigned int j = 0; j < m_face->glyph->bitmap.rows; j++)
            {
                for(unsigned int k = 0; k < BLANK_COLUMN; k++)
                {
                    tmpTexture[tmpY+j].push_back(0);
                }
                for(unsigned int k = 0; k < m_face->glyph->bitmap.width; k++)
                {
                    tmpTexture[tmpY+j].push_back(m_face->glyph->bitmap.buffer[j*m_face->glyph->bitmap.width+k]);
                }
            }
            for(unsigned int j = m_face->glyph->bitmap.rows; j < tmpMaxLineHeight; j++)
            {
                for(unsigned int k = 0; k < m_face->glyph->bitmap.width+BLANK_COLUMN; k++)
                {
                    tmpTexture[tmpY+j].push_back(0);
                }
            }

            m_characterMap[i] = characterIndex;

            tmpLineWidth += m_face->glyph->bitmap.width+BLANK_COLUMN;
            characterIndex++;
        }

        for(unsigned int j = 0; j < tmpMaxLineHeight; j++)
        {
            for(unsigned int k = 0; k < TEXTURE_WIDTH-tmpLineWidth; k++)
            {
                tmpTexture[tmpY+j].push_back(0);
            }
        }

        m_textureWidth = TEXTURE_WIDTH;
        m_textureHeight = tmpY + tmpMaxLineHeight;
    }

    m_texture = new unsigned char[m_textureWidth * m_textureHeight];
    for(unsigned int i = 0; i < m_textureHeight ; i++)
    {
        for(unsigned int j = 0; j < m_textureWidth; j++)
        {
            m_texture[i*m_textureWidth+j] = tmpTexture[i][j];
        }
    }

    CHECK_FREETYPE_ERROR(FT_Done_Face(m_face));
    CHECK_FREETYPE_ERROR(FT_Done_FreeType(m_library));
}

TextureFont::TextureFont(const char* textureFontFileName)
    :m_texture(nullptr)
    ,m_characterMap(nullptr)
{
    size_t characterInfoSize;
    std::ifstream stream;
    stream.open(textureFontFileName, std::ifstream::binary);

    stream.read(reinterpret_cast<char *>(&m_textureWidth), sizeof(m_textureWidth));
    stream.read(reinterpret_cast<char *>(&m_textureHeight), sizeof(m_textureHeight));
    stream.read(reinterpret_cast<char *>(&m_characterTotalNum), sizeof(m_characterTotalNum));
    stream.read(reinterpret_cast<char *>(&m_characterInfoInvalidIndex), sizeof(m_characterInfoInvalidIndex));
    stream.read(reinterpret_cast<char *>(&m_pt), sizeof(m_pt));
    stream.read(reinterpret_cast<char *>(&characterInfoSize), sizeof(characterInfoSize));
    std::cout << m_textureWidth << " " << m_textureHeight << " " << m_characterTotalNum << " " << m_characterInfoInvalidIndex << " " << m_pt << " " << characterInfoSize;
    m_texture = new unsigned char[m_textureWidth * m_textureHeight];
    m_characterMap = new unsigned int[m_characterTotalNum];
    stream.read(reinterpret_cast<char *>(m_texture), m_textureWidth * m_textureHeight);
    for(size_t i = 0; i < characterInfoSize; i++)
    {
        CharacterInfo tmpInfo;
        stream.read(reinterpret_cast<char *>(&tmpInfo), sizeof(tmpInfo));
        m_characterInfo.push_back(tmpInfo);
    }
    stream.read(reinterpret_cast<char *>(m_characterMap), m_characterTotalNum * sizeof(unsigned int));
    stream.close();
}

TextureFont::~TextureFont()
{
    if(m_texture)
    {
        delete [] m_texture;
        m_texture = nullptr;
    }
    if(m_characterMap)
    {
        delete [] m_characterMap;
        m_characterMap = nullptr;
    }
}

unsigned int TextureFont::pt() const
{
    return m_pt;
}

unsigned int TextureFont::characterTotalNum() const
{
    return m_characterTotalNum;
}

unsigned int TextureFont::textureWidth() const
{
    return m_textureWidth;
}

unsigned int TextureFont::textureHeight() const
{
    return m_textureHeight;
}

CharacterInfo TextureFont::characterInfo(unsigned int unicode) const
{
    if(unicode >= m_characterTotalNum)
    {
        return m_characterInfo[m_characterInfoInvalidIndex];
    }
    else
    {
        return m_characterInfo[m_characterMap[unicode]];
    }
}

TextureCoord TextureFont::textureCoord(unsigned int unicode) const
{
    CharacterInfo info = characterInfo(unicode);
    TextureCoord coord;
    coord.left = (float)info.x/(float)(m_textureWidth-1);
    coord.right = (float)(info.x+info.width-1)/(float)(m_textureWidth-1);
    coord.top = (float)info.y/(float)(m_textureHeight-1);
    coord.bottom = (float)(info.y+info.height-1)/(float)(m_textureHeight-1);
    return coord;
}

void TextureFont::saveToTextureFile(const char* textureFontFileName) const
{
    size_t szCharacterInfo = m_characterInfo.size();
    std::ofstream stream;
    stream.open(textureFontFileName, std::ofstream::binary);
    stream.write(reinterpret_cast<const char *>(&m_textureWidth), sizeof(m_textureWidth));
    stream.write(reinterpret_cast<const char *>(&m_textureHeight), sizeof(m_textureHeight));
    stream.write(reinterpret_cast<const char *>(&m_characterTotalNum), sizeof(m_characterTotalNum));
    stream.write(reinterpret_cast<const char *>(&m_characterInfoInvalidIndex), sizeof(m_characterInfoInvalidIndex));
    stream.write(reinterpret_cast<const char *>(&m_pt), sizeof(m_pt));
    stream.write(reinterpret_cast<const char *>(&szCharacterInfo), sizeof(szCharacterInfo));
    stream.write(reinterpret_cast<const char *>(m_texture), m_textureWidth * m_textureHeight);
    for(auto iter = m_characterInfo.begin(); iter != m_characterInfo.end(); iter++)
    {
        auto tmpChInfo = *iter;
        stream.write(reinterpret_cast<char *>(&tmpChInfo), sizeof(tmpChInfo));
    }
    stream.write(reinterpret_cast<char *>(m_characterMap), m_characterTotalNum * sizeof(unsigned int));
    stream.close();
}

const unsigned char* TextureFont::texture() const
{
    return m_texture;
}

CharacterImage TextureFont::characterImage(unsigned int unicode) const
{
    CharacterInfo chinfo = characterInfo(unicode);
    CharacterImage chimage;
    chimage.m_width = chinfo.width;
    chimage.m_height = chinfo.height;
    chimage.m_bitmap_left = chinfo.bitmap_left;
    chimage.m_bitmap_top = chinfo.bitmap_top;
    chimage.m_image = new unsigned char[chinfo.width * chinfo.height];
    for(unsigned int i = 0; i < chinfo.height; i++)
    {
        memcpy(chimage.m_image+i*chinfo.width, m_texture+(chinfo.y+i)*TEXTURE_WIDTH+chinfo.x, chinfo.width);
    }
    return chimage;
}
