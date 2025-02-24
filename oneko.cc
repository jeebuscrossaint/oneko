#include "oneko.hh"
#include <QScreen>
#include <QApplication>
#include <QTimer>
#include <cmath>

void Oneko::createTrayIcon() {
        trayMenu = new QMenu(this);
        trayMenu->addAction("Exit", qApp, &QApplication::quit);
        
        trayIcon = new QSystemTrayIcon(this);
        trayIcon->setContextMenu(trayMenu);
        trayIcon->setIcon(QIcon("gif/1.GIF"));
        trayIcon->show();
        
        connect(trayIcon, &QSystemTrayIcon::activated, this, &Oneko::trayIconClicked);
    }

Oneko::Oneko(QWidget* parent) : QWidget(parent) {
        // Setup window flags for transparency
        setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
        setAttribute(Qt::WA_TranslucentBackground);
        
        // Initialize position
        ox = oy = 0;
        mouseMoved = false;
        currentFrame = 0;
        
        // Create label for cat animation
        catLabel = new QLabel(this);
        animation = new QMovie("gif/1.GIF");
        catLabel->setMovie(animation);
        animation->start();
        
        // Size window to match GIF
        resize(32, 32);
        
        // Create tray icon
        createTrayIcon();
        
        // Setup animation timer
        moveTimer = new QTimer(this);
        connect(moveTimer, &QTimer::timeout, this, &Oneko::updateCatPosition);
        moveTimer->start(100); // 10fps
    }
    
    void Oneko::updateCatPosition() {
        QPoint mousePos = QCursor::pos();
        
        // Calculate distance to mouse cursor
        int dx = mousePos.x() - (ox + OFFSET_X);
        int dy = mousePos.y() - (oy + OFFSET_Y);
        double dist = std::sqrt(dx*dx + dy*dy);
        
        // Move cat if mouse is far enough
        if (dist > TRIGGER_DISTANCE) {
            mouseMoved = true;
            
            if (dist > RUN_DISTANCE) {
                double scale = RUN_DISTANCE / dist;
                ox += static_cast<int>(dx * scale);
                oy += static_cast<int>(dy * scale);
            } else {
                ox = mousePos.x() - OFFSET_X;
                oy = mousePos.y() - OFFSET_Y;
            }
            
            // Move window to new position
            move(ox, oy);
            
            // Update animation frame
            QString frameFile = QString("gif/%1.GIF").arg(currentFrame + 1);
            animation->stop();
            animation->setFileName(frameFile);
            animation->start();
            
            // Cycle through frames
            currentFrame = (currentFrame + 1) % 32;
        }
    }

void Oneko::trayIconClicked(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::Context) {
        trayMenu->popup(QCursor::pos());
    }
}

Oneko::~Oneko() {
    delete animation;
}