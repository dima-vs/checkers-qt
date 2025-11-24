#include "GameWidget.h"
#include <QResizeEvent>

GameWidget::GameWidget(QWidget *parent)
    : QWidget{parent}, m_boardWidget(new BoardWidget()),
    m_gameSettings { Position::CheckerColor::WHITE, 1 },

    m_blackChecker(":/pic/images/checkers/black_checker.png"),
    m_whiteChecker(":/pic/images/checkers/white_checker.png"),

    m_userMaleIcon(":/pic/images/icons/user_icon_male.png"),
    m_userFemaleIcon(":/pic/images/icons/user_icon_female.png"),

    m_botIconEasy(":/pic/images/icons/bot_icon1.png"),
    m_botIconMedium(":/pic/images/icons/bot_icon2.png"),
    m_botIconHard1(":/pic/images/icons/bot_icon3.png"),
    m_botIconHard2(":/pic/images/icons/bot_icon4.png")
{
    initIconBackgroundColors();
    initView();

    connect(
        m_boardWidget,
        &BoardWidget::boardResized,
        this,
        &GameWidget::onBoardResize
        );
}

void GameWidget::initIconBackgroundColors()
{
    m_iconBackgroundColors.insert(LIGHT_BLUE, QColor("#5ABFFB"));
    m_iconBackgroundColors.insert(LIGHT_RED, QColor("#FF6B6B"));
    m_iconBackgroundColors.insert(LIGHT_GREEN, QColor("#6EE7B7"));
    m_iconBackgroundColors.insert(LIGHT_YELLOW, QColor("#FDE68A"));
    m_iconBackgroundColors.insert(LIGHT_PURPLE, QColor("#C4B5FD"));
    m_iconBackgroundColors.insert(LIGHT_ORANGE, QColor("#FDBA74"));
    m_iconBackgroundColors.insert(LIGHT_GRAY, QColor("#CBD5E1"));
}

void GameWidget::initView()
{
    QFont globalFont = QFont("Arial", 10, QFont::Black);
    setFont(globalFont);

    QHBoxLayout* mainLayout = new QHBoxLayout();

    // === Ліва вертикальна панель основних кнопок ===
    QVBoxLayout* functionButtons = new QVBoxLayout();
    initFunctionButtonsPanel(functionButtons);

    // === Середня панель ===
    m_centralContent = new QStackedWidget();
    initCentralPanel(m_centralContent);

    // === Права вертикальна панель стану гри ===
    QVBoxLayout* gameStateLayout = new QVBoxLayout();
    initGameStateLayout(gameStateLayout);

    // === Встановлення mainLayout ===
    mainLayout->addLayout(functionButtons);
    mainLayout->addWidget(m_centralContent);
    mainLayout->addLayout(gameStateLayout);
    setLayout(mainLayout);
}

void GameWidget::initCentralPanel(QStackedWidget* centralContent)
{
    m_boardWidgetContainer = new QWidget();
    initBoardPanel(m_boardWidgetContainer);

    m_gameSettingsContainer = new QWidget();
    initGameSettingsPanel(m_gameSettingsContainer);

    centralContent->addWidget(m_boardWidgetContainer);
    centralContent->addWidget(m_gameSettingsContainer);

    centralContent->setCurrentWidget(m_boardWidgetContainer);
}

void GameWidget::initFunctionButtonsPanel(QVBoxLayout* functionButtons)
{
    QVBoxLayout* buttonsStack = new QVBoxLayout();
    m_buttonsStack = new QWidget();

    m_playBtn = new QPushButton("Нова гра");
    m_gameSettingsBtn = new QPushButton("Налаштування гри");
    m_customizeBtn = new QPushButton("Персоналізація");

    buttonsStack->addWidget(m_playBtn);
    buttonsStack->addWidget(m_gameSettingsBtn);
    buttonsStack->addWidget(m_customizeBtn);
    buttonsStack->addStretch();

    m_buttonsStack->setLayout(buttonsStack);
    functionButtons->addWidget(m_buttonsStack);
}

