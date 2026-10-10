#!/usr/bin/env fish
# install-esp-idf.fish — one-time ESP-IDF v5.4 install for esp32s3.
#
# Run with:  fish tools/install-esp-idf.fish
#
# Uses the uv-managed Python 3.12 (ESP-IDF supports 3.9–3.13; system python is
# 3.14 which is NOT supported). Purely additive: installs into ~/esp/esp-idf and
# ~/.espressif. Does NOT modify global config, does NOT use sudo.
#
# Disk: ~2.5–3.5 GB. Idempotent-ish: skips the clone if the dir exists.

set -l IDF_VERSION v5.4
set -l IDF_DIR $HOME/esp/esp-idf
set -l UV_PY (uv python find 3.12)

if test -z "$UV_PY"
    echo "ERROR: uv python 3.12 not found. Run: uv python install 3.12"
    exit 1
end

echo "Using Python: $UV_PY"

if not test -d $IDF_DIR
    echo "Cloning ESP-IDF $IDF_VERSION (recursive, may take a while)..."
    mkdir -p $HOME/esp
    git clone -b $IDF_VERSION --recursive https://github.com/espressif/esp-idf.git $IDF_DIR
    or begin
        echo "Clone failed."
        exit 1
    end
else
    echo "ESP-IDF dir already exists: $IDF_DIR (skipping clone)"
end

# Force IDF to use the uv python for its virtualenv.
set -gx IDF_PYTHON_ENV_PATH $HOME/.espressif/python_env/idf5.4_py312_env

cd $IDF_DIR
echo "Installing esp32s3 toolchain + python env..."
env PATH="(dirname $UV_PY):$PATH" ./install.sh esp32s3
or begin
    echo "install.sh failed."
    exit 1
end

echo ""
echo "Done. To use it in a new shell:"
echo "    source $IDF_DIR/export.fish"
echo "    cd <repo>/firmware"
echo "    idf.py set-target esp32s3"
echo "    idf.py build"
