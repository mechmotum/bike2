import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm
from control import lqr, StateSpace, step_response, initial_response

viridis = cm.get_cmap('viridis', 12)

m = 53.0  # [kg], total mass of bike and reaction wheel (without rider)
g = 9.81  # [m/s/s], acceleration due to gravity
l = 0.7  # [m], mass center of bike and reaction wheel (without rider)
I = 10.0*0.2**2  # [kg*m/s/s], inertia of reaction wheel, m*r**2

# linear dynamics
# M*x' = G*x + H*u
# x = [wb [rad/s], thetab [rad], wm [rad/s], thetam [rad]]
# x = [bike rate, bike angle, motor rate, motor angle]
M = np.array([
    [m*l**2, 0.0, 0.0, 0.0],
    [0.0, 1.0, 0.0, 0.0],
    [I, 0.0, I, 0.0],
    [0.0, 0.0, 0.0, 1.0]
])

G = np.array([
    [0.0, m*g*l, 0.0, 0.0],
    [1.0, 0.0, 0.0, 0.0],
    [0.0, 0.0, 0.0, 0.0],
    [0.0, 0.0, 1.0, 0.0],
])

H = np.array([
    [1.0],
    [0.0],
    [-1.0],
    [0.0],
])

# state space form, full state measurment
# x' = A*x + B*u
# y = C*x + D*u
A = np.linalg.inv(M) @ G
B = np.linalg.inv(M) @ H
C = np.eye(4)
D = np.zeros((4, 1))

open_loop = StateSpace(A, B, C, D)

# solve LQR for full state feedback gains
Q = np.eye(4)
R = np.eye(1)
K, S, E = lqr(open_loop, Q, R)

print("Gains:", K.squeeze())
print("Closed loop eigenvalues:", E.squeeze())

#K[0, 2] = 0.0
#K[0, 3] = 0.0

closed_loop = StateSpace(A - B@K, B, C, np.zeros((4, 1)))

step_res = step_response(closed_loop, squeeze=True)
init_res = initial_response(closed_loop, X0=np.array([0.0, 0.1, 0.0, 0.0]),
                            squeeze=True)
fig, ax = plt.subplots()
ax.set_title('Step Response')
ax.plot(step_res.time, step_res.outputs.T)
ax.legend(['bike_rate', 'bike_angle', 'motor_rate', 'motor_angle'])

fig, ax = plt.subplots()
ax.set_title('Initial Value Response')
ax.plot(init_res.time, init_res.outputs.T)
ax.legend(['bike_rate', 'bike_angle', 'motor_rate', 'motor_angle'])

# root locus for only P control


def calc_A(P):
    # P is the propotional gain

    D = 0.0  # derivative gain

    N = np.array([
        [D, m*g*l - P, 0.0, 0.0],
        [1.0, 0.0, 0.0, 0.0],
        [D, P, 0.0, 0.0],
        [0.0, 0.0, 1.0, 0.0]
    ])

    A = np.linalg.inv(M) @ N

    return A


n = 10000
eigs = np.empty((n, 4), dtype=complex)
for i, Pi in enumerate(np.linspace(0.0, 250.0, num=n)):
    eigs[i] = np.linalg.eig(calc_A(Pi))[0]

fig, ax = plt.subplots()
for i, eigi in enumerate(eigs):
    ax.plot(np.real(eigi), np.imag(eigi), '.', color=viridis(i/n))
ax.set_title('Root locus for P control')

plt.show()
