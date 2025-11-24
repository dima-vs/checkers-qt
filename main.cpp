#include "mainwindow.h"
#include <QApplication>
#include <QMetaType>
#include "src/Position.h"
#include "src/CheckersMoves.h"
#include "src/Game.h"

#include "BoardWidget.h"

Q_DECLARE_METATYPE(Position)
Q_DECLARE_METATYPE(CheckersMoves::MovesManager::Move)
Q_DECLARE_METATYPE(CheckersMoves::MovesManager::MoveInfo)
Q_DECLARE_METATYPE(Position::CheckerType)
Q_DECLARE_METATYPE(Game::GameState)

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<Position>();
    qRegisterMetaType<CheckersMoves::MovesManager::Move>();
    qRegisterMetaType<CheckersMoves::MovesManager::MoveInfo>();
    qRegisterMetaType<Position::CheckerType>();
    qRegisterMetaType<Game::GameState>();

    MainWindow w;
    w.show();

    return a.exec();
}
