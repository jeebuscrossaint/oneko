use crate::sprites::{CatState, SpriteManager};
use image::RgbaImage;
use std::time::Duration;

pub struct Cat {
    pub x: f64,
    pub y: f64,
    pub target_x: f64,
    pub target_y: f64,
    pub sprites: SpriteManager,
    pub state: CatState,
    idle_time: f64,
}

impl Cat {
    pub fn new(x: f64, y: f64) -> Self {
        Cat {
            x,
            y,
            target_x: x,
            target_y: y,
            sprites: SpriteManager::new(),
            state: CatState::Idle,
            idle_time: 0.0,
        }
    }

    pub fn update(&mut self, delta_time: Duration) {
        const NEKO_SPEED: f64 = 1.0;

        let diff_x = self.target_x - self.x;
        let diff_y = self.target_y - self.y;
        let distance = (diff_x * diff_x + diff_y * diff_y).sqrt();

        // If we're close to the target, go idle
        if distance < NEKO_SPEED || distance < 48.0 {
            self.state = CatState::Idle;
            self.idle_time += delta_time.as_secs_f64();
            return;
        }

        // Reset idle time when moving
        self.idle_time = 0.0;

        // Simple linear movement
        self.x += (diff_x / distance) * NEKO_SPEED;
        self.y += (diff_y / distance) * NEKO_SPEED;

        // Update direction state based on movement
        let dx = diff_x / distance;
        let dy = diff_y / distance;

        self.state = if dy > 0.5 {
            if dx > 0.5 {
                CatState::RunningUpRight
            } else if dx < -0.5 {
                CatState::RunningUpLeft
            } else {
                CatState::RunningUp
            }
        } else if dy < -0.5 {
            if dx > 0.5 {
                CatState::RunningDownRight
            } else if dx < -0.5 {
                CatState::RunningDownLeft
            } else {
                CatState::RunningDown
            }
        } else if dx > 0.5 {
            CatState::RunningRight
        } else if dx < -0.5 {
            CatState::RunningLeft
        } else {
            CatState::Idle
        };

        // Update sprite animation
        self.sprites.update(delta_time.as_secs_f64());
    }

    pub fn set_target(&mut self, x: f64, y: f64) {
        self.target_x = x;
        self.target_y = y;
    }

    pub fn get_current_frame(&self) -> Option<&RgbaImage> {
        self.sprites.get_current_frame(&self.state)
    }
}
