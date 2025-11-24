#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>
#include <QString>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QButtonGroup>
#include <QRadioButton>
#include <QIcon>
#include <QSlider>
#include <QSize>
#include <QStackedWidget>
#include <QHash>

#include "src/Game.h"
#include "src/Position.h"
#include "BoardWidget.h"

class GameWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GameWidget(QWidget *parent = nullptr);
private:
    struct GameSettings
    {
        Position::CheckerColor playerColor;
        int difficultyLevel;
    };

    enum IconBackgroundColors
    {
        LIGHT_BLUE,
        LIGHT_RED,
        LIGHT_GREEN,
        LIGHT_YELLOW,
        LIGHT_PURPLE,
        LIGHT_ORANGE,
        LIGHT_GRAY
    };

protected:

signals:
private slots:
    void onBoardResize(int xOffset, int yOffset, int boardSide);
    void onGameSettingsClicked();
    void onCancelGameSettingsClicked();
    void onSaveGameSettingsClicked();
private:
    QHash<IconBackgroundColors, QColor> m_iconBackgroundColors;
    GameSettings m_gameSettings;

    // === Board panel ===
    BoardWidget* m_boardWidget;
    QWidget* m_buttonsStack;
    QPushButton* m_playBtn;
    QPushButton* m_gameSettingsBtn;
    QPushButton* m_customizeBtn;
    QLabel* m_timerLb;

    QSpacerItem* m_topLeftPlayerSpace;
    QSpacerItem* m_topRightPlayerSpace;
    QSpacerItem* m_bottomLeftPlayerSpace;
    QSpacerItem* m_bottomRightPlayerSpace;

    QHBoxLayout* m_topPlayerInfoContainer;
    QHBoxLayout* m_bottomPlayerInfoContainer;

    QWidget* m_topPlayerInfoPanel;
    QWidget* m_bottomPlayerInfoPanel;

    QLabel* m_topPlayerIcon;
    QLabel* m_bottomPlayerIcon;
    // === Board panel ===

    QStackedWidget* m_centralContent;
    QWidget* m_boardWidgetContainer;
    QWidget* m_gameSettingsContainer;

    QWidget* m_gameSettingsPanel;

    QRadioButton* m_whiteColorOption;
    QRadioButton* m_blackColorOption;
    QSlider* m_chooseDiffLevel;
    QPushButton* m_saveGameSettingsBtn;
    QPushButton* m_cancelGameSettingsBtn;

    const QPixmap m_blackChecker;
    const QPixmap m_whiteChecker;
    QPixmap m_blackCheckerResized;
    QPixmap m_whiteCheckerResized;

    const QPixmap m_userMaleIcon;
    const QPixmap m_userFemaleIcon;
    QPixmap m_userMaleIconResized;
    QPixmap m_userFemaleIconResized;

    const QPixmap m_botIconEasy;
    const QPixmap m_botIconMedium;
    const QPixmap m_botIconHard1;
    const QPixmap m_botIconHard2;
    QPixmap m_botIconEasyResized;
    QPixmap m_botIconMediumResized;
    QPixmap m_botIconHard1Resized;
    QPixmap m_botIconHard2Resized;

    QString getDefaultIconStyle();
    void initIconBackgroundColors();
    void updateGameSettingsPanel();
    void saveGameSettings();
    void updateBotIcon();

    void initView();
    void initCentralPanel(QStackedWidget *centralContent);
    void initFunctionButtonsPanel(QVBoxLayout* functionButtons);
    void initBoardPanel(QWidget* boardWidgetContainer);
    void initGameStateLayout(QVBoxLayout* gameStateLayout);
    void initGameSettingsPanel(QWidget* gameSettingsContainer);
};

#endif // GAMEWIDGET_H
