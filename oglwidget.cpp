#include "oglwidget.h"
#include <QVector>
#include <QDebug>
#include <assert.h>
#include <stdio.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_TYPES_H
#include FT_OUTLINE_H
#include FT_RENDER_H

#define TRUNC(x) ((x) >> 6)
#define CHECK_OPENGL_ES_ERROR(x) do { \
        x; \
        GLenum error = glGetError(); \
        if(error != GL_NO_ERROR) { \
            qDebug() << QString("[Error] %1, Line %2: %3").arg(__FILE__).arg(__LINE__).arg(getGLErrorString(error)); \
            exit(1); \
        } \
    } while(0)

QString getGLErrorString(GLenum err)
{
    switch(err)
    {
    case GL_NO_ERROR:
        return QString("GL_NO_ERROR: No error has been recorded.");
        break;
    case GL_INVALID_ENUM:
        return QString("GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.");
        break;
    case GL_INVALID_VALUE:
        return QString("GL_INVALID_VALUE: A numeric argument is out of range.");
        break;
    case GL_INVALID_OPERATION:
        return QString("GL_INVALID_OPERATION: The specified operation is not allowed in the current state.");
        break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return QString("GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete.");
        break;
    case GL_OUT_OF_MEMORY:
        return QString("GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.");
        break;
    case GL_STACK_UNDERFLOW:
        return QString("GL_STACK_UNDERFLOW: An attempt has been made to perform an operation that would cause an internal stack to underflow.");
        break;
    case GL_STACK_OVERFLOW:
        return QString("GL_STACK_OVERFLOW: An attempt has been made to perform an operation that would cause an internal stack to overflow.");
        break;
    default:
        return QString("");
        break;
    }
}

OGLWidget::OGLWidget(QWidget* parent, Qt::WindowFlags f)
    :QOpenGLWidget(parent, f)
    ,timer(nullptr)
    ,texFont(nullptr)
{
    texFont = new TextureFont("/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc", 12, 96, 96);
//    texFont->saveToTextureFile("/home/tang/texFont.tf");
//    texFont = new TextureFont("/home/tang/texFont.tf");

//    CharacterImage chimage = texFont->characterImage(QString("北").unicode()->unicode());
//    for(unsigned int i = 0; i < chimage.height(); i++)
//    {
//        for(unsigned int j = 0; j < chimage.width(); j++)
//        {
//            printf("%d ", chimage.image()[i*chimage.width()+j]>0?1:0);
//        }
//        printf("\n");
//    }

//    glyphImage = QImage(texFont->texture(),
//                      texFont->textureWidth(),
//                      texFont->textureHeight(),
//                      texFont->textureWidth(),
//                      QImage::Format_Indexed8);
//    QVector<QRgb> colorTable;
//    for(int i = 0; i < 256; ++i)
//    {
//        colorTable << qRgba(0, 0, 0, i);
//    }
//    glyphImage.setColorTable(colorTable);
//    glyphImage.save("/home/tang/texFont.png");

    timer = new QTimer(this);
    timer->setInterval(0);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));

    QSurfaceFormat tmpFormat;
    tmpFormat.setRenderableType(QSurfaceFormat::OpenGLES);
    tmpFormat.setProfile(QSurfaceFormat::NoProfile);
    tmpFormat.setVersion(3, 0);
    setFormat(tmpFormat);

    timer->start();
}

OGLWidget::~OGLWidget()
{
    delete texFont;
}

