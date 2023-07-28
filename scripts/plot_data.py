"""
Use as::

    python plot_data /path/to/csv/file.csv

Dependencies:

    - python
    - matplotlib
    - numpy
    - pandas

"""
import argparse

import matplotlib.pyplot as plt
plt.switch_backend('WebAgg')

import numpy as np
import pandas as pd

def plot(csvfile):
    """Returns a matplotlib axes with data in ``csvfile`` plotted.

    The file should have column headers:

    time : [s]
    ay : body fixed lateral accelerometer component [m/s/s]
    az : body fixed vertical accelerometer component [m/s/s]
    wx : body fixed roll angular rate [rad/s]
    wm : reaction wheel angular rate [rad/s]
    theta_a : angle from accelerometer [rad]
    theta_g : angle from gyroscope [rad]
    theta_af : low pass filtered accelerometer angle [rad]
    theta_gf : band pass filtered gyroscope angle [rad]
    theta : complementary filtered roll angle estimate [rad]

    """

    df = pd.read_csv(csvfile, index_col='time', sep=";")

    fig, axes = plt.subplots(6, 1, sharex=True)

    df[['ay', 'az']].plot(ax=axes[0])

    df['wx'].apply(np.rad2deg).plot(ax=axes[1])

    df[['theta_a', 'theta_af']].apply(np.rad2deg).plot(ax=axes[2])

    df[['theta_g', 'theta_gf']].apply(np.rad2deg).plot(ax=axes[3])

    df[['theta']].apply(np.rad2deg).plot(ax=axes[4])

    df[['wm']].apply(lambda radps: radps*60.0/2.0/np.pi).plot(ax=axes[5])

    return axes


if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument('csvfile')
    args = parser.parse_args()

    plot(args.csvfile)
    plt.show()
