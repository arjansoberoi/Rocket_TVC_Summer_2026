"""
Simulation.py
--------------
Very basic wind input generator for our TVC model rocket (Dallas, TX).

This makes the WIND the rocket sees over time. It does NOT model the
rocket's response (no forces, no drag) -- that's the TVC model's job.
Each sample carries three things:

  intensity   -> wind speed in mph (0 to 10, realistic for Dallas)
  direction   -> which of 4 spots it hits: left/right x top/bottom,
                 giving a twist sign (+ CCW / - CW)
  moment arm  -> how far from the center of mass it lands (m),
                 signed + toward the nose, - toward the tail

Pick a MODE:
  "step"     Hold wind at 0, then at t = STEP_TIME slam it to a constant
             and hold it. The single most useful test -- feed it to the
             model and watch the angle deflect, overshoot, and settle.
  "discrete" A few hand-picked gusts at different magnitudes and moment
             arms, to check the response scales sanely and that
             saturation / anti-windup behave.
  "random"   Randomized gusts and directions (a rough real-ish profile).
"""

import random
import numpy as np
import matplotlib.pyplot as plt

# ----------------------------------------------------------------------
# Which test to run
# ----------------------------------------------------------------------
MODE = "all"             # "all" | "step" | "discrete" | "random"

# ----------------------------------------------------------------------
# Time + limits
# ----------------------------------------------------------------------
DURATION_S = 30.0        # length of the window we simulate (seconds)
DT = 0.2                 # time step / array interval (seconds)
MIN_WIND = 0.0           # hard floor on wind speed (mph)
MAX_WIND = 10.0          # hard ceiling on wind speed (mph)

# ----------------------------------------------------------------------
# Rocket geometry -- only used to place the moment arm (a distance)
# ----------------------------------------------------------------------
ROCKET_LENGTH_M = 1.0                            # total length (m)
CG_FROM_NOSE_M = 0.55                            # center of mass from nose (m)
TOP_HALF_LEN = CG_FROM_NOSE_M                    # CoM -> nose
BOTTOM_HALF_LEN = ROCKET_LENGTH_M - CG_FROM_NOSE_M  # CoM -> tail

# ----------------------------------------------------------------------
# STEP test settings
# ----------------------------------------------------------------------
STEP_TIME = 1.0          # when the wind turns on (seconds)
STEP_LEVEL = 6.0         # constant wind held after the step (mph)
STEP_SIDE = "left"       # left / right
STEP_HALF = "top"        # top / bottom (above / below CoM)
STEP_ARM_FRAC = 0.8      # where in that half it lands (0 = CoM, 1 = tip)

# ----------------------------------------------------------------------
# DISCRETE test gusts:  (start_s, dur_s, mag_mph, side, half, arm_frac, sharp)
# Different magnitudes and moment arms so we can check scaling.
# ----------------------------------------------------------------------
DISCRETE_GUSTS = [
    (2.0,  3.0, 3.0, "left",  "top",    0.5, False),
    (8.0,  3.0, 6.0, "right", "top",    0.8, True),
    (14.0, 3.0, 9.0, "left",  "bottom", 0.6, False),
    (20.0, 3.0, 6.0, "right", "bottom", 0.3, True),
]

# ----------------------------------------------------------------------
# RANDOM mode settings
# ----------------------------------------------------------------------
BASELINE_MEAN = 4.0
BASELINE_WANDER = 0.30
GUST_MIN_DURATION = 2.0
GUST_MAX_DURATION = 5.0
GUST_MIN_PEAK = 2.0
GUST_MAX_PEAK = 7.0
GUST_CHANCE = 0.06
GUST_SHARP_CHANCE = 0.40


# ----------------------------------------------------------------------
# Shared helpers
# ----------------------------------------------------------------------
def gust_envelope(n_steps, sharp=False):
    """Rise-and-fall shape for one gust, values in [0, 1]."""
    if n_steps < 2:
        return np.ones(max(n_steps, 1))
    if not sharp:
        return np.hanning(n_steps)          # smooth, symmetric swell
    rise_steps = max(1, int(n_steps * random.uniform(0.05, 0.15)))
    fall_steps = n_steps - rise_steps
    rise = np.sin(np.linspace(0, np.pi / 2, rise_steps))   # fast up
    fall = np.cos(np.linspace(0, np.pi / 2, fall_steps))   # slow down
    return np.concatenate([rise, fall])


