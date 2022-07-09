#ifndef VIEW_H
#define VIEW_H

#include <QElapsedTimer>
#include <QOpenGLWidget>

class Scene;

class View : public QOpenGLWidget
{
public:
    explicit View(QWidget* parent = nullptr);
    ~View();

private:
    void initializeGL() override;
    void paintGL() override;
    void	mouseMoveEvent(QMouseEvent* event) override;
    void	mousePressEvent(QMouseEvent* event) override; 

    Scene* _scene = nullptr;
    QElapsedTimer _elapsed;
};
