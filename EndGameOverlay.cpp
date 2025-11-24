#include <QPainter>
#include <QColor>
#include <QFont>
#include <QRect>
#include <QPoint>
#include <QString>
#include <QPen>
#include "EndGameOverlay.h"


EndGameOverlay::EndGameOverlay(QWidget *parent)
    : QWidget{parent}, m_alpha(0)
{
    updateFont();
}

void EndGameOverlay::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QColor overlayColor(0, 0, 0, m_alpha);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), overlayColor);

    painter.setFont(m_font);

    QColor outlineColor(Qt::black);
    QColor textColor(255, 50, 50);
    textColor = QColor(224, 13, 20);
    QPen outlinePen(outlineColor);
    outlinePen.setJoinStyle(Qt::RoundJoin);

    outlinePen.setWidthF(m_fontSize / 5.0);
    painter.setPen(outlinePen);
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(m_textPath);

    painter.setPen(Qt::NoPen);
    painter.setBrush(textColor);
    painter.drawPath(m_textPath);
}

void EndGameOverlay::resizeEvent(QResizeEvent *event)
{
    updateFont();
}

void EndGameOverlay::updateFont()
{
    m_fontSize = qMax(24, qMin(width(), height()) / 12);
    m_font = QFont("Arial", m_fontSize, QFont::Bold);

    QFontMetrics fm(m_font);
    QRect boundingRect = fm.boundingRect(m_statusText);
    m_textXOffset = (width() - boundingRect.width()) / 2;
    m_textYOffset = (height() + boundingRect.height()) / 2 - fm.descent();

    m_textPath = QPainterPath();
    m_textPath.addText(m_textXOffset, m_textYOffset, m_font, m_statusText);
}

void EndGameOverlay::setStatusText(Game::GameState gameState)
{
    switch (gameState)
    {
    case Game::GameState::DRAW:
        m_statusText = "Нічия";
        break;
    case Game::GameState::PLAYER_LOST:
        m_statusText = "Ви програли";
        break;
    case Game::GameState::PLAYER_WON:
        m_statusText = "Ви виграли!";
        break;
    default:
        m_statusText = "Undefined";
        break;
    }
}
