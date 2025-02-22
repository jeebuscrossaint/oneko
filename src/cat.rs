use crate::sprites::{CatState, SpriteManager};
use image::RgbaImage;
use std::time::Duration;

pub struct Cat {
    pub x: f64,
    pub y: f64,
    pub velocity_x: f64,
    pub velocity_y: f64,
    pub target_x: f64,
    pub target_y: f64,
    pub sprites: SpriteManager,
    pub state: CatState,
}

impl Cat {
    pub fn new(x: f64, y: f64) -> Self {
        Cat {
            x,
            y,
            velocity_x: 0.0,
            velocity_y: 0.0,
            target_x: x,
            target_y: y,
            sprites: SpriteManager::new(),
            state: CatState::Idle,
        }
    }

    pub fn update(&mut self, delta_time: Duration) {
        const ACCELERATION: f64 = 800.0; // Pixels per second squared
        const MAX_SPEED: f64 = 400.0; // Maximum pixels per second
        const FRICTION: f64 = 0.8; // Friction coefficient
        const MOVEMENT_THRESHOLD: f64 = 5.0; // Minimum movement to trigger running animation

        let delta_seconds = delta_time.as_secs_f64();

        // Calculate direction to target
        let dx = self.target_x - self.x;
        let dy = self.target_y - self.y;
        let distance = (dx * dx + dy * dy).sqrt();

        if distance > 1.0 {
            // Normalize direction
            let dir_x = dx / distance;
            let dir_y = dy / distance;

            // Apply acceleration in the direction of the target
            self.velocity_x += dir_x * ACCELERATION * delta_seconds;
            self.velocity_y += dir_y * ACCELERATION * delta_seconds;

            // Apply speed limit
            let speed =
                (self.velocity_x * self.velocity_x + self.velocity_y * self.velocity_y).sqrt();
            if speed > MAX_SPEED {
                let scale = MAX_SPEED / speed;
                self.velocity_x *= scale;
                self.velocity_y *= scale;
            }
        } else {
            // Apply friction when near target
            self.velocity_x *= FRICTION;
            self.velocity_y *= FRICTION;
        }

        // Update position
        self.x += self.velocity_x * delta_seconds;
        self.y += self.velocity_y * delta_seconds;

        // Update animation state based on movement
        let speed = (self.velocity_x * self.velocity_x + self.velocity_y * self.velocity_y).sqrt();

        if speed < MOVEMENT_THRESHOLD {
            self.state = CatState::Idle;
        } else {
            // Calculate movement direction angle
            let angle = self.velocity_y.atan2(self.velocity_x);
            const PI: f64 = std::f64::consts::PI;

            // Convert angle to direction state
            // Divide the circle into 8 sectors
            self.state = match angle {
                a if a > -PI / 8.0 && a <= PI / 8.0 => CatState::RunningRight,
                a if a > PI / 8.0 && a <= 3.0 * PI / 8.0 => CatState::RunningDownRight,
                a if a > 3.0 * PI / 8.0 && a <= 5.0 * PI / 8.0 => CatState::RunningDown,
                a if a > 5.0 * PI / 8.0 && a <= 7.0 * PI / 8.0 => CatState::RunningDownLeft,
                a if a > 7.0 * PI / 8.0 || a <= -7.0 * PI / 8.0 => CatState::RunningLeft,
                a if a > -7.0 * PI / 8.0 && a <= -5.0 * PI / 8.0 => CatState::RunningUpLeft,
                a if a > -5.0 * PI / 8.0 && a <= -3.0 * PI / 8.0 => CatState::RunningUp,
                _ => CatState::RunningUpRight,
            };
        }

        // Update sprite animation
        self.sprites.update(delta_seconds);
    }

    pub fn set_target(&mut self, x: f64, y: f64) {
        self.target_x = x;
        self.target_y = y;
    }

    pub fn get_current_frame(&self) -> Option<&RgbaImage> {
        self.sprites.get_current_frame(&self.state)
    }
}
