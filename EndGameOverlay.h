#ifndef ENDGAMEOVERLAY_H
#define ENDGAMEOVERLAY_H

#include <QWidget>
#include <QString>
#include <QFont>
#include <QFontMetrics>
#include <QPainterPath>

#include "src/Game.h"

class EndGameOverlay : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(int alpha MEMBER m_alpha NOTIFY onAlphaUpdated)
public:
    explicit EndGameOverlay(QWidget *parent = nullptr);

    void setStatusText(Game::GameState gameState);
protected:
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);
private:
    QFont m_font;
    int m_alpha;
    int m_textXOffset;
    int m_textYOffset;
    int m_fontSize;
    QString m_statusText;
    QPainterPath m_textPath;

    void updateFont();
    void onAlphaUpdated() { update(); }
signals:
};

#endif // ENDGAMEOVERLAY_H
