Install
=======

Below we assume you have the default Python environment already configured on
your computer and you intend to install *RerF* inside of it.  We strongly 
encourage the use of Python virtual environments, please follow instructions
on `venv <https://docs.python.org/3/library/venv.html>`_ and `virtual
environments <http://docs.python-guide.org/en/latest/dev/virtualenvs/>`_.

Make sure you have the latest version of ``pip`` (the Python package manager)
installed. If you do not, refer to the `Pip documentation
<https://pip.pypa.io/en/stable/installing/>`_ and install ``pip`` first.

Currently *RerF* is available on :ref:`Linux <linux>` and :ref:`MacOS <mac>`.

From PyPI
---------

::

  pip install rerf

https://pypi.org/project/rerf

From source
-----------


Requirements
````````````

.. _linux:

Linux
'''''

Make sure you have the appropriate build tools

::

    sudo apt-get install build-essential cmake python3-dev libomp-dev libeigen3-dev   # Ubuntu/Debian

.. _mac:

Mac
'''

- Install the SDK headers (Mojave update removes SDK headers)

  ::

      open /Library/Developer/CommandLineTools/Packages/macOS_SDK_headers_for_macOS_10.14.pkg
- Get `brew`_

  ::

      /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
- Get Python 3

  ::

      brew install python3

- Get openmp (for multiprocessing)

  ::

      brew install libomp
- Get llvm

  ::

      brew install llvm
- Get eigen

  ::
  
      brew install eigen

- Set up paths to use llvm's ``clang``

  ::

      echo ‘export PATH=“/usr/local/opt/llvm/bin:$PATH”’ >> ~/.bash_profile

  Source your ``.bash_profile``

  ::

      source ~/.bash_profile

.. _brew : https://brew.sh/


Install (from source)
`````````````````````

From PyPI
'''''''''

::

  pip install --no-binary rerf rerf

From GitHub
'''''''''''

::

  pip install -e "git+https://github.com/neurodata/RerF.git@staging#egg=rerf&subdirectory=Python"

Build from source
'''''''''''''''''

- Get the source files

  ::

      git clone https://github.com/neurodata/RerF.git

- Navigate to Python directory

  ::

      cd RerF/Python

- Clean out old installation (skip if fresh install)

  ::

      python setup.py clean --all

- Build using ``pip``

  ::

      pip install -e .
