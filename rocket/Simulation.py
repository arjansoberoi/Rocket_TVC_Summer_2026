"""
Wind input generator for our TVC model rocket (Dallas, TX).

Generates the wind the rocket sees over time -- not the rocket's response.
Each sample has: wind speed (mph), twist sign (+ CCW / - CW), and moment
arm (m, + toward nose / - toward tail).

MODE picks the test: "step" (clean step response), "discrete" (a few set
gusts), "random" (real-ish profile), or "all" (compare all three).
"""

import random
import numpy as np
import matplotlib.pyplot as plt

MODE = "all"             # "all" | "step" | "discrete" | "random"

# Time + limits
DURATION_S = 30.0        # seconds
DT = 0.05                # sample interval (seconds)
MIN_WIND = 0.0           # mph
MAX_WIND = 10.0          # mph

# Rocket geometry -- only used to place the moment arm
ROCKET_LENGTH_M = 1.0
CG_FROM_NOSE_M = 0.55
TOP_HALF_LEN = CG_FROM_NOSE_M
BOTTOM_HALF_LEN = ROCKET_LENGTH_M - CG_FROM_NOSE_M

# Step test
STEP_TIME = 1.0          # wind turns on (s)
STEP_LEVEL = 6.0         # held wind after the step (mph)
STEP_SIDE = "left"
STEP_HALF = "top"
STEP_ARM_FRAC = 0.8      # 0 = CoM, 1 = tip

# Discrete gusts: (start_s, dur_s, mag_mph, side, half, arm_frac, sharp)
DISCRETE_GUSTS = [
    (2.0,  3.0, 3.0, "left",  "top",    0.5, False),
    (8.0,  3.0, 6.0, "right", "top",    0.8, True),
    (14.0, 3.0, 9.0, "left",  "bottom", 0.6, False),
    (20.0, 3.0, 6.0, "right", "bottom", 0.3, True),
]

# Random mode (per-step values, tuned for DT = 0.05 s)
BASELINE_MEAN = 4.0
BASELINE_WANDER = 0.15
BASELINE_PULL = 0.0125
GUST_MIN_DURATION = 2.0
GUST_MAX_DURATION = 5.0
GUST_MIN_PEAK = 2.0
GUST_MAX_PEAK = 7.0
GUST_CHANCE = 0.015      # per step (~0.3 per second)
GUST_SHARP_CHANCE = 0.40

# Unit conversions
IN_TO_M = 0.0254
MPH_TO_MS = 0.44704


def gust_envelope(n_steps, sharp=False):
    """Rise-and-fall shape for one gust, values in [0, 1]."""
    if n_steps < 2:
        return np.ones(max(n_steps, 1))
    if not sharp:
        return np.hanning(n_steps)
    rise = max(1, int(n_steps * random.uniform(0.05, 0.15)))
    up = np.sin(np.linspace(0, np.pi / 2, rise))
    down = np.cos(np.linspace(0, np.pi / 2, n_steps - rise))
    return np.concatenate([up, down])


def direction_sign(side, half):
    """Twist sign: +1 CCW, -1 CW."""
    return -1 if (side, half) in (("left", "top"), ("right", "bottom")) else +1


def moment_arm(half, frac):
    """Signed distance from the CoM (m): + toward nose, - toward tail."""
    return frac * TOP_HALF_LEN if half == "top" else -frac * BOTTOM_HALF_LEN


def broadside_drag_force(wind_mph, length_in, diameter_in,
                         cd=1.1, air_density=1.225):
    """Broadside drag force in newtons: F = 0.5 * rho * v^2 * Cd * A."""
    v = wind_mph * MPH_TO_MS
    area = (length_in * IN_TO_M) * (diameter_in * IN_TO_M)
    return 0.5 * air_density * v ** 2 * cd * area


def wind_moment(wind_mph, length_in, diameter_in, arm_m,
                cd=1.1, air_density=1.225):
    """Moment about the CoM in newton-metres: drag force times lever arm."""
    force = broadside_drag_force(wind_mph, length_in, diameter_in, cd, air_density)
    return force * arm_m


# Each builder returns (time, wind, sign, arm, log).
def build_step():
    """Zero, then a constant step held for the rest of the run."""
    time = np.arange(0.0, DURATION_S, DT)
    wind = np.where(time >= STEP_TIME, float(STEP_LEVEL), 0.0)
    sign = np.full(len(time), direction_sign(STEP_SIDE, STEP_HALF))
    arm = np.full(len(time), moment_arm(STEP_HALF, STEP_ARM_FRAC))
    log = [(STEP_TIME, DURATION_S - STEP_TIME, STEP_LEVEL,
            STEP_SIDE, STEP_HALF, STEP_ARM_FRAC, False)]
    return time, wind, sign, arm, log


def build_discrete():
    """A handful of set gusts at different magnitudes and moment arms."""
    time = np.arange(0.0, DURATION_S, DT)
    n = len(time)
    wind = np.zeros(n)
    sign = np.ones(n)
    arm = np.zeros(n)
    log = []
    for start, dur, mag, side, half, frac, sharp in DISCRETE_GUSTS:
        i0 = int(start / DT)
        i1 = min(i0 + int(dur / DT), n)
        if i1 <= i0:
            continue
        wind[i0:i1] += gust_envelope(i1 - i0, sharp) * mag
        sign[i0:i1] = direction_sign(side, half)
        arm[i0:i1] = moment_arm(half, frac)
        log.append((start, dur, mag, side, half, frac, sharp))
    return time, np.clip(wind, MIN_WIND, MAX_WIND), sign, arm, log


