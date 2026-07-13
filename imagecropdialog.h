#ifndef IMAGECROPDIALOG_H
#define IMAGECROPDIALOG_H

#include <QDialog>
#include <QPixmap>
#include <QPointF>
#include <QWidget>

class CropPreviewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CropPreviewWidget(QWidget* parent = nullptr);

    void setImage(const QPixmap& image);
    void setCropAspect(qreal aspect);
    void setZoom(qreal zoom);
    QPixmap croppedPixmap(const QSize& outputSize) const;

protected:
    bool event(QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    QRectF cropRect() const;
    qreal baseScale() const;
    QRectF imageRect() const;
    void clampOffset();

    QPixmap m_image;
    qreal m_aspect = 1.0;
    qreal m_zoom = 1.0;
    QPointF m_offset;
    QPointF m_lastMousePos;
    bool m_dragging = false;
};

class ImageCropDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImageCropDialog(const QString& imagePath,
                             qreal cropAspect,
                             const QSize& outputSize,
                             QWidget* parent = nullptr);

    QPixmap croppedPixmap() const;

private:
    CropPreviewWidget* m_preview = nullptr;
    QSize m_outputSize;
};

#endif // IMAGECROPDIALOG_H
