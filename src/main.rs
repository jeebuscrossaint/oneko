mod cat;
mod mouse;
mod sprites;
mod window;

use image::{GenericImageView, ImageFormat};
use std::sync::Arc;
use tray_icon::{TrayIconBuilder, icon::Icon};

fn create_tray_icon() -> Option<Arc<tray_icon::TrayIcon>> {
    // Load the GIF
    let icon_data = include_bytes!("../gif/1.GIF");
    let img = image::load_from_memory_with_format(icon_data, ImageFormat::Gif)
        .expect("Failed to load icon")
        .to_rgba8();

    // Get dimensions
    let (width, height) = (img.width() as usize, img.height() as usize);

    // Create icon
    let icon = Icon::from_rgba(img.into_raw(), width as u32, height as u32)
        .expect("Failed to create icon");

    // Create tray icon
    let tray_icon = TrayIconBuilder::new()
        .with_tooltip("oneko")
        .with_icon(icon)
        .build()
        .ok()?;

    Some(Arc::new(tray_icon))
}

fn main() {
    println!("oneko!");

    // Create tray icon and keep it alive
    let _tray_icon = create_tray_icon();

    // Start the main application
    mouse::track_mouse();
}
