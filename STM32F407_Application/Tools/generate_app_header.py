#!/usr/bin/env python3
import argparse
import struct
import zlib
from pathlib import Path

MAGIC_NUMBER = (
    0x41505048,
    0x53544D33,
    0x32463430,
    0x37424F4F,
    0x54415050,
)

APP_MAX_SIZE = 384 * 1024


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Create STM32F407 Application Header from application.bin"
    )
    parser.add_argument("application_bin", type=Path)
    parser.add_argument("header_bin", type=Path)
    args = parser.parse_args()

    app_data = args.application_bin.read_bytes()
    app_size = len(app_data)

    if app_size == 0:
        raise SystemExit("ERROR: application.bin is empty")

    if app_size > APP_MAX_SIZE:
        raise SystemExit(
            f"ERROR: application.bin is {app_size} bytes; "
            f"maximum is {APP_MAX_SIZE} bytes"
        )

    # CRC-32/ISO-HDLC, identical to the Bootloader software implementation.
    app_crc = zlib.crc32(app_data) & 0xFFFFFFFF

    # STM32F407 is little-endian: 5 magic words + size + CRC = 28 bytes.
    header_data = struct.pack(
        "<7I",
        *MAGIC_NUMBER,
        app_size,
        app_crc,
    )

    args.header_bin.write_bytes(header_data)

    print(f"Application size : {app_size} bytes")
    print(f"Application CRC  : 0x{app_crc:08X}")
    print(f"Header size      : {len(header_data)} bytes")
    print(f"Header output    : {args.header_bin}")


if __name__ == "__main__":
    main()
