#ifndef ZOOMVIEW_H
#define ZOOMVIEW_H

#include <QGraphicsView>

class ZoomView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit ZoomView(QWidget *parent = nullptr);
    void softFitInView();

public slots:
    void fitInView();

signals:
    void mouseMove(QPoint pos);
    void mouseResize(QSize size);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    bool                        m_mouseResizing;
    bool                        m_mouseMoving;
    QSizeF                      m_startSize;
    QPointF                     m_startPosition;

};

#endif // ZOOMVIEW_H