void GameWidget::initBoardPanel(QWidget* boardWidgetContainer)
{
    QVBoxLayout* centralBoardLayout = new QVBoxLayout();
    m_topPlayerInfoPanel = new QWidget();
    m_bottomPlayerInfoPanel = new QWidget();
    QString infoPanelStyle =
        "background-color: #1F1F1F;"
        "border-radius: 4px;"
        "font-family: Arial;"
        "font-size: 9pt;"
        "font-weight: bold;";

    m_topPlayerInfoPanel->setStyleSheet(infoPanelStyle);
    m_bottomPlayerInfoPanel->setStyleSheet(infoPanelStyle);

    m_topPlayerInfoContainer = new QHBoxLayout();
    m_bottomPlayerInfoContainer = new QHBoxLayout();

    QHBoxLayout* topPlayerInfo = new QHBoxLayout(m_topPlayerInfoPanel);
    QHBoxLayout* bottomPlayerInfo = new QHBoxLayout(m_bottomPlayerInfoPanel);

    // відступи
    m_topLeftPlayerSpace = new QSpacerItem(0, 0);
    m_topRightPlayerSpace = new QSpacerItem(0, 0);
    m_bottomLeftPlayerSpace = new QSpacerItem(0, 0);
    m_bottomRightPlayerSpace = new QSpacerItem(0, 0);

    m_topPlayerInfoContainer->addItem(m_topLeftPlayerSpace);
    m_topPlayerInfoContainer->addWidget(m_topPlayerInfoPanel);
    m_topPlayerInfoContainer->addItem(m_topRightPlayerSpace);

    m_bottomPlayerInfoContainer->addItem(m_bottomLeftPlayerSpace);
    m_bottomPlayerInfoContainer->addWidget(m_bottomPlayerInfoPanel);
    m_bottomPlayerInfoContainer->addItem(m_bottomRightPlayerSpace);

    // верхня інформація (противник)
    QLabel* topPlayerIcon = new QLabel();
    m_userMaleIconResized = m_userFemaleIcon.scaled(
        32, 32, Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );
    topPlayerIcon->setPixmap(m_userMaleIconResized);
    QLabel* topPlayerLabel = new QLabel("Противник");

    // нижня інформація (гравець)
    QWidget* bottomPlayerIcon = new QWidget();
    QLabel* bottomPlayerLabel = new QLabel("Ви");

    QString iconStyle =
        "background-color: #c0c0c0;"
        "border-radius: 6px;";

    topPlayerIcon->setFixedSize(32, 32);
    topPlayerIcon->setStyleSheet(iconStyle);

    bottomPlayerIcon->setFixedSize(32, 32);
    bottomPlayerIcon->setStyleSheet(iconStyle);

    // противник (зверху)
    topPlayerInfo->addWidget(topPlayerIcon);
    topPlayerInfo->addSpacing(6);
    topPlayerInfo->addWidget(topPlayerLabel);
    topPlayerInfo->addStretch();

    // гравець (знизу)
    bottomPlayerInfo->addWidget(bottomPlayerIcon);
    bottomPlayerInfo->addSpacing(6);
    bottomPlayerInfo->addWidget(bottomPlayerLabel);
    bottomPlayerInfo->addStretch();

    // додаємо елементи в центральний макет
    m_boardWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    centralBoardLayout->addLayout(m_topPlayerInfoContainer);
    centralBoardLayout->addWidget(m_boardWidget);
    centralBoardLayout->addLayout(m_bottomPlayerInfoContainer);

    boardWidgetContainer->setLayout(centralBoardLayout);
}

