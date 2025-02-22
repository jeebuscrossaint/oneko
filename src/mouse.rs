use crate::cat::Cat;
use crate::window::Window;
use mouse_position::mouse_position::Mouse;
use std::time::Instant;
use winit::event::Event;
use winit::event_loop::ControlFlow;

#[cfg(target_os = "linux")]
use std::process::Command;

#[cfg(target_os = "linux")]
use winit::event::{Event, WindowEvent};
#[cfg(target_os = "linux")]
use winit::event_loop::ControlFlow;

#[cfg(target_os = "linux")]
fn is_wayland() -> bool {
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
    let (mut window, event_loop) = Window::new();

    // Get the primary monitor and center position
    let monitor = event_loop
        .primary_monitor()
        .expect("Failed to get primary monitor");
    let size = monitor.size();
    let center_x = (size.width / 2) as f64;
    let center_y = (size.height / 2) as f64;

    // Initialize cat at center
    let mut cat = Cat::new(center_x, center_y);
    let mut last_update = Instant::now();

    #[cfg(target_os = "linux")]
    {
        if is_wayland() {
            println!("Running under Wayland");
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
                        window.set_position((cat.x as i32) - 16, (cat.y as i32) - 16);
                    }
                    Event::MainEventsCleared => {
                        cat.update(delta_time);
                        window.request_redraw();
                    }
                    Event::RedrawRequested(_) => {
                        window.render(&cat);
                    }
                    _ => (),
                }
            });
        } else {
            // X11 implementation
            println!("Running under X11");
            event_loop.run(move |event, _, control_flow| {
                *control_flow = ControlFlow::Poll;

                let now = Instant::now();
                let delta_time = now - last_update;
                last_update = now;

                match event {
                    Event::MainEventsCleared => {
                        let position = Mouse::get_mouse_position();
                        match position {
                            Mouse::Position { x, y } => {
                                cat.set_target(x as f64, y as f64);
                            }
                            Mouse::Error => println!("Error getting mouse position"),
                        }

                        cat.update(delta_time);
                        window.request_redraw();
                    }
                    Event::RedrawRequested(_) => {
                        window.render(&cat);
                    }
                    _ => (),
                }
            });
        }
    }

    #[cfg(not(target_os = "linux"))]
    {
        event_loop.run(move |event, _, control_flow| {
            *control_flow = ControlFlow::Poll;

            let now = Instant::now();
            let delta_time = now - last_update;
            last_update = now;

            match event {
                Event::MainEventsCleared => {
                    let position = Mouse::get_mouse_position();
                    match position {
                        Mouse::Position { x, y } => {
                            cat.set_target(x as f64, y as f64);
                            // Move window to follow cat
                            window.set_position(
                                (cat.x as i32) - 16, // Center horizontally
                                (cat.y as i32) - 16, // Center vertically
                            );
                        }
                        Mouse::Error => println!("Error getting mouse position"),
                    }

                    cat.update(delta_time);
                    window.request_redraw();
                }
                Event::RedrawRequested(_) => {
                    window.render(&cat);
                }
                _ => (),
            }
        });
    }
}
