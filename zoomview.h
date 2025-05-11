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

protected:
    void resizeEvent(QResizeEvent *event) override;

private:

};

#endif // ZOOMVIEW_H
