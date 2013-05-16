#include "fontawesomeiconengine.h"

#include <QFontDatabase>
#include <QPainter>

int FontAwesomeIconEngine::fontId = -1;
QString FontAwesomeIconEngine::fontName;
QHash<QString, QChar> FontAwesomeIconEngine::namedCodepoints;

FontAwesomeIconEngine::FontAwesomeIconEngine() :
    QIconEngine()
{
    loadFont();
}

FontAwesomeIconEngine::FontAwesomeIconEngine(const QString &iconName) :
    QIconEngine()
{
    setIconName(iconName);
}

void FontAwesomeIconEngine::setIconName(const QString &iconName)
{
    this->iconName = iconName;
}

QIconEngine *FontAwesomeIconEngine::clone() const
{
    return new FontAwesomeIconEngine(iconName);
}

void FontAwesomeIconEngine::paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state)
{
    painter->save();

    int pixelSize = qRound(rect.height()*0.625f);
    QFont font(fontName, pixelSize);
    painter->setFont(font);

    QString text = QString(namedCodepoints.value(iconName));
    painter->drawText(rect, Qt::AlignCenter, text);

    painter->restore();
}

QPixmap FontAwesomeIconEngine::pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
    QPixmap pm(size);
    pm.fill(Qt::transparent);

    {
        QPainter p(&pm);
        paint(&p, QRect(QPoint(0,0),size), mode, state);
    }

    return pm;
}

void FontAwesomeIconEngine::loadFont()
{
    if (fontId < 0) {
        fontId = QFontDatabase::addApplicationFont(":/fonts/fontawesome.ttf");

        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
        fontName = fontFamilies.at(0);

        namedCodepoints.insert("icon-repeat", QChar(0xf01e));
        namedCodepoints.insert("icon-print", QChar(0xf02f));
        namedCodepoints.insert("icon-bold", QChar(0xf032));
        namedCodepoints.insert("icon-italic", QChar(0xf033));
        namedCodepoints.insert("icon-list", QChar(0xf03a));
        namedCodepoints.insert("icon-picture", QChar(0xf03e));
        namedCodepoints.insert("icon-cut", QChar(0xf0c4));
        namedCodepoints.insert("icon-copy", QChar(0xf0c5));
        namedCodepoints.insert("icon-save", QChar(0xf0c7));
        namedCodepoints.insert("icon-list-ul", QChar(0xf0ca));
        namedCodepoints.insert("icon-list-ol", QChar(0xf0cb));
        namedCodepoints.insert("icon-strikethrough", QChar(0xf0cc));
        namedCodepoints.insert("icon-underline", QChar(0xf0cd));
        namedCodepoints.insert("icon-table", QChar(0xf0ce));
        namedCodepoints.insert("icon-undo", QChar(0xf0e2));
        namedCodepoints.insert("icon-paste", QChar(0xf0ea));
    }
}