void GameWidget::initGameStateLayout(QVBoxLayout* gameStateLayout)
{
    m_timerLb = new QLabel("00:00");
    const int checkersPixmapSize = 24;

    m_whiteCheckerResized = m_whiteChecker.scaled(
        checkersPixmapSize, checkersPixmapSize, Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    m_blackCheckerResized = m_blackChecker.scaled(
        checkersPixmapSize, checkersPixmapSize, Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    QHBoxLayout* whiteRemainingLayout = new QHBoxLayout();
    QHBoxLayout* blackRemainingLayout = new QHBoxLayout();

    QLabel* whiteCheckerImageLb = new QLabel();
    QLabel* whiteCheckerTextLb = new QLabel("0");
    QLabel* blackCheckerImageLb = new QLabel();
    QLabel* blackCheckerTextLb = new QLabel("0");

    whiteCheckerImageLb->setPixmap(m_whiteCheckerResized);
    blackCheckerImageLb->setPixmap(m_blackCheckerResized);

    whiteRemainingLayout->addWidget(whiteCheckerImageLb);
    whiteRemainingLayout->addWidget(whiteCheckerTextLb);
    blackRemainingLayout->addWidget(blackCheckerImageLb);
    blackRemainingLayout->addWidget(blackCheckerTextLb);

    gameStateLayout->addWidget(m_timerLb);
    gameStateLayout->addLayout(whiteRemainingLayout);
    gameStateLayout->addLayout(blackRemainingLayout);
    gameStateLayout->addStretch();
}

void GameWidget::initGameSettingsPanel(QWidget* gameSettingsContainer)
{
    QWidget* gameSettingsPanel = new QWidget();
    QVBoxLayout* mainLayout = new QVBoxLayout();

    // === Заголовок ===
    QHBoxLayout* headerLayout = new QHBoxLayout();
    QString headerStyle =
        "font-family: Arial;"
        "font-size: 18pt;"
        "font-weight: bold;";
    QLabel* header = new QLabel("Налаштування гри");
    header->setStyleSheet(headerStyle);
    headerLayout->addStretch();
    headerLayout->addWidget(header);
    headerLayout->addStretch();

    // === Вибір кольору ===
    QHBoxLayout* selectCheckerColorLayout = new QHBoxLayout();
    int checkerIconSize = 36;
    QPixmap whiteScaled = m_whiteChecker.scaled(
        checkerIconSize, checkerIconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QPixmap blackScaled = m_blackChecker.scaled(
        checkerIconSize, checkerIconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QLabel* whiteCheckerImgLb = new QLabel();
    whiteCheckerImgLb->setPixmap(whiteScaled);
    QLabel* whiteCheckerTextLb = new QLabel("Білі");

    QLabel* blackCheckerImgLb = new QLabel();
    blackCheckerImgLb->setPixmap(blackScaled);
    QLabel* blackCheckerTextLb = new QLabel("Чорні");

    QRadioButton* whiteOp = new QRadioButton();
    QRadioButton* blackOp = new QRadioButton();

    QButtonGroup* checkerColorGroup = new QButtonGroup(gameSettingsPanel);
    checkerColorGroup->addButton(whiteOp);
    checkerColorGroup->addButton(blackOp);

    selectCheckerColorLayout->addStretch(4);
    selectCheckerColorLayout->addWidget(whiteOp);
    selectCheckerColorLayout->addSpacing(4);
    selectCheckerColorLayout->addWidget(whiteCheckerImgLb);
    selectCheckerColorLayout->addSpacing(4);
    selectCheckerColorLayout->addWidget(whiteCheckerTextLb);
    selectCheckerColorLayout->addStretch(1);

    selectCheckerColorLayout->addWidget(blackOp);
    selectCheckerColorLayout->addSpacing(4);
    selectCheckerColorLayout->addWidget(blackCheckerImgLb);
    selectCheckerColorLayout->addSpacing(4);
    selectCheckerColorLayout->addWidget(blackCheckerTextLb);
    selectCheckerColorLayout->addStretch(4);

    if (m_gameSettings.playerColor == Position::WHITE)
        whiteOp->setChecked(true);
    else
        blackOp->setChecked(true);

    // === Вибір рівня складності ===
    QHBoxLayout* selectDiffLevelLayout = new QHBoxLayout();
    const int botIconsSize = 70;
    QPixmap botIconEasyScaled = m_botIconEasy.scaled(
        botIconsSize, botIconsSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QPixmap botIconHardScaled = m_botIconHard2.scaled(
        botIconsSize, botIconsSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QWidget* botIconEasyContainer = new QWidget();
    QWidget* botIconHardContainer = new QWidget();

    QVBoxLayout* verIconEasyLayout = new QVBoxLayout();
    QHBoxLayout* horIconEasyLayout = new QHBoxLayout();

    QVBoxLayout* verIconHardLayout = new QVBoxLayout();
    QHBoxLayout* horIconHardLayout = new QHBoxLayout();

    verIconEasyLayout->setContentsMargins(0, 0, 0, 0);
    verIconEasyLayout->setSpacing(0);
    horIconEasyLayout->setContentsMargins(0, 0, 0, 0);
    horIconEasyLayout->setSpacing(0);
    verIconHardLayout->setContentsMargins(0, 0, 0, 0);
    verIconHardLayout->setSpacing(0);
    horIconHardLayout->setContentsMargins(0, 0, 0, 0);
    horIconHardLayout->setSpacing(0);

    QLabel* botIconEasyLb = new QLabel(botIconEasyContainer);
    QLabel* botIconHardLb = new QLabel(botIconHardContainer);

    // === Easy icon container ===
    // Центрування по горизонталі
    horIconEasyLayout->addStretch();
    horIconEasyLayout->addWidget(botIconEasyLb);
    horIconEasyLayout->addStretch();

    // Центрування по вертикалі
    verIconEasyLayout->addStretch();
    verIconEasyLayout->addLayout(horIconEasyLayout);
    verIconEasyLayout->addStretch();

    botIconEasyContainer->setLayout(verIconEasyLayout);

    // === Hard icon container ===
    // Центрування по горизонталі
    horIconHardLayout->addStretch();
    horIconHardLayout->addWidget(botIconHardLb);
    horIconHardLayout->addStretch();

    // Центрування по вертикалі
    verIconHardLayout->addStretch();
    verIconHardLayout->addLayout(horIconHardLayout);
    verIconHardLayout->addStretch();

    botIconHardContainer->setLayout(verIconHardLayout);

    botIconEasyLb->setFixedSize(botIconsSize, botIconsSize);
    botIconHardLb->setFixedSize(botIconsSize, botIconsSize);
    botIconEasyLb->setPixmap(botIconEasyScaled);
    botIconHardLb->setPixmap(botIconHardScaled);

    int botIconsContainerSize = botIconsSize + 4;
    QString botIconContainerStyle =
        "background-color: #615e5e;"
        "border-radius: " +
        QString::number(botIconsContainerSize / 2) + "px;";
    botIconEasyContainer->setFixedSize(botIconsContainerSize, botIconsContainerSize);
    botIconHardContainer->setFixedSize(botIconsContainerSize, botIconsContainerSize);
    botIconEasyContainer->setStyleSheet(botIconContainerStyle);
    botIconHardContainer->setStyleSheet(botIconContainerStyle);

    QString botIconLbStyle =
        "border-radius: " +
        QString::number(botIconsSize / 2) + "px;";
    QString easyBotIconLbStyle = "background-color: " +
        m_iconBackgroundColors[LIGHT_BLUE].name() + ";";
    QString hardBotIconLbStyle = "background-color: " +
            m_iconBackgroundColors[LIGHT_RED].name() + ";";

    botIconEasyLb->setStyleSheet(botIconLbStyle + easyBotIconLbStyle);
    botIconHardLb->setStyleSheet(botIconLbStyle + hardBotIconLbStyle);

    QSlider* chooseDiffLevel = new QSlider(Qt::Horizontal);
    chooseDiffLevel->setMinimum(1);
    chooseDiffLevel->setMaximum(10);
    chooseDiffLevel->setSliderPosition(1);
    chooseDiffLevel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    selectDiffLevelLayout->addStretch(4);
    selectDiffLevelLayout->addWidget(botIconEasyContainer);
    selectDiffLevelLayout->addWidget(chooseDiffLevel, 10);
    selectDiffLevelLayout->addWidget(botIconHardContainer);
    selectDiffLevelLayout->addStretch(4);

    // === Кнопки збереження та скасування ===
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    QPushButton* saveBtn = new QPushButton("Зберегти");
    QPushButton* cancelBtn = new QPushButton("Скасувати");

    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(saveBtn, 8);
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(cancelBtn, 8);
    buttonsLayout->addStretch(1);

    // === Додавання елементів в головний макет ===
    mainLayout->addStretch(4);
    mainLayout->addLayout(headerLayout);
    mainLayout->addStretch(4);
    mainLayout->addLayout(selectCheckerColorLayout);
    mainLayout->addStretch(5);
    mainLayout->addLayout(selectDiffLevelLayout);
    mainLayout->addStretch(5);
    mainLayout->addLayout(buttonsLayout);
    mainLayout->addStretch(3);

    gameSettingsPanel->setObjectName("gameSettingsPanel");
    gameSettingsPanel->setLayout(mainLayout);

    QVBoxLayout* verLayout = new QVBoxLayout();
    QHBoxLayout* horLayout = new QHBoxLayout();

    verLayout->addStretch(1);
    verLayout->addWidget(gameSettingsPanel, 5);
    verLayout->addStretch(1);

    horLayout->addStretch(2);
    horLayout->addLayout(verLayout, 6);
    horLayout->addStretch(2);
    gameSettingsContainer->setLayout(horLayout);
}

void GameWidget::onBoardResize(int xOffset, int yOffset, int boardSide)
{
    m_topLeftPlayerSpace->changeSize(xOffset, 0);
    m_topRightPlayerSpace->changeSize(xOffset, 0);
    m_bottomLeftPlayerSpace->changeSize(xOffset, 0);
    m_bottomRightPlayerSpace->changeSize(xOffset, 0);

    m_topPlayerInfoContainer->update();
    m_bottomPlayerInfoContainer->update();
}
