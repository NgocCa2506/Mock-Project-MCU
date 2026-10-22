# Mock-Project-MCU

Dự án Bootloader + Application cho vi điều khiển **STM32F407VGTx**, xây dựng trên STM32CubeIDE. Bao gồm cơ chế **OTA (Over-The-Air) update** qua Flash nội, kiểm tra tính hợp lệ của firmware bằng **CRC-32**, và benchmark hiệu năng các thuật toán CRC.

Github: [github.com/NgocCa2506/Mock-Project-MCU](https://github.com/NgocCa2506/Mock-Project-MCU)

---

## Mục lục

1. [Cấu trúc dự án](#cấu-trúc-dự-án)
2. [Bản đồ Flash](#bản-đồ-flash)
3. [Luồng hoạt động](#luồng-hoạt-động)
4. [Cấu hình build](#cấu-hình-build)
5. [Hướng dẫn khởi chạy](#hướng-dẫn-khởi-chạy)
   - Bước 1 – Import project
   - Bước 2 – Build và nạp Bootloader
   - Bước 3 – Cấu hình post-build xuất `.bin`
   - Bước 4 – Build và nạp Current Application
6. [Quy trình OTA](#quy-trình-ota)
7. [Cập nhật app_size và app_crc](#cập-nhật-app_size-và-app_crc)
8. [Tín hiệu LED](#tín-hiệu-led)
9. [Benchmark CRC](#benchmark-crc)

---

## Cấu trúc dự án

```
Mock-Project-MCU/
├── STM32F407_Bootloader/          # Project Bootloader
│   └── Core/
│       ├── Inc/
│       │   ├── flash_layout.h     # Bản đồ Flash, dùng chung 2 project
│       │   ├── app_header.h       # Định nghĩa struct app_header_t
│       │   ├── app_header_validate.h
│       │   ├── boot_jump.h        # Nhảy sang Application
│       │   ├── boot_validate.h    # Kiểm tra hợp lệ Application
│       │   ├── boot_ota.h         # Cấu hình OTA (method, voltage range)
│       │   ├── crc32.h            # Các thuật toán CRC-32
│       │   ├── crc_benchmark.h    # Benchmark 3 thuật toán CRC
│       │   └── led.h
│       └── Src/
│           ├── main.c
│           ├── app_header_validate.c
│           ├── boot_jump.c
│           ├── boot_validate.c
│           ├── boot_ota.c         # Logic OTA
│           ├── crc32.c
│           ├── crc_benchmark.c
│           └── led.c
│
├── STM32F407_Application/         # Project Application
│   ├── Core/
│   │   ├── Inc/
│   │   │   ├── flash_layout.h     # Byte-identical với Bootloader
│   │   │   ├── app_header.h
│   │   │   ├── app_config.h       # Chọn build Current App / OTA App
│   │   │   ├── app_startup.h      # Khởi tạo VTOR khi được Bootloader gọi
│   │   │   ├── app_ota_request.h  # Yêu cầu Bootloader thực hiện OTA
│   │   │   └── led.h
│   │   └── Src/
│   │       ├── main.c
│   │       ├── app_header.c       # Định nghĩa g_app_header (size, CRC, version)
│   │       ├── app_startup.c
│   │       ├── app_ota_request.c  # Set ota_flag=1, reset MCU
│   │       └── led.c
│   ├── STM32F407VGTX_FLASH.ld     # Linker script Current App
│   └── Tools/
│       └── app_bin_info.py        # Tính app_size và app_crc từ .bin
```

> **Lưu ý:** `flash_layout.h` phải giữ **byte-identical** ở cả hai project.

---

## Bản đồ Flash

STM32F407VGTx có **1 MB Flash** nội, chia như sau:

| Sector | Địa chỉ                       | Kích thước | Mục đích                             |
| ------ | -------------------------------- | ------------- | --------------------------------------- |
| 0–2   | `0x08000000` – `0x0800BFFF` | 48 KB         | **Bootloader**                    |
| 3      | `0x0800C000` – `0x0800FFFF` | 16 KB         | Reserved                                |
| 4      | `0x08010000` – `0x0801FFFF` | 64 KB         | **App Header** (`app_header_t`) |
| 5–7   | `0x08020000` – `0x0807FFFF` | 384 KB        | **Application Code**              |
| 8      | `0x08080000` – `0x0809FFFF` | 128 KB        | **OTA Header** (`app_header_t`) |
| 9–11  | `0x080A0000` – `0x080FFFFF` | 384 KB        | **OTA Code**                      |

```
0x08000000 ┌──────────────────────┐
           │   Bootloader         │  Sector 0-2  (48 KB)
0x0800C000 ├──────────────────────┤
           │   Reserved           │  Sector 3    (16 KB)
0x08010000 ├──────────────────────┤
           │   App Header         │  Sector 4    (64 KB)
0x08020000 ├──────────────────────┤
           │                      │  Sector 5   (128 KB)
           │   Application Code   │  Sector 6   (128 KB)
           │                      │  Sector 7   (128 KB)
0x08080000 ├──────────────────────┤
           │   OTA Header         │  Sector 8   (128 KB)
0x080A0000 ├──────────────────────┤
           │                      │  Sector 9   (128 KB)
           │   OTA Code           │  Sector 10  (128 KB)
           │                      │  Sector 11  (128 KB)
0x08100000 └──────────────────────┘
```

---

## Luồng hoạt động

### Boot bình thường

```
Reset
  │
  ▼
Bootloader khởi động
  │  Blink RED LED (3 lần)
  │
  ├─ Đọc App Header (0x08010000)
  │
  ├─[ota_flag == 1]──► Chạy OTA ──► (xem Quy trình OTA)
  │
  ├─ validate_app()
  │    ├── Kiểm tra Magic Number (5 words)
  │    ├── Kiểm tra app_size
  │    ├── Tính CRC-32 và so sánh app_crc
  │    └── Kiểm tra Reset Handler address
  │
  ├─[INVALID]──► Blink RED LED vô hạn (lỗi)
  │
  ├─ crc_benchmark_run()
  │    ├── Bitwise CRC-32
  │    ├── Lookup Table CRC-32
  │    └── Hardware CRC-32
  │
  └─[all_results_match == 1]──► jump_to_app() ──► Application chạy
```

### Quy trình OTA

```
Application đang chạy
  │  Người dùng nhấn User Button (PA0)
  │
  ▼
app_ota_request()
  │  Đọc App Header → set ota_flag = 1 → ghi lại Sector 4
  │  NVIC_SystemReset()
  │
  ▼
Bootloader phát hiện ota_flag == 1
  │  Blink ORANGE LED (3 lần)
  │
  ├── Xóa ota_flag trong App Header (Sector 4)
  │
  ├── ota_image_valid() – kiểm tra OTA Header + CRC của OTA Code
  │     └─[INVALID]──► return (App không bị đụng)
  │
  ├── Sao chép OTA Code → App Code (theo OTA_METHOD)
  │
  └── Sao chép OTA Header → App Header, version += 1
        │
        ▼
     Tiếp tục flow boot bình thường (validate + jump)
```

---

## Cấu hình build

### `boot_ota.h` – Bootloader

| Macro                     | Giá trị                         | Mô tả                                              | Benchmark  |
| ------------------------- | --------------------------------- | ---------------------------------------------------- | ---------- |
| `OTA_METHOD`            | `OTA_METHOD_OVERWRITE`          | Ghi đè App Code, không xóa trước               | 469.557    |
|                           | `OTA_METHOD_ERASE_FIRST`        | Xóa toàn bộ 3 sector App, sau đó ghi            | 46.682.775 |
|                           | `OTA_METHOD_ERASE_SMART`        | Xóa đúng số sector cần thiết theo `app_size` | 16.020.926 |
| `FLASH_PROGRAM_VOLTAGE` | `FLASH_PROGRAM_VOLTAGE_RANGE_3` | 2.7–3.6 V, ghi 4 byte/lần (WORD)                   |            |
|                           | `FLASH_PROGRAM_VOLTAGE_RANGE_4` | + Vpp ngoài 8–9 V, ghi 8 byte/lần (DOUBLEWORD)    |            |

### `app_config.h` – Application

| Macro             | Giá trị | Mô tả                                            |
| ----------------- | --------- | -------------------------------------------------- |
| `APP_BUILD_OTA` | `0U`    | Build **Current App** → LED xanh lá (PD12) |
|                   | `1U`    | Build **OTA App** → LED xanh dương (PD15) |

### `crc32.h` – Bootloader

| Macro                      | Giá trị                      | Mô tả                                      |
| -------------------------- | ------------------------------ | -------------------------------------------- |
| `CRC_SELECTED_ALGORITHM` | `CRC_ALGORITHM_BITWISE`      | Bitwise, chậm nhất                         |
|                            | `CRC_ALGORITHM_LOOKUP_TABLE` | Lookup table 256 phần tử *(mặc định)* |
|                            | `CRC_ALGORITHM_HARDWARE`     | Hardware peripheral STM32, nhanh nhất       |

---

## Hướng dẫn khởi chạy

### Yêu cầu

- **STM32CubeIDE** (khuyến nghị ≥ 1.14)
- **STM32F407VG Discovery board** (hoặc board tương thích)
- **ST-Link** (tích hợp trên Discovery board)
- **Python 3** (để chạy `app_bin_info.py`)

### Bước 1 – Import project vào STM32CubeIDE

1. Mở STM32CubeIDE.
2. **File → Import → General → Existing Projects into Workspace**.
3. Chọn thư mục gốc `Mock-Project-MCU/`.
4. Tick cả hai project: `STM32F407_Bootloader` và `STM32F407_Application`.
5. Nhấn **Finish**.

### Bước 2 – Build và nạp Bootloader

1. Chọn project `STM32F407_Bootloader` trong Project Explorer.
2. **Project → Build Project** (hoặc `Ctrl+B`).
3. **Run → Debug** hoặc **Run → Run** để nạp lên board.
4. Bootloader sẽ nạp tại địa chỉ `0x08000000`.

### Bước 3 – Cấu hình post-build để xuất file `.bin`

STM32CubeIDE mặc định chỉ tạo file `.elf`. Cần cấu hình thêm để tự động tách ra hai file `.bin` sau mỗi lần build.

#### 3a – Bật tạo file `.bin` (tùy chọn tích hợp sẵn)

1. Chuột phải vào project `STM32F407_Application` → **Properties**.
2. Vào **C/C++ Build → Settings → MCU Post build outputs**.
3. Tick vào **Convert to binary file (-O binary)**.
4. Nhấn **Apply and Close**.

> Bước này tạo file `STM32F407_Application.bin` (toàn bộ image), nhưng **chưa** tách riêng `application.bin` và `app_header.bin`. Cần thêm post-build step ở bước tiếp theo.

#### 3b – Thêm post-build step tách `application.bin` và `app_header.bin`

Project dùng hai file `.bin` riêng biệt:

| File                | Nội dung                              | Mục đích                                                 |
| ------------------- | -------------------------------------- | ----------------------------------------------------------- |
| `application.bin` | Code App (bỏ section `.app_header`) | Đầu vào của `app_bin_info.py` để tính CRC và Size |
| `app_header.bin`  | Chỉ section `.app_header`           | Nạp riêng tại `0x08080000` cho header của OTA code    |

Để tự động tạo sau mỗi lần build:

1. Chuột phải vào project `STM32F407_Application` → **Properties**.
2. Vào **C/C++ Build → Settings → Build Steps → Post-build steps**.
3. Trong ô **Command**, dán lệnh sau:

```bash
arm-none-eabi-objcopy -O binary --remove-section=.app_header STM32F407_Application.elf application.bin && arm-none-eabi-objcopy -O binary --only-section=.app_header STM32F407_Application.elf app_header.bin
```

4. Ô **Description** có thể điền: `Generate application.bin and app_header.bin`.
5. Nhấn **Apply and Close**.

Sau khi build thành công, thư mục `Debug/` sẽ chứa:

```
Debug/
├── STM32F407_Application.elf   ← file gốc (ELF, đầy đủ debug info)
├── STM32F407_Application.bin   ← toàn bộ image (tùy chọn bước 3a)
├── application.bin             ← App Code only  (không có .app_header)
└── app_header.bin              ← App Header only (chỉ .app_header)
```

> **Giải thích lệnh:**
>
> - `--remove-section=.app_header` → loại bỏ section header, chỉ giữ code để tính CRC.
> - `--only-section=.app_header` → chỉ lấy section header, dùng để nạp riêng nếu cần.

### Bước 4 – Build và nạp Current Application

1. Mở `STM32F407_Application/Core/Inc/app_config.h`, đảm bảo:
   ```c
   #define APP_BUILD_OTA    0U
   ```
2. Build project: **Project → Build Project**.
3. Chạy `app_bin_info.py` để lấy `app_size` và `app_crc`:
   ```bash
   py -3 .\app_bin_info.py ..\Debug\application.bin
   ```
4. Cập nhật kết quả CRC và Size vào `Core/Src/app_header.c` (mục `/* Current App */`).
5. Build lại lần nữa.
6. Nạp lên board:
   - **Run → Debug Configurations** → chọn cấu hình Application.
   - Hoặc dùng STM32CubeProgrammer nạp `STM32F407_Application.bin`(file này chứa cả app_header và app code) tại `0x08010000`.

> Board sẽ blink **LED đỏ** (Bootloader) → **LED xanh lá** (Application chạy).

---

## Quy trình OTA

### Bước 1 – Build OTA App

1. Mở `STM32F407_Application/Core/Inc/app_config.h`, đổi thành:
   ```c
   #define APP_BUILD_OTA    1U
   ```
2. Build project.
3. Chạy `app_bin_info.py`:
   ```bash
   python Tools/app_bin_info.py Debug/application.bin
   ```
4. Cập nhật kết quả CRC và Size vào `Core/Src/app_header.c` (mục `/* OTA App */`).
5. Build lại lần nữa.

### Bước 2 – Nạp OTA image lên board

Nạp file `Debug/app_header.bin` tại địa chỉ **`0x08080000`** bằng STM32CubeProgrammer:

```
Address : 0x08080000
File    : Debug/app_header.bin
```

> File `.bin` này chứa App Header (`0x08080000`).



Nạp file `Debug/application.bin` tại địa chỉ **`0x080A0000`** bằng STM32CubeProgrammer:

```
Address : 0x080A0000
File    : Debug/application.bin
```

> File `.bin` này chứa App OTA (`0x080A0000`).

### Bước 3 – Kích hoạt OTA

1. Reset board để chạy lại Current App.
2. Nhấn **User Button (PA0)** trong khi Application đang chạy.
3. Bootloader tự động kích hoạt OTA:
   - **ORANGE LED** blink trong quá trình copy.
   - Sau khi xong, Application mới (LED xanh dương) sẽ chạy.
   - `version` trong App Header tăng lên 1.

---

## Cập nhật app_size và app_crc

Mỗi khi thay đổi code Application, phải cập nhật lại `app_size` và `app_crc` trong `app_header.c`:

```bash
# Current App (APP_BUILD_OTA = 0)
py -3 .\app_bin_info.py ..\Debug\application.bin
```

Kết quả mẫu:

```
Application size : 7700 bytes
Application CRC  : 0x9953026A

Copy into g_app_header (app_header.c):
    .app_size = 0x00001E14UL,
    .app_crc  = 0x9953026AUL
```

Dán vào đúng mục `/* Current App */` hoặc `/* OTA App */` trong `app_header.c`, sau đó **build lại**.

> Script dùng `zlib.crc32()` — tương thích với CRC-32/ISO-HDLC của Bootloader.

---

## Tín hiệu LED

| LED                 | GPIO | Màu            | Ý nghĩa                               |
| ------------------- | ---- | --------------- | --------------------------------------- |
| Bootloader running  | PD14 | 🔴 Đỏ         | Blink khi Bootloader đang khởi động |
| OTA in progress     | PD13 | 🟠 Cam          | Blink trong quá trình copy OTA        |
| Current App running | PD12 | 🟢 Xanh lá     | Blink khi Current App đang chạy       |
| OTA App running     | PD15 | 🔵 Xanh dương | Blink khi OTA App đang chạy           |
| Error               | PD14 | 🔴 Đỏ         | Blink vô hạn nếu validate thất bại |

---

## Benchmark CRC

Bootloader đo thời gian (CPU cycles) của 3 thuật toán CRC-32 trên toàn bộ App Code trước khi nhảy sang Application. Kết quả lưu trong biến `g_crc_benchmark` (xem qua **Live Expressions** trong STM32CubeIDE debugger):

| Trường              | Mô tả                                        |
| --------------------- | ---------------------------------------------- |
| `crc_bitwise`       | Giá trị CRC tính bằng thuật toán bitwise |
| `crc_lookup`        | Giá trị CRC tính bằng lookup table         |
| `crc_hardware`      | Giá trị CRC tính bằng hardware peripheral  |
| `cycles_bitwise`    | CPU cycles của thuật toán bitwise           |
| `cycles_lookup`     | CPU cycles của lookup table                   |
| `cycles_hardware`   | CPU cycles của hardware CRC                   |
| `all_results_match` | `1` nếu cả 3 giá trị CRC bằng nhau      |

Công thức quy đổi sang ms (HSI 16 MHz):

```
time_ms = cycles / (SystemCoreClock / 1000)
        = cycles / 16000
```

Tương tự, benchmark cho OTA được lưu trong:

| Biến                        | Mô tả                                     |
| ---------------------------- | ------------------------------------------- |
| `g_ota_cycles_overwrite`   | Cycles khi dùng `OTA_METHOD_OVERWRITE`   |
| `g_ota_cycles_erase_write` | Cycles khi dùng `OTA_METHOD_ERASE_FIRST` |
| `g_ota_cycles_erase_smart` | Cycles khi dùng `OTA_METHOD_ERASE_SMART` |
