#include <QFont>

#include "mainwindow.h"
#include "src/Position.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setStyleSheet(
        "* {"
        "background-color: #302C2B;"
        "color: #ffffff;"
        "}"

        "QWidget#gameSettingsPanel, QWidget#gameSettingsPanel * {"
        "border-radius: 4px;"
        "background-color: #1F1F1F;"
        "}"

        "QPushButton, QWidget#gameSettingsPanel QPushButton {"
        "background-color: #474747;"
        "padding: 4px;"
        "border: 1px solid #615e5e;"
        "border-radius: 5px;"
        "font-family: Arial;"
        "font-size: 8pt;"
        "font-weight: Medium;"
        "}"

        "QPushButton:hover, QWidget#gameSettingsPanel QPushButton:hover {"
        "background-color: #5C5C5C;"
        "}"

        "QPushButton:pressed, QWidget#gameSettingsPanel QPushButton:pressed {"
        "background-color: #333333;"
        "}"
        );

    GameWidget* gw = new GameWidget();
    this->setCentralWidget(gw);
    resize(800, 500);

    // === Test play version ===
    // GameController* gm = new GameController(Position::WHITE, 6);
    // BoardWidget* bw = new BoardWidget();

    // gm->moveToThread(&gameContrThread);
    // connect(bw, &BoardWidget::cellSelected,
    //     gm, &GameController::analizeClickedPoint);
    // connect(bw, &BoardWidget::moveAnimationEnded,
    //         gm, &GameController::onMoveAnimationEnded);

    // connect(gm, &GameController::positionUpdated,
    //     bw, &BoardWidget::updatePosition);
    // connect(gm, &GameController::moveIsMade,
    //         bw, &BoardWidget::animateMove);
    // connect(gm, &GameController::gameEnded,
    //         bw, &BoardWidget::onGameOverSignal);
    // gameContrThread.start();


    // this->setCentralWidget(bw);
    // resize(500, 500);

    // bw->updatePosition(gm->getCurrentPosition());
    // gm->tryToMove();
}

MainWindow::~MainWindow()
{
    // delete gm;
    // delete bw;
    gameContrThread.quit();
    gameContrThread.wait();
}
