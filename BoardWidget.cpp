#include "BoardWidget.h"

#include <QPainter>
#include <QPoint>
#include <QFont>
#include <QFontMetrics>
#include <QChar>
#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QEasingCurve>
#include <QPropertyAnimation>

#include <QDebug>
#include <QThread>


BoardWidget::BoardWidget(QWidget *parent)
    : QWidget{parent}, m_cellSelected(false), m_mousePressed(false),
    m_blackChecker(":/pic/images/checkers/black_checker.png"),
    m_whiteChecker(":/pic/images/checkers/white_checker.png"),
    m_blackKing(":/pic/images/checkers/black_king.png"),
    m_whiteKing(":/pic/images/checkers/white_king.png"),
    m_animDuration(100),
    m_animInterval(10),
    m_isAnimContinues(false),
    m_isGameOver(false)
{
    m_endGameOverlay = new EndGameOverlay(this);
    m_endGameOverlay->hide();

    m_animTimer = new QTimer(this);
    connect(
        m_animTimer, &QTimer::timeout,
        this, &BoardWidget::onAnimTimerTimeOut
        );
    setMouseTracking(true);
    m_highlightedCellExist = false;
}

void BoardWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    QColor borderColor(181, 136, 99);
    QColor lightColor(240, 217, 181);
    QColor darkColor(181, 136, 99);

    QColor lastMoveMarker = QColor(130, 7, 7);
    QColor highlightedCellColor = QColor(246, 255, 69);
    QColor selectedCellColor = highlightedCellColor;

    drawEmptyBoard(
        &painter,
        borderColor,
        lightColor,
        lightColor,
        darkColor,
        lightColor
        );

    highlightCells(&painter, lastMoveMarker,
                   highlightedCellColor, selectedCellColor);
    drawPosition(&painter);

    if (m_isAnimContinues)
        drawAnimatedChecker(&painter);

    // змінити розмір перекриваючої панелі завершення гри
    m_endGameOverlay->setGeometry(m_xOffset - m_coordinatesSpace,
        m_yOffset - m_coordinatesSpace,
        m_boardSide + 2 * m_coordinatesSpace,
        m_boardSide + 2 * m_coordinatesSpace);
}

void BoardWidget::drawEmptyBoard(
    QPainter *painter,
    QColor boardBackground,
    QColor coordLettersColor,
    QColor lightColor,
    QColor darkColor,
    QColor innerBorderColor
    )
{
    drawBoardFramework(painter, boardBackground, innerBorderColor);
    drawBoardCells(painter, lightColor, darkColor);
    drawCoordinates(painter, coordLettersColor);
}

void BoardWidget::drawBoardFramework(
    QPainter *painter,
    QColor boardBackground,
    QColor innerBorderColor
    )
{
    // фон дошки
    painter->fillRect(
        m_xOffset - m_coordinatesSpace,
        m_yOffset - m_coordinatesSpace,
        m_boardSide + 2 * m_coordinatesSpace,
        m_boardSide + 2 * m_coordinatesSpace,
        boardBackground);

    QPen pen = QPen(Qt::black);
    pen.setWidth(1);
    painter->setPen(pen);

    // виділити границі дошки
    painter->drawRect(
        m_xOffset - m_coordinatesSpace,
        m_yOffset - m_coordinatesSpace,
        m_boardSide + 2 * m_coordinatesSpace,
        m_boardSide + 2 * m_coordinatesSpace);

    pen = QPen(innerBorderColor);
    painter->setPen(pen);

    QColor transparent(0, 0, 0, 0);
    QBrush brush(transparent);
    painter->setBrush(brush);

    // межа області координат та клітин грального поля
    painter->drawRect(
        m_xOffset,
        m_yOffset,
        m_boardSide,
        m_boardSide);
}

