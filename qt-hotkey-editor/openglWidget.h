#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include "logo.h"

#include <QMatrix4x4>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>

class QOpenGLShaderProgram;

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
  Q_OBJECT

public:
  OpenGLWidget(QWidget *parent = nullptr);
  ~OpenGLWidget();

  QSize minimumSizeHint() const override;
  QSize sizeHint() const override;

public Q_SLOTS:
  void setXRotation(int angle);
  void setYRotation(int angle);
  void setZRotation(int angle);
  void cleanup();

Q_SIGNALS:
  void xRotationChanged(int angle);
  void yRotationChanged(int angle);
  void zRotationChanged(int angle);

protected:
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int width, int height) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;

private:
  void setupVertexAttribs();

  bool m_core;
  int m_xRot = 0;
  int m_yRot = 0;
  int m_zRot = 0;
  QPoint m_lastPos;
  Logo m_logo;
  QOpenGLVertexArrayObject m_vao;
  QOpenGLBuffer m_logoVbo;
  QOpenGLShaderProgram *m_program = nullptr;
  int m_projMatrixLoc = 0;
  int m_mvMatrixLoc = 0;
  int m_normalMatrixLoc = 0;
  int m_lightPosLoc = 0;
  QMatrix4x4 m_proj;
  QMatrix4x4 m_camera;
  QMatrix4x4 m_world;
};

#endif
