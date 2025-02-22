use std::time::Duration;

pub struct Cat {
    pub x: f64,
    pub y: f64,
    pub velocity_x: f64,
    pub velocity_y: f64,
    pub target_x: f64,
    pub target_y: f64,
}

impl Cat {
    pub fn new(x: f64, y: f64) -> Self {
        Cat {
            x,
            y,
            velocity_x: 0.0,
            velocity_y: 0.0,
            target_x: 0.0,
            target_y: 0.0,
        }
    }

    pub fn update(&mut self, delta_time: Duration) {
        const ACCELERATION: f64 = 800.0; // pixels per second squared
        const MAX_SPEED: f64 = 400.0; // pixels per second
        const FRICTION: f64 = 0.8; // friction coefficint i think its like mu or something

        let delta_seconds = delta_time.as_secs_f64();

        // calculate the direction to the target
        let dx = self.target_x - self.x;
        let dy = self.target_y - self.y;
        let distance = (dx * dx + dy * dy).sqrt();

        if distance > 1.0 {
            // normalize direction
            let dir_x = dx / distance;
            let dir_y = dy / distance;

            // apply acceleration in the direction of the target
            self.velocity_x += dir_x * ACCELERATION * delta_seconds;
            self.velocity_y += dir_y * ACCELERATION * delta_seconds;

            // apply speed limit (doesnt apply when freebird is playing)
            let speed =
                (self.velocity_x * self.velocity_x + self.velocity_y * self.velocity_y).sqrt();
            if speed > MAX_SPEED {
                let scale = MAX_SPEED / speed;
                self.velocity_x *= scale;
                self.velocity_y *= scale;
            }
        } else {
            // apply friction when near target
            self.velocity_x *= FRICTION;
            self.velocity_y *= FRICTION;
        }

        // update position
        self.x += self.velocity_x * delta_seconds;
        self.y += self.velocity_y * delta_seconds;
    }

    pub fn set_target(&mut self, x: f64, y: f64) {
        self.target_x = x;
        self.target_y = y;
    }
}
