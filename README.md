# Test OrangePi 3b GPIO operations

## Requirements
 - Board: Orange Pi 3b rev.2
 - OS: Armbian OS v25.5 rolling for Orange Pi 3B running Armbian Linux 6.1.99-vendor-rk35xx

## Install dependencies
```bash
sudo apt install build-essential meson
```

## Clone repository and enter to it's folder
```bash
git clone https://github.com/oktilon/opi3b-test.git
cd opi3b-test
```

## Configure build
```bash
meson setup build
```

## Build app
```bash
ninja -C build
```

## Run app
```bash
build/app
```

