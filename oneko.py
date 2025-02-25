import sys
import math
import random
from PyQt6.QtWidgets import QApplication, QLabel, QSystemTrayIcon, QMenu
from PyQt6.QtCore import Qt, QTimer, QPoint, QSize
from PyQt6.QtGui import QMovie, QCursor, QIcon
import os


class OnekoWindow(QLabel):
    CHASING = 1
    IDLE = 2
    SURPRISED = 3
    DIGGING = 4

    def __init__(self):
        super().__init__()
        self.initUI()
        self.setupTrayIcon()
        # Start at cursor position
        cursor_pos = QCursor.pos()
        self.move(cursor_pos.x() - self.SPRITE_SIZE // 2,
                  cursor_pos.y() - self.SPRITE_SIZE // 2)

    def initUI(self):
        self.setWindowFlags(
            Qt.WindowType.FramelessWindowHint
            | Qt.WindowType.WindowStaysOnTopHint
            | Qt.WindowType.Tool
            | Qt.WindowType.WindowTransparentForInput
            | Qt.WindowType.SubWindow
            | Qt.WindowType.WindowDoesNotAcceptFocus
        )
        self.setAttribute(Qt.WidgetAttribute.WA_TranslucentBackground)
        self.setAttribute(Qt.WidgetAttribute.WA_TransparentForMouseEvents)

        # Constants
        self.SPRITE_SIZE = 32
        self.TRIGGER_DISTANCE = 64
        self.CATCH_DISTANCE = 24
        self.OFFSET_X = -2
        self.OFFSET_Y = -3
        self.NEKO_SPEED = 12
        self.DIG_TIMEOUT = 50  # ~5 seconds at 90ms intervals

        # State variables
        self.state = self.CHASING
        self.sleep_counter = 0
        self.idle_timer = 0
        self.current_frame = 0
        self.last_update = 0
        self.idle_action_timer = 0
        self.current_idle_action = 'sit'
        self.idle_action_counter = 0
        self.dig_timer = 0
        self.dig_direction = None

        # Load animations
        self.loadAnimations()
        self.setMovie(self.animations[24])
        self.resize(self.SPRITE_SIZE, self.SPRITE_SIZE)

        # Timer for movement and state updates
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.update_state)
        self.timer.start(90)

        self.show()

    def setupTrayIcon(self):
        # Create the tray icon
        self.tray_icon = QSystemTrayIcon(self)

        # Try to use one of the cat GIFs as the tray icon
        icon_path = os.path.join("gif", "1.GIF")
        if os.path.exists(icon_path):
            self.tray_icon.setIcon(QIcon(icon_path))
        else:
            self.tray_icon.setIcon(QIcon.fromTheme("applications-system"))

        # Create the tray menu
        self.tray_menu = QMenu()

        # Add toggle visibility action
        self.toggle_action = self.tray_menu.addAction("Hide Kitty")
        self.toggle_action.triggered.connect(self.toggleVisibility)

        # Add quit action
        quit_action = self.tray_menu.addAction("Quit")
        quit_action.triggered.connect(QApplication.instance().quit)

        # Set the menu for the tray icon
        self.tray_icon.setContextMenu(self.tray_menu)

        # Add tooltip
        self.tray_icon.setToolTip("Oneko")

        # Show the tray icon
        self.tray_icon.show()

    def toggleVisibility(self):
        if self.isVisible():
            self.hide()
            self.toggle_action.setText("Show Kitty")
        else:
            self.show()
            self.toggle_action.setText("Hide Kitty")

    def loadAnimations(self):
        self.animations = {}
        for i in range(1, 33):
            path = os.path.join("gif", f"{i}.GIF")
            if os.path.exists(path):
                movie = QMovie(path)
                movie.setScaledSize(QSize(self.SPRITE_SIZE, self.SPRITE_SIZE))
                movie.start()
                self.animations[i - 1] = movie

    def is_cursor_on_screen(self):
        cursor_pos = QCursor.pos()
        screen = QApplication.primaryScreen().geometry()
        return screen.contains(cursor_pos)

    def get_screen_edge(self):
        cursor_pos = QCursor.pos()
        screen = QApplication.primaryScreen().geometry()

        # Check which edge is closest
        if cursor_pos.y() <= screen.top():
            return "up", (16, 17)  # Animation frames for up digging
        elif cursor_pos.y() >= screen.bottom():
            return "down", (20, 21)  # Animation frames for down digging
        elif cursor_pos.x() <= screen.left():
            return "left", (22, 23)  # Animation frames for left digging
        else:
            return "right", (18, 19)  # Animation frames for right digging

    def update_state(self):
        cursor_pos = QCursor.pos()
        cat_pos = self.pos() + QPoint(self.SPRITE_SIZE // 2, self.SPRITE_SIZE // 2)

        dx = cursor_pos.x() + self.OFFSET_X - cat_pos.x()
        dy = cursor_pos.y() + self.OFFSET_Y - cat_pos.y()
        distance = math.sqrt(dx * dx + dy * dy)

        # Check if cursor is off screen
        if not self.is_cursor_on_screen():
            if self.state != self.DIGGING:
                self.state = self.DIGGING
                self.dig_timer = 0
                self.dig_direction, _ = self.get_screen_edge()
            self.handle_digging()
            return

        if distance < self.CATCH_DISTANCE:
            if self.state != self.IDLE:
                self.state = self.IDLE
                self.idle_timer = 0
                self.current_idle_action = 'sit'
                self.idle_action_counter = 0
                self.setMovie(self.animations[24])
            self.handle_idle_animations()
            return

        if (self.state == self.IDLE or self.state == self.DIGGING) and distance > self.TRIGGER_DISTANCE:
            self.state = self.SURPRISED
            self.setMovie(self.animations[31])
            self.last_update = 5
            return

        if self.state == self.SURPRISED:
            self.last_update -= 1
            if self.last_update <= 0:
                self.state = self.CHASING

        if self.state == self.CHASING:
            if distance > 0:
                angle = math.atan2(dy, dx)
                move_x = round((dx / distance) * self.NEKO_SPEED)
                move_y = round((dy / distance) * self.NEKO_SPEED)

                new_pos = self.pos() + QPoint(move_x, move_y)
                self.move(new_pos)

                degrees = math.degrees(angle)
                self.current_frame = (self.current_frame + 1) % 2
                self.set_direction_animation(degrees)

    def handle_digging(self):
        self.dig_timer += 1
        _, frames = self.get_screen_edge()

        # Animate digging
        self.setMovie(self.animations[frames[self.dig_timer % 2]])

        # Give up after timeout
        if self.dig_timer >= self.DIG_TIMEOUT:
            self.state = self.IDLE
            self.idle_timer = 301  # Force sleep consideration
            self.current_idle_action = 'sleep'
            self.idle_action_counter = 0

    def handle_idle_animations(self):
        self.idle_timer += 1

        # Every ~10 seconds, decide what to do
        if self.idle_timer % 100 == 0:
            if self.current_idle_action == 'sit':
                # 15% chance to start washing
                if random.random() < 0.15:
                    self.current_idle_action = 'wash'
                    self.idle_action_counter = 0
                # Higher chance to sleep if we were just digging
                elif self.idle_timer > 300 and random.random() < (0.5 if self.state == self.DIGGING else 0.1):
                    self.current_idle_action = 'sleep'
                    self.idle_action_counter = 0
            elif self.current_idle_action == 'wash':
                if self.idle_action_counter > 30:
                    self.current_idle_action = 'sit'
                    self.idle_action_counter = 0

        # Handle different idle actions
        if self.current_idle_action == 'sit':
            self.setMovie(self.animations[24])  # Use proper sitting animation
        elif self.current_idle_action == 'wash':
            self.idle_action_counter += 1
            self.setMovie(self.animations[24 + (self.idle_timer // 3 % 2)])
        elif self.current_idle_action == 'sleep':
            self.idle_action_counter += 1
            if self.idle_action_counter < 20:  # Prepare to sleep
                self.setMovie(self.animations[26])
            else:  # Sleep
                self.setMovie(self.animations[28 + (self.idle_timer // 5 % 2)])

    def set_direction_animation(self, degrees):
        if -22.5 <= degrees <= 22.5:  # Right
            self.setMovie(self.animations[4 + self.current_frame])
        elif 22.5 < degrees <= 67.5:  # Down-Right
            self.setMovie(self.animations[6 + self.current_frame])
        elif 67.5 < degrees <= 112.5:  # Down
            self.setMovie(self.animations[8 + self.current_frame])
        elif 112.5 < degrees <= 157.5:  # Down-Left
            self.setMovie(self.animations[10 + self.current_frame])
        elif degrees > 157.5 or degrees <= -157.5:  # Left
            self.setMovie(self.animations[12 + self.current_frame])
        elif -157.5 < degrees <= -112.5:  # Up-Left
            self.setMovie(self.animations[14 + self.current_frame])
        elif -112.5 < degrees <= -67.5:  # Up
            self.setMovie(self.animations[0 + self.current_frame])
        elif -67.5 < degrees <= -22.5:  # Up-Right
            self.setMovie(self.animations[2 + self.current_frame])


if __name__ == "__main__":
    app = QApplication(sys.argv)
    app.setQuitOnLastWindowClosed(False)
    window = OnekoWindow()
    sys.exit(app.exec())
