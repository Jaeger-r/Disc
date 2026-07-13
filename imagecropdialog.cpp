#include "imagecropdialog.h"

#include <QDialogButtonBox>
#include <QGestureEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QNativeGestureEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPinchGesture>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWheelEvent>

CropPreviewWidget::CropPreviewWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(520, 360);
    setMouseTracking(true);
    grabGesture(Qt::PinchGesture);
}

void CropPreviewWidget::setImage(const QPixmap& image)
{
    m_image = image;
    m_offset = QPointF(0, 0);
    clampOffset();
    update();
}

void CropPreviewWidget::setCropAspect(qreal aspect)
{
    if (aspect > 0.1) {
        m_aspect = aspect;
    }
    clampOffset();
    update();
}

void CropPreviewWidget::setZoom(qreal zoom)
{
    m_zoom = qBound<qreal>(1.0, zoom, 3.0);
    clampOffset();
    update();
}

QRectF CropPreviewWidget::cropRect() const
{
    const QRectF bounds = rect().adjusted(34, 28, -34, -28);
    qreal width = bounds.width();
    qreal height = width / m_aspect;
    if (height > bounds.height()) {
        height = bounds.height();
        width = height * m_aspect;
    }
    return QRectF((rect().width() - width) / 2.0,
                  (rect().height() - height) / 2.0,
                  width,
                  height);
}

qreal CropPreviewWidget::baseScale() const
{
    if (m_image.isNull()) {
        return 1.0;
    }
    const QRectF crop = cropRect();
    return qMax(crop.width() / m_image.width(), crop.height() / m_image.height());
}

QRectF CropPreviewWidget::imageRect() const
{
    if (m_image.isNull()) {
        return QRectF();
    }
    const qreal scale = baseScale() * m_zoom;
    const QSizeF imageSize(m_image.width() * scale, m_image.height() * scale);
    const QRectF crop = cropRect();
    const QPointF topLeft(crop.center().x() - imageSize.width() / 2.0 + m_offset.x(),
                          crop.center().y() - imageSize.height() / 2.0 + m_offset.y());
    return QRectF(topLeft, imageSize);
}

void CropPreviewWidget::clampOffset()
{
    if (m_image.isNull()) {
        m_offset = QPointF(0, 0);
        return;
    }

    const QRectF crop = cropRect();
    const qreal scale = baseScale() * m_zoom;
    const QSizeF imageSize(m_image.width() * scale, m_image.height() * scale);
    const qreal maxX = qMax<qreal>(0, (imageSize.width() - crop.width()) / 2.0);
    const qreal maxY = qMax<qreal>(0, (imageSize.height() - crop.height()) / 2.0);
    m_offset.setX(qBound(-maxX, m_offset.x(), maxX));
    m_offset.setY(qBound(-maxY, m_offset.y(), maxY));
}

void CropPreviewWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(235, 241, 245));

    const QRectF crop = cropRect();
    if (!m_image.isNull()) {
        painter.drawPixmap(imageRect(), m_image, QRectF(m_image.rect()));
    }

    QPainterPath outer;
    outer.addRect(rect());
    QPainterPath inner;
    inner.addRoundedRect(crop, 18, 18);
    painter.fillPath(outer.subtracted(inner), QColor(18, 32, 44, 116));

    painter.setPen(QPen(QColor(255, 255, 255, 218), 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(crop, 18, 18);

    painter.setPen(QColor(255, 255, 255, 210));
    QFont hintFont = painter.font();
    hintFont.setPointSize(11);
    hintFont.setBold(true);
    painter.setFont(hintFont);
    painter.drawText(crop.adjusted(0, crop.height() - 42, 0, -12),
                     Qt::AlignHCenter | Qt::AlignBottom,
                     QStringLiteral("拖动调整位置，双指捏合缩放"));
}

bool CropPreviewWidget::event(QEvent* event)
{
    if (event->type() == QEvent::NativeGesture) {
        auto* nativeGesture = static_cast<QNativeGestureEvent*>(event);
        if (nativeGesture->gestureType() == Qt::ZoomNativeGesture && !qFuzzyIsNull(nativeGesture->value())) {
            setZoom(m_zoom * (1.0 + nativeGesture->value()));
            event->accept();
            return true;
        }
    }
    if (event->type() == QEvent::Gesture) {
        auto* gestureEvent = static_cast<QGestureEvent*>(event);
        if (auto* pinch = static_cast<QPinchGesture*>(gestureEvent->gesture(Qt::PinchGesture))) {
            if (pinch->changeFlags().testFlag(QPinchGesture::ScaleFactorChanged)) {
                setZoom(m_zoom * pinch->scaleFactor());
                gestureEvent->accept(pinch);
                return true;
            }
        }
    }
    return QWidget::event(event);
}

void CropPreviewWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton || !cropRect().contains(event->pos())) {
        return;
    }
    m_dragging = true;
    m_lastMousePos = event->position();
}

void CropPreviewWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_dragging) {
        return;
    }
    const QPointF delta = event->position() - m_lastMousePos;
    m_lastMousePos = event->position();
    m_offset += delta;
    clampOffset();
    update();
}

void CropPreviewWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
    }
}

void CropPreviewWidget::wheelEvent(QWheelEvent* event)
{
    const QPoint angle = event->angleDelta();
    const QPoint pixel = event->pixelDelta();
    const qreal delta = !pixel.isNull() ? pixel.y() : angle.y() / 8.0;
    if (qFuzzyIsNull(delta)) {
        event->ignore();
        return;
    }
    const qreal factor = delta > 0 ? 1.045 : 0.955;
    setZoom(m_zoom * factor);
    event->accept();
}

void CropPreviewWidget::resizeEvent(QResizeEvent*)
{
    clampOffset();
}

QPixmap CropPreviewWidget::croppedPixmap(const QSize& outputSize) const
{
    QPixmap result(outputSize);
    result.fill(Qt::transparent);
    if (m_image.isNull()) {
        return result;
    }

    QPainter painter(&result);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    const QRectF crop = cropRect();
    const QRectF drawn = imageRect();
    const qreal outputScaleX = outputSize.width() / crop.width();
    const qreal outputScaleY = outputSize.height() / crop.height();
    const QRectF target((drawn.left() - crop.left()) * outputScaleX,
                        (drawn.top() - crop.top()) * outputScaleY,
                        drawn.width() * outputScaleX,
                        drawn.height() * outputScaleY);
    painter.drawPixmap(target, m_image, QRectF(m_image.rect()));
    return result;
}

ImageCropDialog::ImageCropDialog(const QString& imagePath,
                                 qreal cropAspect,
                                 const QSize& outputSize,
                                 QWidget* parent)
    : QDialog(parent)
    , m_outputSize(outputSize)
{
    setWindowTitle(QStringLiteral("裁剪图片"));
    setMinimumWidth(620);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(14);

    auto* title = new QLabel(QStringLiteral("把想保留的部分放进裁剪框"), this);
    title->setObjectName(QStringLiteral("titleLabel"));
    m_preview = new CropPreviewWidget(this);
    m_preview->setCropAspect(cropAspect);
    m_preview->setImage(QPixmap(imagePath));

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok, this);
    if (auto* okButton = buttons->button(QDialogButtonBox::Ok)) {
        okButton->setText(QStringLiteral("使用裁剪"));
    }
    if (auto* cancelButton = buttons->button(QDialogButtonBox::Cancel)) {
        cancelButton->setText(QStringLiteral("取消"));
    }
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    layout->addWidget(title);
    layout->addWidget(m_preview, 1);
    layout->addWidget(buttons);

    setStyleSheet(QStringLiteral(
        "QDialog { background: #eef3f7; color: #16324f; }"
        "QLabel#titleLabel { font-size: 17px; font-weight: 800; }"
        "QPushButton { min-height: 36px; border-radius: 12px; border: none; padding: 0 16px; font-weight: 700; background: #0b8793; color: white; }"
        "QPushButton:hover { background: #086a74; }"));
}

QPixmap ImageCropDialog::croppedPixmap() const
{
    return m_preview ? m_preview->croppedPixmap(m_outputSize) : QPixmap();
}
