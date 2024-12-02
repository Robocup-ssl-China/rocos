import casadi as ca
import numpy as np

class RobotConfig:
    max_speed = 3.4 # m/s
    mass = 2.2 # kg
    degree1 = 45
    degree2 = 45
    wheels_degree = np.array([degree1, 180-degree2, 180+degree2, 360-degree1])

class WheelConfig:
    power = 30 # w
    radius = 58/2.0 # mm
    gear_ratio = 3.18
    stall_torque = 0.15 # Nm
    nomimal_coef = 280 # rpm * Nm
    degree = 45  # degree

def wheelGetAccLimit(speed, degree: float):
    max_rpm = RobotConfig.max_speed * np.sin(degree/180*np.pi) * WheelConfig.gear_ratio * 1000 / WheelConfig.radius / (2*np.pi) * 60
    friction_torque = WheelConfig.nomimal_coef / max_rpm
    friction_ratio = friction_torque / max_rpm
    # from m/s -> rpm
    rpm = speed * WheelConfig.gear_ratio * 1000 / WheelConfig.radius / (2*np.pi) * 60

    rpm_i1 = ca.if_else(ca.fabs(rpm) < 1e-3, 1e-3, 1/rpm)
    # from rpm -> Nm
    torque = ca.fmin(ca.fmax(-WheelConfig.stall_torque, WheelConfig.nomimal_coef * rpm_i1), WheelConfig.stall_torque)
    # from Nm -> force (N)
    friction_torque = -friction_ratio * rpm
    mm_torque = ca.horzcat(torque, -torque)
    mm_friction_torque = ca.horzcat(friction_torque, friction_torque)
    force = (mm_friction_torque + mm_torque) / (WheelConfig.radius / 1000) * WheelConfig.gear_ratio
    return ca.vertcat(ca.mmin(force), ca.mmax(force)) / RobotConfig.mass

def robotGetAccLimit(v:ca.MX) -> ca.MX:
    wheels = RobotConfig.wheels_degree
    wheel_matrix = np.array([[-np.sin(np.deg2rad(w)), np.cos(np.deg2rad(w))] for w in wheels])
    wheel_ratio = np.average([-np.sin(np.deg2rad(wheels[-1])),np.cos(np.deg2rad(wheels[-1]))])
    simple_mx = wheel_ratio * 2 * np.array([[-1,1],[-1,-1]])
    simple_mx_inv = np.linalg.inv(simple_mx)

    speed_wheel = ca.mtimes(v.T,wheel_matrix.T)
    acc_wheel = ca.horzcat(*[wheelGetAccLimit(speed_wheel[i],w) for i,w in enumerate(wheels)]).T
    acc_limit = acc_wheel[:2,:] * 2
    acc_limit0 = ca.fmin(acc_limit[:,0],-1e-4)
    acc_limit1 = ca.fmax(acc_limit[:,1],1e-4)
    acc_limit_clip = ca.horzcat(acc_limit0,acc_limit1)
    print("acc_limit ", acc_limit_clip, acc_limit_clip.shape)
    return acc_limit_clip

if __name__ == "__main__":
    import time
    import matplotlib.pyplot as plt
    import matplotx
    def testWheel():
        opti = ca.Opti()

        T = opti.variable(1)
        N = 10
        dt = T/N

        acc = opti.variable(N)
        opt_state = opti.variable(N+1,3)
        vel = opt_state[:,0]

        vel0 = opti.parameter()
        vel1 = opti.parameter()

        opti.subject_to(opti.bounded(0, T, 10))
        opti.subject_to(opt_state[0] == vel0)
        for i in range(N):
            current_acc = wheelGetAccLimit(vel[i], RobotConfig.degree1)
            vel_next = vel[i]+acc[i]*dt
            print("vel_next : ",vel_next, vel_next.shape)
            opti.subject_to(vel[i+1] == vel_next)
            opti.subject_to(opti.bounded(current_acc[0],acc[i],current_acc[1]))
            opti.subject_to(opt_state[i,1] == current_acc[0])
            opti.subject_to(opt_state[i,2] == current_acc[1])

        opti.subject_to(opti.bounded(-RobotConfig.max_speed,vel,RobotConfig.max_speed))

        obj = 0
        for i in range(N):
            obj = obj + (vel[i+1]-vel1)**2 + 0.01*acc[i]**2
        obj = obj + T
        
        print("obj : ",obj)

        opti.minimize(obj)

        opts_setting = {'ipopt.max_iter':1000, 'ipopt.print_level':0, 'print_time':0, 'ipopt.acceptable_tol':1e-2, 'ipopt.acceptable_obj_change_tol':1e-2}

        opti.solver('ipopt', opts_setting)

        _end = 3.0

        start_time = time.time()

        opti.set_value(vel0, 0.0)
        opti.set_value(vel1, _end)
        opti.set_initial(acc, 0)
        opti.set_initial(opt_state, 0.0)

        # print all parameters

        sol = opti.solve()

        print(f"Time: {time.time()-start_time}")

        acc_sol = sol.value(acc)
        vel_sol = sol.value(vel)
        acc_min = sol.value(opt_state[:-1,1])
        acc_max = sol.value(opt_state[:-1,2])

        plt.plot(vel_sol, label="velocity")
        plt.plot(acc_sol, label="acceleration")
        plt.plot(acc_min, label="min")
        plt.plot(acc_max, label="max")
        matplotx.line_labels()
        plt.show()

    def testWheelFunc():
        a = np.arange(-3,3,0.1)
        acc_limit = []
        for acc in a:
            _limit = wheelGetAccLimit(acc,45)
            acc_limit.append(_limit)
        acc_limit = np.array(acc_limit).reshape(-1,2)
        print(acc_limit[:,0])
        print(acc_limit[:,1])
        plt.plot(a, acc_limit[:,0], label="min")
        plt.plot(a, acc_limit[:,1], label="max")
        plt.legend()
        plt.show()

    # testWheelFunc()
    testWheel()
        
    def testRobot():
        T = 0.1
        N = 20

        opti = ca.Opti()
        # vx, vy, w
        opt_controls = opti.variable(N, 3)
        vx = opt_controls[:,0]
        vy = opt_controls[:,1]
        w = opt_controls[:,2]
        opt_states = opti.variable(N+1, 3)
        x = opt_states[:,0]
        y = opt_states[:,1]
        theta = opt_states[:,2]

        opt_x0 = opti.parameter(6)
        opt_xf = opti.parameter(6)

        # def f()
        

    def testRobotFunc():
        v = np.mgrid[-0.1:0.2:0.1,-0.1:0.2:0.1].reshape(2,-1).T
        for vel in v:
            acc = robotGetAccLimit(vel.reshape(2,1))
    
    testRobotFunc()