def direction_sign(side, half):
    """Twist sign: +1 CCW, -1 CW. (left+top / right+bottom are CW.)"""
    return -1 if (side, half) in (("left", "top"), ("right", "bottom")) else +1


def moment_arm(half, frac):
    """Signed distance from the CoM (m): + toward nose, - toward tail."""
    if half == "top":
        return +frac * TOP_HALF_LEN
    return -frac * BOTTOM_HALF_LEN


# ----------------------------------------------------------------------
# The three input builders -- each returns (time, wind, sign, arm, log)
# ----------------------------------------------------------------------
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
        steps = i1 - i0
        if steps <= 0:
            continue
        wind[i0:i1] += gust_envelope(steps, sharp) * mag
        sign[i0:i1] = direction_sign(side, half)
        arm[i0:i1] = moment_arm(half, frac)
        log.append((start, dur, mag, side, half, frac, sharp))
    wind = np.clip(wind, MIN_WIND, MAX_WIND)
    return time, wind, sign, arm, log


def build_random():
    """Randomized gusts and directions -- a rough real-ish profile."""
    time = np.arange(0.0, DURATION_S, DT)
    n = len(time)

    baseline = np.empty(n)
    baseline[0] = BASELINE_MEAN
    for i in range(1, n):
        step = random.uniform(-BASELINE_WANDER, BASELINE_WANDER)
        pull = (BASELINE_MEAN - baseline[i - 1]) * 0.05
        baseline[i] = baseline[i - 1] + step + pull

    b_side = random.choice(("left", "right"))
    b_half = random.choice(("top", "bottom"))
    sign = np.full(n, direction_sign(b_side, b_half))
    arm = np.full(n, moment_arm(b_half, random.uniform(0.3, 1.0)))

    gusts = np.zeros(n)
    log = []
    i = 0
    while i < n:
        if random.random() < GUST_CHANCE:
            dur = random.uniform(GUST_MIN_DURATION, GUST_MAX_DURATION)
            steps = min(int(dur / DT), n - i)
            peak = random.uniform(GUST_MIN_PEAK, GUST_MAX_PEAK)
            sharp = random.random() < GUST_SHARP_CHANCE
            gusts[i:i + steps] += gust_envelope(steps, sharp) * peak

            g_side = random.choice(("left", "right"))
            g_half = random.choice(("top", "bottom"))
            g_frac = random.uniform(0.3, 1.0)
            sign[i:i + steps] = direction_sign(g_side, g_half)
            arm[i:i + steps] = moment_arm(g_half, g_frac)

            log.append((time[i], steps * DT, peak, g_side, g_half, g_frac, sharp))
            i += steps
        else:
            i += 1

    wind = np.clip(baseline + gusts, MIN_WIND, MAX_WIND)
    return time, wind, sign, arm, log


# Name -> builder, and a friendly label for plot titles.
BUILDERS = {
    "step":     (build_step,     "Constant (step)"),
    "discrete": (build_discrete, "A few discrete gusts"),
    "random":   (build_random,   "Randomized (original)"),
}


def simulate_wind():
    if MODE == "step":
        return build_step()
    if MODE == "discrete":
        return build_discrete()
    return build_random()


# ----------------------------------------------------------------------
# Plot
# ----------------------------------------------------------------------
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
    """All three profiles stacked in one figure for comparison."""
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

    # one shared legend for the whole figure
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
        a = moment_arm(half, frac)
        print(f"  t={start:5.1f}s  {dur:4.1f}s  {mag:4.1f} mph  {kind:6s}  "
              f"{side:5s}+{half:6s} -> {spin:3s}  arm {a:+.2f} m")


if __name__ == "__main__":
    print(f"MODE = {MODE}  ({len(np.arange(0, DURATION_S, DT))} samples, "
          f"step = {DT} s)")

    if MODE == "all":
        for name, (builder, _label) in BUILDERS.items():
            _t, wind, _s, _a, log = builder()
            print_summary(name, wind, log)
        plot_all()
    else:
        time, wind, sign, arm, log = simulate_wind()
        print_summary(MODE, wind, log)
        plot_results(time, wind, wind * sign)
