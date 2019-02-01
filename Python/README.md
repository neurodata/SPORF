# Randomer Forest (rerf) Python Package

## Requirements

Python bindings are made using [pybind11](https://github.com/pybind/pybind11).

C++ compiler (gcc)

  ```sh
  sudo apt-get install build-essential cmake python3-dev   # Ubuntu/Debian
  ```

## Environment

Activate virtualenv

```sh
python3 -m venv env
. env/bin/activate
```

Install requirements

```sh
pip install pybind11
```

- Install from github

  ```sh
  pip install -e "git+https://github.com/neurodata/RerF.git@pybind#egg=rerf&subdirectory=Python"
  ```

- Build from source

  ```sh
  git clone https://github.com/neurodata/R-RerF.git
  cd R-RerF/Python
  c++ -O3 -Wall -shared -std=c++14 -fPIC `python3 -m pybind11 --includes` packedForest.cpp -o pyfp`python3-config --extension-suffix`
  ```

Note: problems running the compile command on `fish`.  Use `bash` for the time being.

## Run the example

[Example.py](examples/example.py)