void OGLWidget::initializeGL()
{
    if(context()->isOpenGLES())
    {
        qDebug() << QString("OpenGL ES %1.%2").arg(format().majorVersion()).arg(format().minorVersion()).toStdString().data();
    }
    else
    {
        qDebug() << QString("OpenGL %1.%2").arg(format().majorVersion()).arg(format().minorVersion()).toStdString().data();
    }
    initializeOpenGLFunctions();

//    GLfloat tmpCubeVertex[24] = {
//        -0.5f, -0.5f, -0.5f,
//        0.5f, -0.5f, -0.5f,
//        0.5f, 0.5f, -0.5f,
//        -0.5f, 0.5f, -0.5f,
//        -0.5f, -0.5f, 0.5f,
//        0.5f, -0.5f, 0.5f,
//        0.5f, 0.5f, 0.5f,
//        -0.5f, 0.5f, 0.5f
//    };
//    GLushort tmpCubeIndex[] = {
//        0, 3, 1, 2, 5, 6, 4, 7, 255,
//        1, 5, 0, 4, 3, 7, 2, 6
//    };

//    GLfloat tmpCubeVertex[] = {
//        -1.0f, -1.0f, 0.0f,
//        -1.0f, 1.0f, 0.0f,
//        1.0f, -1.0f, 0.0f
//    };
//    GLushort tmpCubeIndex[] = {
//        0, 2, 1
//    };

    GLfloat tmpCubeVertex[6][12] = {
        {
            -0.5f, -0.5f, -0.5f,
            -0.5f, 0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, 0.5f, -0.5f
        },
        {
            -0.5f, 0.5f, -0.5f,
            -0.5f, 0.5f, 0.5f,
            0.5f, 0.5f, -0.5f,
            0.5f, 0.5f, 0.5f
        },
        {
            -0.5f, 0.5f, 0.5f,
            -0.5f, -0.5f, 0.5f,
            0.5f, 0.5f, 0.5f,
            0.5f, -0.5f, 0.5f
        },
        {
            -.5f, -.5f, .5f,
            -.5f, -.5f, -.5f,
            .5f, -.5f, .5f,
            .5f, -.5f, -.5f
        },
        {
            -.5f, .5f, .5f,
            -.5f, .5f, -.5f,
            -.5f, -.5f, .5f,
            -.5f, -.5f, -.5f
        },
        {
            .5f, .5f, .5f,
            .5f, -.5f, .5f,
            .5f, .5f, -.5f,
            .5f, -.5f, -.5f
        }
    };

    CHECK_OPENGL_ES_ERROR(glGenBuffers(6, glCubeVertexBuffer));
    for(int i = 0; i < 6; i++)
    {
        CHECK_OPENGL_ES_ERROR(glBindBuffer(GL_ARRAY_BUFFER, glCubeVertexBuffer[i]));
        CHECK_OPENGL_ES_ERROR(glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*12, tmpCubeVertex[i], GL_STATIC_DRAW));
    }

    QString texWords[6] = {"1", "2", "水", "4", "5", "6"};

    GLfloat tmpCubeTexCoord[6][8];
    for(int i = 0; i < 6; i++)
    {
        TextureCoord coord = texFont->textureCoord(texWords[i].unicode()->unicode());
        tmpCubeTexCoord[i][0] = coord.left;
        tmpCubeTexCoord[i][1] = coord.top;
        tmpCubeTexCoord[i][2] = coord.left;
        tmpCubeTexCoord[i][3] = coord.bottom;
        tmpCubeTexCoord[i][4] = coord.right;
        tmpCubeTexCoord[i][5] = coord.top;
        tmpCubeTexCoord[i][6] = coord.right;
        tmpCubeTexCoord[i][7] = coord.bottom;
    }

    CHECK_OPENGL_ES_ERROR(glGenBuffers(6, glCubeTexCoordBuffer));
    for(int i = 0; i < 6; i++)
    {
        CHECK_OPENGL_ES_ERROR(glBindBuffer(GL_ARRAY_BUFFER, glCubeTexCoordBuffer[i]));
        CHECK_OPENGL_ES_ERROR(glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*8, tmpCubeTexCoord[i], GL_STATIC_DRAW));
    }

    CHECK_OPENGL_ES_ERROR(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    CHECK_OPENGL_ES_ERROR(glGenTextures(1, &glFontTexture));
    CHECK_OPENGL_ES_ERROR(glBindTexture(GL_TEXTURE_2D, glFontTexture));
    CHECK_OPENGL_ES_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, texFont->textureWidth(), texFont->textureHeight(), 0, GL_RED, GL_UNSIGNED_BYTE, texFont->texture()));
    CHECK_OPENGL_ES_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    CHECK_OPENGL_ES_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

    const char* vShaderStr =
        "#version 300 es\n"
        "layout(location = 0) in vec4 vPosition;\n"
        "layout(location = 1) in vec2 vTexCoord;\n"
        "out vec2 v_TexCoord;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vPosition;\n"
        "    v_TexCoord = vTexCoord;\n"
        "}\n";

    const char* fShaderStr =
            "#version 300 es\n"
            "precision mediump float;\n"
            "uniform sampler2D s_tex0;\n"
            "in vec2 v_TexCoord;\n"
            "out vec4 fragColor;\n"
            "void main()\n"
            "{\n"
            "    fragColor = texture(s_tex0, v_TexCoord);\n // * vec4(1.0, 1.0, 1.0, 1.0);\n"
            "}\n";

    GLuint vertexShader;
    GLuint fragmentShader;
    GLint linked;
    GLint compiled;

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderStr, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled);
    assert(compiled == GL_TRUE);

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderStr, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled);
    assert(compiled == GL_TRUE);

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    assert(linked == GL_TRUE);

    CHECK_OPENGL_ES_ERROR(glClearColor(1.0, 1.0, 1.0, 1.0));
    CHECK_OPENGL_ES_ERROR(glClear(GL_COLOR_BUFFER_BIT));
}

void OGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void OGLWidget::paintGL()
{
    CHECK_OPENGL_ES_ERROR(glUseProgram(program));

    CHECK_OPENGL_ES_ERROR(glActiveTexture(GL_TEXTURE0));
    CHECK_OPENGL_ES_ERROR(glBindTexture(GL_TEXTURE_2D, glFontTexture));
    CHECK_OPENGL_ES_ERROR(glUniform1i(glGetUniformLocation(program, "s_tex0"), 0));

    CHECK_OPENGL_ES_ERROR(glEnableVertexAttribArray(0));
    CHECK_OPENGL_ES_ERROR(glEnableVertexAttribArray(1));
    for(int i = 0; i < 6; i++)
    {
        CHECK_OPENGL_ES_ERROR(glBindBuffer(GL_ARRAY_BUFFER, glCubeVertexBuffer[i]));
        CHECK_OPENGL_ES_ERROR(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0));
        CHECK_OPENGL_ES_ERROR(glBindBuffer(GL_ARRAY_BUFFER, glCubeTexCoordBuffer[i]));
        CHECK_OPENGL_ES_ERROR(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0));
//        glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
//        glEnable(GL_CULL_FACE);
        CHECK_OPENGL_ES_ERROR(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
//        glDisable(GL_CULL_FACE);
//        glDisable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
    }
    CHECK_OPENGL_ES_ERROR(glDisableVertexAttribArray(1));
    CHECK_OPENGL_ES_ERROR(glDisableVertexAttribArray(0));
}
