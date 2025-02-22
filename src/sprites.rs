use image::{ImageFormat, RgbaImage};
use std::collections::HashMap;

#[derive(Debug, Eq, PartialEq, Hash, Clone)]
pub enum CatState {
    Idle,
    RunningUp,
    RunningDown,
    RunningLeft,
    RunningRight,
    RunningUpLeft,
    RunningUpRight,
    RunningDownLeft,
    RunningDownRight,
}

pub struct SpriteManager {
    frames: HashMap<CatState, Vec<RgbaImage>>,
    current_frame: usize,
    animation_timer: f64,
}

impl SpriteManager {
    pub fn new() -> Self {
        let mut frames = HashMap::new();

        // Define the state frames using a macro to handle the string concatenation
        macro_rules! load_gif {
            ($path:expr) => {{
                let gif_bytes = include_bytes!(concat!("../gif/", $path));
                image::load_from_memory_with_format(gif_bytes, ImageFormat::Gif)
                    .expect("Failed to load sprite")
                    .to_rgba8()
            }};
        }

        // Load each state's frames
        frames.insert(CatState::Idle, vec![load_gif!("1.GIF"), load_gif!("2.GIF")]);

        frames.insert(
            CatState::RunningUp,
            vec![load_gif!("3.GIF"), load_gif!("4.GIF")],
        );

        frames.insert(
            CatState::RunningUpRight,
            vec![load_gif!("5.GIF"), load_gif!("6.GIF")],
        );

        frames.insert(
            CatState::RunningRight,
            vec![load_gif!("7.GIF"), load_gif!("8.GIF")],
        );

        frames.insert(
            CatState::RunningDownRight,
            vec![load_gif!("9.GIF"), load_gif!("10.GIF")],
        );

        frames.insert(
            CatState::RunningDown,
            vec![load_gif!("11.GIF"), load_gif!("12.GIF")],
        );

        frames.insert(
            CatState::RunningDownLeft,
            vec![load_gif!("13.GIF"), load_gif!("14.GIF")],
        );

        frames.insert(
            CatState::RunningLeft,
            vec![load_gif!("15.GIF"), load_gif!("16.GIF")],
        );

        frames.insert(
            CatState::RunningUpLeft,
            vec![load_gif!("17.GIF"), load_gif!("18.GIF")],
        );

        SpriteManager {
            frames,
            current_frame: 0,
            animation_timer: 0.0,
        }
    }

    pub fn get_current_frame(&self, state: &CatState) -> Option<&RgbaImage> {
        self.frames
            .get(state)
            .and_then(|frames| frames.get(self.current_frame))
    }

    pub fn update(&mut self, delta_time: f64) {
        const FRAME_DURATION: f64 = 0.1; // 200ms per frame

        self.animation_timer += delta_time;
        if self.animation_timer >= FRAME_DURATION {
            self.animation_timer -= FRAME_DURATION;
            self.current_frame = (self.current_frame + 1) % 2;
        }
    }
}
