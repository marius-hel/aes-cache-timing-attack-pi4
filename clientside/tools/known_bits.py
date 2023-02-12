""" Really unoptimized but simple script to analyze correlation result """
import argparse
import sys

remaining = [[] for _ in range(16)]
probs = [[[0, 0] for _ in range(8)] for _ in range(16)]

def main(arg):
    """ Main function """

    with open(arg.file, mode='r', encoding='utf-8') as file:
        lines = file.readlines()
        for line in lines:
            splitted = line.split()
            k = int(splitted[1])
            remaining[k] = [int(hexstr, base=16) for hexstr in splitted[2:]]

    for k in range(16):
        for remaining_byte in remaining[k]:
            for i in range(8):
                if remaining_byte & (1 << i):
                    probs[k][i][1] +=1
                else:
                    probs[k][i][0] +=1
        n_remaining_k = len(remaining[k])
        for i in range(8):
            probs[k][i][0] = probs[k][i][0] / n_remaining_k
            probs[k][i][1] = probs[k][i][1] / n_remaining_k

    min_prob = arg.prob
    known_count = 0
    key_bits = [['_' for _ in range(8)] for _ in range(16)]
    for k in range(16):
        for i in range(8):
            if probs[k][i][0] >= min_prob:
                key_bits[k][7-i] = '0'
                known_count += 1
            elif probs[k][i][1] >= min_prob:
                key_bits[k][7-i] = '1'
                known_count += 1

    key_bits_str = ""
    for k in range(16):
        byte_str = ""
        for i in range(8):
            byte_str += key_bits[k][i]
        key_bits_str += byte_str + ' '

    print(f"{known_count} bits of the key are supposed known with a probability >= {min_prob}")
    print("From left to right: k[0] ... k[15] ")
    print(key_bits_str)
    sys.exit(0)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=('Analyze known bits from <file> after',
    'correlation, <prob> is the minimum probability for a bit to be qualified as known'))
    parser.add_argument('file')
    parser.add_argument('prob', type=float)
    args = parser.parse_args()
    if args.prob > 1.0:
        print("Error: prob should be <= 1.0")
        sys.exit(1)
    if args.prob <= 0.5:
        print("Error: prob should be > 0.50")
        sys.exit(1)
    main(args)
