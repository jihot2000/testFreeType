#ifndef OGLWIDGET_H
#define OGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QTimer>
#include "texturefont.h"

class OGLWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
public:
    explicit OGLWidget(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    virtual ~OGLWidget();

    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;

private:
    GLuint glCubeVertexBuffer[6];
    GLuint glCubeTexCoordBuffer[6];
    GLuint glFontTexture;
    GLuint program;
    QImage glyphImage;
    QTimer * timer;
    TextureFont * texFont;
};

#endif // OGLWIDGET_H
