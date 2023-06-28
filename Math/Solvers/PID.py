class PID(object):
    def __init__(self, p_gain=1, i_gain=0, d_gain=0):
        self.p_gain = p_gain
        self.i_gain = i_gain
        self.d_gain = d_gain
        self.last_error = None
        self.i_error = 0

    def __call__(self, dt, error):
        self.i_error += error * dt
        if self.last_error is None:
            d_error = 0
        else:
            d_error = (error - self.last_error) / dt
        self.last_error = error
        return (error * self.p_gain +
            self.i_error * self.i_gain +
            d_error * self.d_gain)


class SimulatedMass(object):
    def __init__(self, mass=1, friction=0):
        self.mass = mass
        self.friction = friction
        self.state = (0, 0, 0)
        self._force = list()

    def force(self, force):
        self._force.append(force)

    def tick(self, dt):
        self.force(-self.velocity() * self.friction)
        acceleration = sum(self._force) / self.mass
        self._force = []
        velocity = self.velocity() + acceleration * dt
        position = self.position() + (self.velocity() + velocity) * 0.5 * dt
        self.state = (position, velocity, acceleration)

    def position(self):
        return self.state[0]

    def velocity(self):
        return self.state[1]

    def acceleration(self):
        return self.state[2]

    def _compute_force_for_velocity(self, desired, dt):
        delta_v = desired - self.velocity()
        acceleration = delta_v/dt
        return self.mass * acceleration

    def control(self, amount):
        #self.force(self._compute_force_for_velocity(u, dt))
        self.force(amount)

import matplotlib.pyplot as plt
import numpy as np



mass = SimulatedMass(1.0, 0.1)
pid = PID(0.5, 0.01, 1)
dt = 0.01
target = 1
states = list()
for step in range(1000):
    states.append(mass.state)
    u = pid(dt, target - mass.position())
    mass.control(u)
    mass.tick(dt)

print(states)
print(f"Total error {sum(abs(target - s[0]) for s in states)} Last error: {target-states[-1][0]} Maximum velocity: {max(s[1] for s in states)} total force: {sum(abs(s[2]) * mass.mass for s in states)}.")

# Data for plotting
t = np.arange(0.0, dt*len(states), dt)
p  = np.array(list(s[0] for s in states))
v = np.array(list(s[1] for s in states))
a = np.array(list(s[2] for s in states))
fig, ax = plt.subplots()
ax.plot(t, p)
ax.plot(t, v)
# ax.plot(t, a)

ax.set(xlabel='time (s)', ylabel='Position (m)',
       title='Position')
ax.grid()

fig.savefig("test.png")
plt.show()