def build_random():
    """Wandering baseline plus randomized gusts."""
    time = np.arange(0.0, DURATION_S, DT)
    n = len(time)

    baseline = np.empty(n)
    baseline[0] = BASELINE_MEAN
    for i in range(1, n):
        step = random.uniform(-BASELINE_WANDER, BASELINE_WANDER)
        pull = (BASELINE_MEAN - baseline[i - 1]) * BASELINE_PULL
        baseline[i] = baseline[i - 1] + step + pull

    sign = np.full(n, direction_sign(random.choice(("left", "right")),
                                     random.choice(("top", "bottom"))))
    arm = np.full(n, moment_arm(random.choice(("top", "bottom")),
                                random.uniform(0.3, 1.0)))

    gusts = np.zeros(n)
    log = []
    i = 0
    while i < n:
        if random.random() < GUST_CHANCE:
            dur = random.uniform(GUST_MIN_DURATION, GUST_MAX_DURATION)
            steps = min(int(dur / DT), n - i)
            peak = random.uniform(GUST_MIN_PEAK, GUST_MAX_PEAK)
            sharp = random.random() < GUST_SHARP_CHANCE
            side = random.choice(("left", "right"))
            half = random.choice(("top", "bottom"))
            frac = random.uniform(0.3, 1.0)

            gusts[i:i + steps] += gust_envelope(steps, sharp) * peak
            sign[i:i + steps] = direction_sign(side, half)
            arm[i:i + steps] = moment_arm(half, frac)
            log.append((time[i], steps * DT, peak, side, half, frac, sharp))
            i += steps
        else:
            i += 1

    wind = np.clip(baseline + gusts, MIN_WIND, MAX_WIND)
    return time, wind, sign, arm, log


BUILDERS = {
    "step":     (build_step,     "Constant (step)"),
    "discrete": (build_discrete, "A few discrete gusts"),
    "random":   (build_random,   "Randomized"),
}


def plot_results(time, wind, signed_wind):
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(11, 7), sharex=True)

    ax1.plot(time, wind, color="tab:blue", linewidth=1.6, label="Wind speed")
    if MODE == "step":
        ax1.axvline(STEP_TIME, color="orange", linestyle="--", linewidth=1,
                    label=f"Step @ {STEP_TIME:.0f}s")
    ax1.set_title(f"Wind Intensity vs. Time  (mode = {MODE})")
    ax1.set_ylabel("Wind speed (mph)")
    ax1.set_ylim(0, MAX_WIND + 1)
    ax1.grid(True, alpha=0.3)
    ax1.legend(loc="upper right")

    ax2.plot(time, signed_wind, color="black", linewidth=1.0)
    ax2.axhline(0, color="gray", linewidth=1)
    ax2.fill_between(time, signed_wind, 0, where=(signed_wind >= 0),
                     color="tab:green", alpha=0.4, label="CCW (+)")
    ax2.fill_between(time, signed_wind, 0, where=(signed_wind < 0),
                     color="tab:red", alpha=0.4, label="CW (-)")
    ax2.set_title("Wind Intensity + Direction (sign)")
    ax2.set_xlabel("Time (s)")
    ax2.set_ylabel("Wind speed (mph), signed")
    ax2.set_ylim(-(MAX_WIND + 1), MAX_WIND + 1)
    ax2.grid(True, alpha=0.3)
    ax2.legend(loc="upper right")

    plt.tight_layout()
    plt.show()


def plot_all():
    """All three profiles stacked in one figure."""
    fig, axes = plt.subplots(len(BUILDERS), 1, figsize=(11, 9), sharex=True)

    for ax, (name, (builder, label)) in zip(axes, BUILDERS.items()):
        time, wind, sign, arm, log = builder()
        signed = wind * sign
        ax.plot(time, signed, color="black", linewidth=1.0)
        ax.axhline(0, color="gray", linewidth=1)
        ax.fill_between(time, signed, 0, where=(signed >= 0),
                        color="tab:green", alpha=0.4)
        ax.fill_between(time, signed, 0, where=(signed < 0),
                        color="tab:red", alpha=0.4)
        ax.set_title(f"{label}   (mode = {name})")
        ax.set_ylabel("mph, signed")
        ax.set_ylim(-(MAX_WIND + 1), MAX_WIND + 1)
        ax.grid(True, alpha=0.3)

    green = plt.Rectangle((0, 0), 1, 1, color="tab:green", alpha=0.4)
    red = plt.Rectangle((0, 0), 1, 1, color="tab:red", alpha=0.4)
    axes[0].legend([green, red], ["CCW (+)", "CW (-)"], loc="upper right")
    axes[-1].set_xlabel("Time (s)")
    fig.suptitle("Wind Simulation -- all three profiles (Dallas, TX)")
    plt.tight_layout()
    plt.show()


def print_summary(name, wind, log):
    print(f"\n=== {name} ===")
    print(f"Wind -> min {wind.min():.2f} | mean {wind.mean():.2f} | "
          f"max {wind.max():.2f} mph   ({len(log)} event(s))")
    for start, dur, mag, side, half, frac, sharp in log:
        kind = "sharp" if sharp else "smooth"
        spin = "CCW" if direction_sign(side, half) > 0 else "CW"
        print(f"  t={start:5.1f}s  {dur:4.1f}s  {mag:4.1f} mph  {kind:6s}  "
              f"{side:5s}+{half:6s} -> {spin:3s}  arm {moment_arm(half, frac):+.2f} m")


if __name__ == "__main__":
    print(f"MODE = {MODE}  ({int(DURATION_S / DT)} samples, step = {DT} s)")

    if MODE == "all":
        for name, (builder, _) in BUILDERS.items():
            _, wind, _, _, log = builder()
            print_summary(name, wind, log)
        plot_all()
    else:
        builder, _ = BUILDERS[MODE]
        time, wind, sign, arm, log = builder()
        print_summary(MODE, wind, log)
        plot_results(time, wind, wind * sign)
