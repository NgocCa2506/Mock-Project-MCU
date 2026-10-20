#!/usr/bin/env python3
import argparse
import zlib
from pathlib import Path

APP_MAX_SIZE = 384 * 1024       # must match APP_SIZE in flash_layout.h


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Print app_size and app_crc of application.bin"
    )
    parser.add_argument("application_bin", type=Path)
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

    print(f"Application size : {app_size} bytes")
    print(f"Application CRC  : 0x{app_crc:08X}")
    print()
    print("Copy into g_app_header (app_header.c):")
    print(f"    .app_size = 0x{app_size:08X}UL,")
    print(f"    .app_crc  = 0x{app_crc:08X}UL")


if __name__ == "__main__":
    main()
