use crate::cat::Cat;
use pixels::{Pixels, SurfaceTexture};
use winit::dpi::{LogicalSize, PhysicalPosition};
use winit::event_loop::EventLoop;
#[cfg(target_os = "windows")]
use winit::platform::windows::WindowBuilderExtWindows;
use winit::window::{WindowBuilder, WindowLevel};

pub struct Window {
    window: winit::window::Window,
    pixels: Pixels,
}

impl Window {
    pub fn new() -> (Self, EventLoop<()>) {
        let event_loop = EventLoop::new();

        let monitor = event_loop
            .primary_monitor()
            .expect("Failed to get primary monitor");

        let size = monitor.size();
        let center = PhysicalPosition::new((size.width / 2) as i32, (size.height / 2) as i32);

        let mut builder = WindowBuilder::new()
            .with_decorations(false)
            .with_transparent(true)
            .with_window_level(WindowLevel::AlwaysOnTop)
            .with_inner_size(LogicalSize::new(32, 32))
            .with_position(center);

        // Add Windows-specific functionality
        #[cfg(target_os = "windows")]
        {
            builder = builder.with_skip_taskbar(true);
        }

        let window = builder.build(&event_loop).unwrap();

        let size = window.inner_size();
        let surface_texture = SurfaceTexture::new(size.width, size.height, &window);
        let pixels = Pixels::new(32, 32, surface_texture).unwrap();

        (Self { window, pixels }, event_loop)
    }

    pub fn render(&mut self, cat: &Cat) {
        let frame = self.pixels.frame_mut();
        // Clear frame with transparent background
        frame.fill(0);

        if let Some(sprite) = cat.get_current_frame() {
            // Copy sprite data to frame buffer
            for (i, pixel) in sprite.pixels().enumerate() {
                frame[i * 4..(i + 1) * 4].copy_from_slice(&pixel.0);
            }
        }

        self.pixels.render().unwrap();
    }

    pub fn request_redraw(&self) {
        self.window.request_redraw();
    }

    pub fn set_position(&self, x: i32, y: i32) {
        self.window
            .set_outer_position(winit::dpi::PhysicalPosition::new(x, y));
    }
}
