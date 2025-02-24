#pragma once
#include <QWidget>
#include <QLabel>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QMovie>
#include <QTimer>

class Oneko : public QWidget {
    Q_OBJECT

private:
    static constexpr int TRIGGER_DISTANCE = 64;
    static constexpr int CATCH_DISTANCE = 16;
    static constexpr int RUN_DISTANCE = 16;
    static constexpr int OFFSET_X = -2;
    static constexpr int OFFSET_Y = -3;

    QLabel* catLabel;
    QSystemTrayIcon* trayIcon;
    QMenu* trayMenu;
    QMovie* animation;
    QTimer* moveTimer;
    
    int ox, oy;
    bool mouseMoved;
    int currentFrame;

    // Add the missing function declaration
    void createTrayIcon();

private slots:
    void updateCatPosition();
    void trayIconClicked(QSystemTrayIcon::ActivationReason reason);

public:
    explicit Oneko(QWidget* parent = nullptr);
    ~Oneko();
};