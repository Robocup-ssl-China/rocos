import time
import matplotlib.pyplot as plt
import matplotx
import numpy as np
import casadi as ca



def test1():
    T = 0.06
    N = 80
    acc_max = 4 # m/s^2
    vel_max = 3 # m/s
    # s_max = 5 # m

    opti = ca.Opti()
    acc = opti.variable(N, 2)

    opt_state = opti.variable(N+1, 2)
    v = opt_state[:, 0]
    s = opt_state[:, 1]

    opt_x0 = opti.parameter(2)
    opt_xs = opti.parameter(2)

    opti.subject_to(opt_state[0, :] == opt_x0.T)
    # opti.subject_to(opt_state[-1, :] == opt_xs.T)

    for i in range(N):
        v_next = opt_state[i,0]+acc[i,0]*T
        s_next = opt_state[i,1]+v[i]*T+0.5*acc[i,0]*T**2
        opti.subject_to(opt_state[i+1,0] == v_next)
        opti.subject_to(opt_state[i+1,1] == s_next)

    opti.subject_to(opti.bounded(-acc_max,acc,acc_max))
    opti.subject_to(opti.bounded(-vel_max,v,vel_max))
    # opti.subject_to(opti.bounded(0,s,s_max))

    obj = 0
    for i in range(N):
        obj = obj + i*(s[i]-opt_xs[1])**2

    opti.minimize(obj)

    opts_setting = {'ipopt.max_iter':1000, 'ipopt.print_level':0, 'print_time':0, 'ipopt.acceptable_tol':1e-2, 'ipopt.acceptable_obj_change_tol':1e-2}

    opti.solver('ipopt', opts_setting)

    _end = 10.0
    start_time = time.time()

    opti.set_value(opt_x0, np.array([0.0,0.0]))
    opti.set_value(opt_xs, np.array([0.0,_end]))
    opti.set_initial(acc, 0)
    opti.set_initial(opt_state, 0)
    sol = opti.solve()

    print(f"Time: {time.time()-start_time}")
    
    a_sol = sol.value(acc)
    v_sol = sol.value(v)
    s_sol = sol.value(s)

    
    plt.plot(v_sol, 'r.-',label='vel')
    plt.plot(s_sol, 'b.-',label='dist')
    plt.plot(a_sol[:,0], 'g.-', label='acc0')
    # plt.plot(a_sol[:,1], label='acc1')
    plt.plot([0,N],[_end,_end],'k-',label='target')
    plt.plot([0,N],[0,0],'k-',label='zero')
    matplotx.line_labels()
    plt.show()

if __name__ == "__main__":
    test1()