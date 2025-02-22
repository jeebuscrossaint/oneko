mod cat;
mod mouse;
mod sprites;
mod window;

use image::ImageFormat;
use std::sync::Arc;
use tray_icon::{
    TrayIconBuilder,
    icon::Icon,
    menu::{Menu, MenuItem},
};

fn create_tray_icon() -> Option<Arc<tray_icon::TrayIcon>> {
    // Create menu
    let mut menu = Menu::new();
    let quit_item = MenuItem::new("Quit", true, None); // No accelerator needed
    menu.append(&quit_item);

    // Load the GIF
    let icon_data = include_bytes!("../gif/32.GIF");
    let img = image::load_from_memory_with_format(icon_data, ImageFormat::Gif)
        .expect("Failed to load icon")
        .to_rgba8();

    // Get dimensions
    let (width, height) = (img.width() as usize, img.height() as usize);

    // Create icon
    let icon = Icon::from_rgba(img.into_raw(), width as u32, height as u32)
        .expect("Failed to create icon");

    // Create tray icon with menu
    let tray_icon = TrayIconBuilder::new()
        .with_tooltip("oneko")
        .with_menu(Box::new(menu))
        .with_icon(icon)
        .build()
        .ok()?;

    // Set up menu event listener
    let tray_icon = Arc::new(tray_icon);
    let tray_clone = tray_icon.clone();

    // Handle menu events in a separate thread
    std::thread::spawn(move || {
        let menu_channel = tray_icon::menu::MenuEvent::receiver();
        while let Ok(event) = menu_channel.recv() {
            // Since we only have one menu item, any event means quit
            std::process::exit(0);
        }
    });

    Some(tray_clone)
}

fn main() {
    // Create tray icon
    let _tray_icon = create_tray_icon().expect("Failed to create tray icon");

    // Start the main application
    mouse::track_mouse();
}