void BoardWidget::highlightCells(
    QPainter* painter,
    QColor lastMoveMarker,
    QColor highlightedCellColor,
    QColor selectedCellColor
    )
{
    highlightLastMove(painter, lastMoveMarker);
    if (m_highlightedCellExist && !m_isGameOver)
        drawCell(m_highlightedCell, painter, highlightedCellColor);
    if (m_cellSelected && !m_isGameOver)
        drawCell(m_pressedCoords, painter, selectedCellColor);
}

void BoardWidget::drawBoardCells(QPainter *painter, QColor light, QColor dark)
{
    QColor currentColor;
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            currentColor = ((i + j) % 2 == 0) ?
                light : dark;

            drawCell(QPoint(j, i), painter, currentColor);
        }
    }
}

void BoardWidget::drawCell(QPoint point, QPainter *painter, QColor color)
{
    painter->fillRect(QRect(
        m_xOffset + point.x() * m_cellSize,
        m_yOffset + point.y() * m_cellSize,
        m_cellSize, m_cellSize),
        color);
}

void BoardWidget::resizeEvent(QResizeEvent *event)
{
    int minBoardSide = 340;
    int maxBoardSide = qMin(width(), height());
    int squareSide = qMax(minBoardSide, maxBoardSide);

    m_coordinatesSpace = squareSide / 12;
    m_cellSize = (squareSide - 2 * m_coordinatesSpace) / 8;
    m_pieceOffset = 0.1 * m_cellSize;

    m_boardSide = m_cellSize * 8;
    m_xOffset = (width() - m_boardSide) / 2;
    m_yOffset = (height() - m_boardSide) / 2;

    emit boardResized(
        m_xOffset - m_coordinatesSpace,
        m_yOffset - m_coordinatesSpace,
        squareSide);

    m_blackCheckerResized = m_blackChecker.scaled(
        m_cellSize - 2*m_pieceOffset, m_cellSize - 2*m_pieceOffset, Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    m_whiteCheckerResized = m_whiteChecker.scaled(
        m_cellSize - 2*m_pieceOffset, m_cellSize - 2*m_pieceOffset, Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    m_blackKingResized = m_blackKing.scaled(
        m_cellSize - 2*m_pieceOffset, m_cellSize - 2*m_pieceOffset, Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    m_whiteKingResized = m_whiteKing.scaled(
        m_cellSize - 2*m_pieceOffset, m_cellSize - 2*m_pieceOffset, Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );
}

void BoardWidget::drawCoordinates(QPainter* painter, QColor lettersColor)
{
    QPen pen = QPen(lettersColor);
    painter->setPen(pen);

    QFont font("Monospace");
    font.setWeight(QFont::Medium);
    int pixelSize = m_coordinatesSpace - 8;
    font.setPixelSize(pixelSize);
    QFontMetrics fm(font);

    int boardSize = 8 * m_cellSize;
    painter->setFont(font);

    char startLetter = 'A';
    char startNumber = '8';

    for (int i = 0; i < 8; ++i)
    {
        QRect leftRect(
            m_xOffset - m_coordinatesSpace,
            m_yOffset + i * m_cellSize,
            m_coordinatesSpace,
            m_cellSize
            );

        QRect rightRect(
            m_xOffset + boardSize,
            m_yOffset + i * m_cellSize,
            m_coordinatesSpace,
            m_cellSize
            );

        painter->drawText(leftRect, Qt::AlignCenter, QChar(startNumber));
        painter->drawText(rightRect, Qt::AlignCenter, QChar(startNumber));

        --startNumber;

        QRect topRect(
            m_xOffset + i * m_cellSize,
            m_yOffset - m_coordinatesSpace,
            m_cellSize,
            m_coordinatesSpace
            );

        QRect bottomRect(
            m_xOffset + i * m_cellSize,
            m_yOffset + boardSize,
            m_cellSize,
            m_coordinatesSpace
            );

        painter->drawText(topRect, Qt::AlignCenter, QChar(startLetter));
        painter->drawText(bottomRect, Qt::AlignCenter, QChar(startLetter));

        ++startLetter;
    }
}

void BoardWidget::drawPosition(QPainter* painter)
{
    Position::CellObjectType objType;

    for (int row = 0; row < 8; ++row)
    {
        for (int col = ((row % 2) ? 0 : 1); col < 8; col += 2)
        {
            // якщо шашка анімується, не малювати її тут
            if (m_isAnimContinues && isCurrentCheckerAnimating(QPoint(col, row)))
                continue;

            objType = m_currentPosition.getCellObjectType(row, col);
            drawChecker(painter, objType, row, col);
        }
    }
}

void BoardWidget::drawChecker(
    QPainter *painter,
    Position::CellObjectType checkerType,
    double row,
    double column)
{
    int x = m_xOffset + column * m_cellSize + m_pieceOffset;
    int y = m_yOffset + row * m_cellSize + m_pieceOffset;
    int size = m_cellSize - 2 * m_pieceOffset;

    switch (checkerType)
    {
    case Position::CellObjectType::BLACK_CHECKER:
        painter->drawPixmap(x, y, size, size, m_blackCheckerResized);
        break;

    case Position::CellObjectType::WHITE_CHECKER:
        painter->drawPixmap(x, y, size, size, m_whiteCheckerResized);
        break;

    case Position::CellObjectType::WHITE_QUEEN:
        painter->drawPixmap(x, y, size, size, m_whiteKingResized);
        break;

    case Position::CellObjectType::BLACK_QUEEN:
        painter->drawPixmap(x, y, size, size, m_blackKingResized);
        break;

    default:
        break;
    }
}

void BoardWidget::mousePressEvent(QMouseEvent *event)
{
    m_cellSelected = false;
    float x = (event->position().toPoint().x() - m_xOffset) / (float) m_cellSize;
    float y = (event->position().toPoint().y() - m_yOffset) / (float) m_cellSize;

    if (x >= 0 && x < 8 && y >= 0 && y < 8)
    {
        m_pressedCoords = QPoint((int) x, (int) y);
        m_mousePressed = true;
    }
    else
    {
        m_mousePressed = false;
        m_highlightedCellExist = false;
    }
    update();
}

void BoardWidget::mouseMoveEvent(QMouseEvent *event)
{

    float x = (event->position().toPoint().x() - m_xOffset) / (float) m_cellSize;
    float y = (event->position().toPoint().y() - m_yOffset) / (float) m_cellSize;

    if ((int) x != m_pressedCoords.x() || (int) y != m_pressedCoords.y())
        m_mousePressed = false;

    if (x >= 0 && x < 8 && y >= 0 && y < 8)
    {
        m_highlightedCell.setX((int) x);
        m_highlightedCell.setY((int) y);
        m_highlightedCellExist = true;
    }
    else
    {
        m_mousePressed = false;
        m_highlightedCellExist = false;
    }
    update();
}

void BoardWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_isGameOver)
        return;

    float x = (event->position().toPoint().x() - m_xOffset) / (float) m_cellSize;
    float y = (event->position().toPoint().y() - m_yOffset) / (float) m_cellSize;

    if (
        !(x >= 0 && x < 8 && y >= 0 && y < 8) ||
        ((int)x) != m_pressedCoords.x() ||
        ((int)y) != m_pressedCoords.y()
        )
    {
        m_mousePressed = false;
        m_cellSelected = false;
    }

    if (m_mousePressed)
    {
        emit cellSelected(m_pressedCoords);
        m_cellSelected = true;
        update();
    }
}

void BoardWidget::showEndOverlay()
{
    QPropertyAnimation *anim = new QPropertyAnimation(
        m_endGameOverlay, "alpha", this
        );
    anim->setDuration(400);
    anim->setStartValue(0);
    anim->setEndValue(100);
    anim->setEasingCurve(QEasingCurve::OutSine);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
    m_endGameOverlay->show();
}

void BoardWidget::onGameOverSignal(Game::GameState gameState)
{
    m_isGameOver = true;
    m_endGameOverlay->setStatusText(gameState);
    showEndOverlay();
}

void BoardWidget::updatePosition(Position newPosition)
{
    m_isAnimContinues = false;
    m_currentPosition = newPosition;
    update();
}

void BoardWidget::animateMove(
    QList<QPoint> movesChain,
    QList<QPoint> takenCheckers,
    Position::CellObjectType animatedCheckerType,
    QList<Position::CellObjectType> m_takenCheckersType,
    MoveInfo moveInfo
    )
{
    qDebug() << "Info: enter BoardWidget::animateMove()";

    m_lastMoveCells.clear();
    m_lastMoveCells.append(movesChain[0]);

    m_moveInfo = moveInfo;
    m_movesChain = movesChain;
    m_takenCheckers = takenCheckers;
    m_animatedCheckerType = moveInfo.beginCheckerType;
    m_takenCheckersType = m_takenCheckersType;
    m_animationsCount = m_movesChain.size() - 1;
    m_animationIndex = 1;

    m_animTimer->setInterval(m_animInterval);
    m_animTimer->start();
    m_isAnimContinues = true;
    m_timeEllapsed = 0;

    qDebug() << "Info: leaves BoardWidget::animateMove()";
}

void BoardWidget::drawAnimatedChecker(QPainter *painter)
{
    if (m_movesChain.size() < 2)
    {
        qDebug() << "Error: m_movesChain.size() < 2!";
        return;
    }

    QEasingCurve ec(QEasingCurve::InOutQuad);

    QPointF startPoint = m_movesChain[m_animationIndex - 1];
    QPointF endPoint = m_movesChain[m_animationIndex];
    double progress = double(m_timeEllapsed) / m_animDuration;
    if (progress > 1)
        progress = 1;

    progress = ec.valueForProgress(progress);

    QPointF animatedPoint = startPoint +
                            (endPoint - startPoint)
                            * progress;

    qDebug() << "progress:" << progress
             << "animatedPoint:" << animatedPoint;

    drawChecker(painter, m_animatedCheckerType, animatedPoint.y(), animatedPoint.x());
}

void BoardWidget::onAnimTimerTimeOut()
{
    m_timeEllapsed += m_animInterval;
    if (m_timeEllapsed > m_animDuration)
    {
        endOrContinueAnimation();
    }
    else
    {
        update();
    }
}

void BoardWidget::endOrContinueAnimation()
{
    m_lastMoveCells.append(m_movesChain[m_animationIndex]);
    // якщо шашка стала дамкою в цій точці, змінюємо її тип
    if (m_moveInfo.wasTransformed &&
        (MovesController::coordsToQPoint(m_moveInfo.pointOfTransformation) ==
                                      m_movesChain[m_animationIndex])
        )
    {
        m_animatedCheckerType = m_moveInfo.finalCheckerType;
    }
    if (m_animationIndex == m_animationsCount)
    {
        m_animTimer->stop();
        QTimer::singleShot(40, this, &BoardWidget::emitMoveAnimationEnded);
    }
    else
    {
        ++m_animationIndex;
        m_timeEllapsed = 0;
        m_animTimer->stop();
        update();
        QTimer::singleShot(100, this, &BoardWidget::startAnimTimer);
    }
}

void BoardWidget::startAnimTimer()
{
    m_animTimer->start();
}

void BoardWidget::emitMoveAnimationEnded()
{
    emit moveAnimationEnded();
}

void BoardWidget::highlightLastMove(QPainter *painter, QColor color)
{
    for (int i = 0; i < m_lastMoveCells.size(); ++i)
    {
        painter->fillRect(QRect(
            m_xOffset + m_lastMoveCells[i].x() * m_cellSize,
            m_yOffset + m_lastMoveCells[i].y() * m_cellSize,
            m_cellSize, m_cellSize),
            color);
    }
}

bool BoardWidget::isCurrentCheckerAnimating(QPoint point)
{
    if (!m_movesChain.isEmpty() && m_movesChain[0] == point)
        return true;

    for (int i = 0; i < m_takenCheckers.size(); ++i)
    {
        if (m_takenCheckers[i] == point)
            return true;
    }
    return false;
}




