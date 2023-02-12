""" Simple script to merge stats from two studies or two attacks """
import argparse
import numpy as np

def get_data_from_file(filename):
    tnum=[[0 for _ in range(256)] for _ in range(16)]
    u=[[0 for _ in range(256)] for _ in range(16)]
    udev=[[0 for _ in range(256)] for _ in range(16)]
    size=0

    with open(filename, mode='r', encoding='utf-8') as file:
        lines = file.readlines()
        for line in lines:
            splitted = line.split()
            j = int(splitted[0])
            size = int(splitted[1])
            b = int(splitted[2])
            tnum[j][b] = int(splitted[3])
            u[j][b] = float(splitted[4])
            udev[j][b] = float(splitted[5])

    return (size, tnum, u, udev)

def main(arg):
    """ Main function """
    (size1, tnum1, u1, udev1) = get_data_from_file(arg.file1)
    (size2, tnum2, u2, udev2) = get_data_from_file(arg.file2)

    assert size1==size2, "Size of packets of the two datasets should be equal"

    ttotal = 0
    tnumtotal = 0
    t = [[0 for _ in range(256)] for _ in range(16)]
    tnum = [[0 for _ in range(256)] for _ in range(16)]
    tsq = [[0 for _ in range(256)] for _ in range(16)]
    u = [[0 for _ in range(256)] for _ in range(16)]
    udev = [[0 for _ in range(256)] for _ in range(16)]

    for j in range(16):
        for b in range(256):
            udev1[j][b] *= udev1[j][b]
            udev1[j][b] += u1[j][b] * u1[j][b]
            tsq[j][b] += tnum1[j][b] * udev1[j][b]
            t[j][b] += u1[j][b] * tnum1[j][b]
            tnum[j][b] += tnum1[j][b]

            udev2[j][b] *= udev2[j][b]
            udev2[j][b] += u2[j][b] * u2[j][b]
            tsq[j][b] += tnum2[j][b] * udev2[j][b]
            t[j][b] += u2[j][b] * tnum2[j][b]
            tnum[j][b] += tnum2[j][b]

            ttotal += t[j][b]
            tnumtotal += tnum[j][b]

            u[j][b] = t[j][b] / tnum[j][b]
            udev[j][b] = tsq[j][b] / tnum[j][b]
            udev[j][b] -= u[j][b] * u[j][b]
            udev[j][b] = np.sqrt(udev[j][b])

    taverage = ttotal / tnumtotal
    with open(arg.outputfile, mode='w', encoding='utf-8') as file:
        for j in range(16):
            for b in range(256):
                file.write(f"{j:2} {size1:4} {b:3} {tnum[j][b]} {u[j][b]:.3f} {udev[j][b]:.3f} {u[j][b] - taverage:.6f} {udev[j][b] / np.sqrt(tnum[j][b]):.6f}\n")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Merge two study/attack files <file1> and <file2> into <outputfile>')
    parser.add_argument('file1')
    parser.add_argument('file2')
    parser.add_argument('outputfile')
    args = parser.parse_args()
    main(args)
