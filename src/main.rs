mod cat;
mod mouse;
mod sprites;
mod window;

use image::ImageFormat;
use tray_icon::{
    TrayIconBuilder,
    icon::Icon,
    menu::{Menu, MenuItem},
};

fn create_tray_icon() -> Option<tray_icon::TrayIcon> {
    // Create menu
    let menu = Menu::new();
    let quit_item = MenuItem::new("Quit", true, None);
    menu.append(&quit_item);

    // Load the GIF
    let icon_data = include_bytes!("../gif/32.GIF");
    let img = image::load_from_memory_with_format(icon_data, ImageFormat::Gif)
        .expect("Failed to load icon")
        .to_rgba8();

    let (width, height) = (img.width() as usize, img.height() as usize);

    // Create icon
    let icon = Icon::from_rgba(img.into_raw(), width as u32, height as u32)
        .expect("Failed to create icon");

    TrayIconBuilder::new()
        .with_tooltip("oneko")
        .with_menu(Box::new(menu))
        .with_icon(icon)
        .build()
        .ok()
}

fn main() {
    let _tray_icon = create_tray_icon().expect("Failed to create tray icon");
    mouse::track_mouse();
}
