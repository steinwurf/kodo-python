License
-------

A valid Kodo license is required if you wish to use this project.

Please request a license by **filling out the license request** form_.

Kodo is available under a research- and education-friendly license,
you can see the details here_.

If you try to configure without a valid license, then you will get an error!

.. _form: http://steinwurf.com/license/
.. _here: http://steinwurf.com/research-license/

About
-----

kodo-python contains a set of high-level Python bindings for the Kodo Network
Coding C++ library. The bindings provide access to basic functionality provided
by Kodo, such as encoding and decoding data. The examples folder contains
sample applications showing the usage of the Python API.

.. image:: http://buildbot.steinwurf.dk/svgstatus?project=kodo-python
    :target: http://buildbot.steinwurf.dk/stats?projects=kodo-python
    :alt: Buildbot status

If you have any questions or suggestions about this library, please contact
us at our developer mailing list (hosted at Google Groups):

* http://groups.google.com/group/steinwurf-dev

.. contents:: Table of Contents:
   :local:

Requirements
------------

First of all, follow `this Getting Started guide
<http://docs.steinwurf.com/getting_started.html>`_ to install
the basic tools required for the compilation (C++11 compiler, Git, Python).

The compilers used by Steinwurf are listed at the bottom of the
`buildbot page <http://buildbot.steinwurf.com>`_.

Linux
.....

These steps may not work with your specific Linux distribution, but they may
guide you in the right direction.

First, acquire the required packages from your package management system::

  sudo apt-get update
  sudo apt-get install python build-essential libpython-dev python-dev

If you are using Python 3, you'll need to install ``libpython3-dev`` instead.

Mac OSX
.......

Install the latest XCode and Command Line Tools from the Mac Store.

Python 2.7 is pre-installed on OSX, and the required Python headers should
also be available. If you are having trouble with the pre-installed Python
version, then you can install a more recent Python version with MacPorts or
Homebrew.

Windows
.......

Install Python 2.7 (32-bit) and Visual Studio Express 2015 for Windows Desktop.
Then set the ``VS90COMNTOOLS`` environment variable to::

  C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\

so that Python distutils can detect your new compiler.


Building From Source
--------------------

It is recommended to build the Python bindings from source.

First, clone the project::

  git clone git@github.com:steinwurf/kodo-python.git

Configure and build the project::

  cd kodo-python
  python waf configure
  python waf build

After building the project, you should find the resulting ``kodo.so``,
``kodo.dylib`` or ``kodo.pyd`` file here (the actual path and extension
depend on your OS)::

  build/linux/src/kodo_python/kodo.so
  build/darwin/src/kodo_python/kodo.dylib
  build/win32/src/kodo_python/kodo.pyd

You can copy this file to the same folder as your Python scripts, or you
can copy it to your PYTHONPATH (so that you can import it from anywhere).

Then you can import the module in your Python script::

  >>> import kodo

Special Options
...............

With the ``enable_codecs`` option, you can configure kodo-python to only enable
some desired codecs and disable all others. For example::

    python waf configure --enable_codecs=full_vector

Run ``python waf --help`` to list the available codecs. You can even
select multiple codecs with a comma-separated list::

    python waf configure --enable_codecs=full_vector,on_the_fly,nocode

Compilation Issues on Linux
...........................

The compilation process might take a long time on certain Linux systems if
less than 4 GB RAM is available. The g++ optimizer might consume a lot of RAM
during the compilation, so if you see that all your RAM is used up, then
it is recommended to constrain the number of parallel jobs to one during the
build step::

    python waf build -j 1

With this change, a fast compilation is possible with 2 GB RAM.

This issue is specific to g++ (which is the default compiler on Linux), but
the RAM usage and the compilation time can be much better with clang.
The code produced by clang is also fast.

If the compilation does not work with g++, then you can install clang like
this (on Ubuntu and Debian)::

    sudo apt-get install clang-3.5

Then you should configure the project with the appropriate mkspec. Use the
following command on 32-bit Linux::

    python waf configure --cxx_mkspec=cxx_clang35_x86

Or use this one on 64-bit Linux::

    python waf configure --cxx_mkspec=cxx_clang35_x64

Compiling on the Raspberry Pi
.............................

The detailed instructions for compiling the project on the Raspberry Pi
are found in our `Raspberry guide
<https://github.com/steinwurf/kodo-python/blob/master/RASPBERRY_GUIDE.rst>`_.
