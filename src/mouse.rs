use crate::cat::Cat;
use std::time::{Duration, Instant};

#[cfg(target_os = "linux")]
use std::process::Command;

#[cfg(target_os = "linux")]
use winit::event::{Event, WindowEvent};
#[cfg(target_os = "linux")]
use winit::event_loop::{ControlFlow, EventLoop};
#[cfg(target_os = "linux")]
use winit::window::WindowBuilder;

use mouse_position::mouse_position::Mouse;

#[cfg(target_os = "linux")]
fn is_wayland() -> bool {
    // Check if we're running under Wayland by checking the XDG_SESSION_TYPE
    if let Ok(output) = Command::new("sh")
        .arg("-c")
        .arg("echo $XDG_SESSION_TYPE")
        .output()
    {
        if let Ok(session_type) = String::from_utf8(output.stdout) {
            return session_type.trim() == "wayland";
        }
    }
    false
}

pub fn track_mouse() {
    let mut cat = Cat::new(0.0, 0.0);
    let mut last_update = Instant::now();

    #[cfg(target_os = "linux")]
    {
        if is_wayland() {
            println!("Running under Wayland");
            let event_loop = EventLoop::new();
            let window = WindowBuilder::new()
                .with_title("oneko")
                .with_inner_size(winit::dpi::LogicalSize::new(1, 1))
                .build(&event_loop)
                .unwrap();

            event_loop.run(move |event, _, control_flow| {
                *control_flow = ControlFlow::Poll;

                let now = Instant::now();
                let delta_time = now - last_update;
                last_update = now;

                match event {
                    Event::WindowEvent {
                        event: WindowEvent::CursorMoved { position, .. },
                        ..
                    } => {
                        cat.set_target(position.x, position.y);
                    }
                    Event::MainEventsCleared => {
                        cat.update(delta_time);
                        println!("Cat position: ({:.1}, {:.1})", cat.x, cat.y);
                        window.request_redraw();
                    }
                    _ => (),
                }
            });
        } else {
            // X11 implementation
            println!("Running under X11");
            loop {
                let now = Instant::now();
                let delta_time = now - last_update;
                last_update = now;

                let position = Mouse::get_mouse_position();
                match position {
                    Mouse::Position { x, y } => {
                        cat.set_target(x as f64, y as f64);
                    }
                    Mouse::Error => println!("Error getting mouse position"),
                }

                cat.update(delta_time);
                println!("Cat position: ({:.1}, {:.1})", cat.x, cat.y);
                std::thread::sleep(Duration::from_millis(16));
            }
        }
    }

    #[cfg(not(target_os = "linux"))]
    {
        loop {
            let now = Instant::now();
            let delta_time = now - last_update;
            last_update = now;

            let position = Mouse::get_mouse_position();
            match position {
                Mouse::Position { x, y } => {
                    cat.set_target(x as f64, y as f64);
                }
                Mouse::Error => println!("Error getting mouse position"),
            }

            cat.update(delta_time);
            println!("Cat position: ({:.1}, {:.1})", cat.x, cat.y);
            std::thread::sleep(Duration::from_millis(16));
        }
    }
}
