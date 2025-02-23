use crate::cat::Cat;
use crate::window::Window;
use mouse_position::mouse_position::Mouse;
use std::time::Instant;
use winit::event::Event;
use winit::event_loop::ControlFlow;

pub fn track_mouse() {
    let (mut window, event_loop) = Window::new();
    let mut cat = Cat::new(0.0, 0.0);
    let mut last_update = Instant::now();

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
                        window.set_position((cat.x as i32) - 16, (cat.y as i32) - 16);
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
