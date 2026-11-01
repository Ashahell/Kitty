#!/usr/bin/env python3
"""Deterministic generator for the M0a MOD fixtures.

No third-party data: every sample is synthesised here.  Run without arguments
(good file) or with --bad len0|len129|trunc_pattern|trunc_sample.
"""
import sys

PAT = 64


def cell(sample=0, period=0, effect=0, param=0):
    b0 = (sample & 0xF0) | ((period >> 8) & 0x0F)
    b1 = period & 0xFF
    b2 = ((sample & 0x0F) << 4) | (effect & 0x0F)
    return bytes((b0, b1, b2, param & 0xFF))


def pattern(cells):
    data = bytearray(PAT * 16)
    for (row, ch), c in cells.items():
        off = row * 16 + ch * 4
        data[off:off + 4] = c
    return bytes(data)


def sample_header(name, length_words, finetune, volume, loop_start, loop_len):
    return (name.encode("ascii")[:22].ljust(22, b"\0") +
            length_words.to_bytes(2, "big") + bytes((finetune, volume)) +
            loop_start.to_bytes(2, "big") + loop_len.to_bytes(2, "big"))


def square(n=128):
    return bytes([100 if i < n // 2 else 0x9C for i in range(n)])


def saw(n=64):
    return bytes([(i * 4 - 128) & 0xFF for i in range(n)])


def build(song_len=3, orders=(0, 1, 2), npat=3, sample_len_words=64,
          include_samples=True, patterns=None):
    out = bytearray()
    out += b"Kitty test module".ljust(20, b"\0")[:20]
    out += sample_header("square", sample_len_words, 0, 64, 0, sample_len_words)
    out += sample_header("saw", 32, 0, 48, 0, 0)
    for _ in range(29):
        out += sample_header("", 0, 0, 0, 0, 0)
    out.append(song_len)
    out.append(0)
    order = bytearray(128)
    for i, o in enumerate(orders):
        if i < len(order):
            order[i] = o
    out += order
    out += b"M.K."
    for p in patterns or []:
        out += p
    for _ in range(npat - len(patterns or [])):
        out += bytes(1024)
    if include_samples:
        out += square()
        out += saw()
    return bytes(out)


def good_patterns():
    p0 = {
        (0, 0): cell(1, 428, 0, 0),
        (0, 1): cell(0, 0, 0xF, 0x03),
        (0, 2): cell(0, 0, 0xF, 0x96),
        (0, 3): cell(0, 0, 0xC, 0x20),
        (1, 1): cell(0, 0, 0xC, 0x41),
        (2, 2): cell(0, 0, 0xF, 0x7D),
        (3, 3): cell(0, 0, 0xF, 0xFF),
        (4, 1): cell(0, 0, 0xF, 0x1F),
        (5, 2): cell(0, 0, 0xF, 0x01),
        (6, 3): cell(0, 0, 0xF, 0x20),
        (7, 1): cell(0, 0, 0xD, 0x15),
        (8, 2): cell(0, 0, 0xB, 0x00),
    }
    p1 = {(0, 1): cell(0, 0, 0xC, 0x00)}
    p2 = {(0, 1): cell(2, 856, 0, 0),
          (1, 2): cell(0, 0, 0xB, 0x00)}
    return [pattern(p0), pattern(p1), pattern(p2)]


def main(argv):
    bad = None
    if len(argv) == 3 and argv[0] == "--bad":
        bad = argv[1]
        out_path = argv[2]
    elif len(argv) == 1:
        out_path = argv[0]
    else:
        print("usage: gen_test_mod.py [--bad len0|len129|trunc_pattern|trunc_sample] OUT")
        return 2
    if bad == "len0":
        data = build(song_len=0, orders=(0,), patterns=good_patterns())
    elif bad == "len129":
        data = build(song_len=129, orders=(0,), patterns=good_patterns())
    elif bad == "trunc_pattern":
        pats = good_patterns()
        data = build(song_len=3, orders=(0, 1, 2), npat=1, patterns=[pats[0]])
    elif bad == "trunc_sample":
        data = build(sample_len_words=1000, include_samples=False,
                     patterns=good_patterns())
    elif bad is None:
        data = build(patterns=good_patterns())
    else:
        print("unknown bad kind: " + bad)
        return 2
    with open(out_path, "wb") as f:
        f.write(data)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
