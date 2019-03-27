# Randomer Forest (rerf) Python Package

## Requirements

Python bindings use [pybind11](https://github.com/pybind/pybind11).

C++ compiler (gcc)

  ```sh
  sudo apt-get install build-essential cmake python3-dev libomp-dev   # Ubuntu/Debian
  ```

On Mac:

- Get `brew`: [https://brew.sh/](https://brew.sh/)
- `brew install python`
- `brew install libomp`
- `brew install llvm`
- `echo ‘export PATH=“/usr/local/opt/llvm/bin:$PATH”’ >> ~/.bash_profile`
- `export LDFLAGS=“-L/usr/local/opt/llvm/lib”`
- `export CPPFLAGS="-I/usr/local/opt/llvm/include"`

## Environment

Create and activate virtualenv/venv

```sh
python3 -m venv env
. env/bin/activate
```
Anaconda Python may cause problems if not pointing to clang  version or greater 10.0.0

## Install from GitHub

  ```sh
  pip install -e "git+https://github.com/neurodata/RerF.git@staging#egg=rerf&subdirectory=Python"
  ```

## Build from source

- Get the source files

  ```sh
  git clone https://github.com/neurodata/RerF.git
  ```

- Navigate to Python directory

  ```sh
  cd RerF/Python
  ```

- Clean out old installation

  ```sh
  python setup.py clean --all
  ```

- Build the package using `setup.py`

  ```sh
  pip install -e .
  ```

## Example

[Example.py](example.py) shows basic usage

## Tests

We use pytest for Python testing

Run the tests from command line at the root of the repo (`RerF/`)

  ```sh
  python -m pytest
  ```
