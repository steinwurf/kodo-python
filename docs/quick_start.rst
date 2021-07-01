Quick Start
===========

.. note:: You will need a valid license to access and use the
          library. For more information on how to obtain a license,
          visit https://steinwurf.com/license.

Installation as Package
-----------------------
Since Kodo is a proprietary project it's not possible to make it available on
`The Python Package Index (PyPI) <https://pypi.org/>`_.
Fortunately it can still be installed as a package using pip. This is useful if
kodo-python is to be used in a virtual environment.

After installing the dependencies (listed below), the installation is done by
following these simple steps::

    # clone the repository
    git clone git@github.com:steinwurf/kodo-python
    # optionally change the version
    cd kodo-python
    git checkout [desired version or git-id]
    cd ..
    # install kodo-python from the repository path
    pip install ./kodo-python

Depending on your operating system, various dependencies are needed before the
build is possible.

Linux Requirements
..................

::

  sudo apt-get update
  sudo apt-get install python build-essential libpython3-dev python3-dev cmake ninja-build

Mac OSX Requirements
....................

* Install the newest version of `XCode <https://developer.apple.com/xcode/>`_.
* Install `cmake <https://cmake.org/>`_.
* Install `ninja <https://ninja-build.org/>`_.

Windows Requirements
....................

* Enable `Developer Mode <https://docs.microsoft.com/en-us/windows/apps/get-started/enable-your-device-for-development>`_
  to support creating symlinks.
* Install Visual Studio. Make sure to also choose the following Workloads:
    - *Desktop development with C++*, and
    - *Python Development* - here you also need to tick
      *Python native development tools* on.
* Install `Python 3 <https://python.org/>`_ (remember adding python3 to path).
* Install `cmake <https://cmake.org/>`_ (remember adding cmake to path).

Building with Waf
-----------------

To build the project with waf, first clone the project::

  git clone git@github.com:steinwurf/kodo-python.git

Configure and build the project::

  cd kodo-python
  python3 waf configure
  python3 waf build

.. note:: If you are building on windows you may need to set the 
    `MSSdk` and `DISTUTILS_USE_SDK` environment variables to configure the
    project::

        $Env:DISTUTILS_USE_SDK = 1
        $Env:MSSdk = 1

After building the project, you should find the resulting ``kodo.so``,
``kodo.dylib`` or ``kodo.pyd`` file here ::

  build_current/kodo[.so, .dylib, .pyd]

The unit tests can be run like so::

    python3 waf --run_tests

You can copy this file to the same folder as your Python scripts, or you
can copy it to your PYTHONPATH (so that you can import it from anywhere).

Then you can import the module in your Python script::

  >>> import kodo

Building From Source With CMake
-------------------------------

To build from source with CMake do the following::

    mkdir cmake_build
    cd cmake_build
    cmake -DPYTHON_EXECUTABLE=/usr/bin/python3 ..
    cmake --build .
