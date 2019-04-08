Install
=======

Below we assume you have the default Python environment already configured on
your computer and you intend to install ``rerf`` inside of it.  If you want
to create and work with Python virtual environments, please follow instructions
on `venv <https://docs.python.org/3/library/venv.html>`_ and `virtual
environments <http://docs.python-guide.org/en/latest/dev/virtualenvs/>`_.

First, make sure you have the latest version of ``pip`` (the Python package manager)
installed. If you do not, refer to the `Pip documentation
<https://pip.pypa.io/en/stable/installing/>`_ and install ``pip`` first.

Requirements
------------

Python bindings use pybind11_.

.. _pybind11: https://github.com/pybind/pybind11

C++ compiler (gcc)::

  sudo apt-get install build-essential cmake python3-dev libomp-dev   # Ubuntu/Debian
  

On Mac:

- Get `brew`_
- ``brew install python3``
- ``brew install libomp``
- ``brew install llvm``
- ``echo ‘export PATH=“/usr/local/opt/llvm/bin:$PATH”’ >> ~/.bash_profile``
- ``export LDFLAGS=“-L/usr/local/opt/llvm/lib”``
- ``export CPPFLAGS="-I/usr/local/opt/llvm/include"``

.. _brew : https://brew.sh/


Install from GitHub
-------------------

::

  pip install -e "git+https://github.com/neurodata/RerF.git@staging#egg=rerf&subdirectory=Python"
  

Build from source
-----------------

- Get the source files

``git clone https://github.com/neurodata/RerF.git``

- Navigate to Python directory

``cd RerF/Python``

- Clean out old installation

``python setup.py clean --all``

- Build the package using `setup.py`

``pip install -e .``
