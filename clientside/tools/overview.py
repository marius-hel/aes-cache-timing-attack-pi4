""" Simple script to plot how bytes affect timings """
import argparse
import numpy as np
import matplotlib.pyplot as plt

def main(arg):
    """ Main function """
    plaintextbyte = list(range(256))
    timings = [[] for _ in range(16)]
    n_samples = [0 for _ in range(16)]

    with open(arg.file, mode='r', encoding='utf-8') as file:
        lines = file.readlines()
        for line in lines:
            splitted = line.split()
            timings[int(splitted[0])].append(float(splitted[4]))
            n_samples[int(splitted[0])] = (int(splitted[3]))

    pow_ind = int(np.round(np.log2(n_samples[0])))
    fig, axes = plt.subplots(4, 4, figsize=(12, 8), sharex=True, sharey=True)
    fig.suptitle(f"Average cycle count for 2^{pow_ind} packets", fontsize=15)

    for i in range(4):
        for j in range(4):
            k = 4*i + j
            axes[i,j].scatter(plaintextbyte, timings[k], marker='.')
            axes[i,j].set_title(f"cycles=f(n[{str(k)}])")
            axes[i,j].grid()

    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Plot average cycle timings from <file>')
    parser.add_argument('file')
    args = parser.parse_args()
    main(args)
