Kodo-Python
===========
This package contains high-level python bindings for the Kodo Network Coding
library. The bindings provide access to basic functionality provided by Kodo,
such as encoding and decoding of data. The examples folder provides sample
applications showing usage of the python API.

.. image:: https://badge.fury.io/py/kodo.svg
    :target: http://badge.fury.io/py/kodo
.. image:: https://pypip.in/download/kodo/badge.svg
    :target: https://pypi.python.org/pypi//kodo/
    :alt: Downloads
.. image:: https://pypip.in/py_versions/kodo/badge.svg
    :target: https://pypi.python.org/pypi/kodo/
    :alt: Supported Python versions
.. image:: https://pypip.in/format/kodo/badge.svg
    :target: https://pypi.python.org/pypi/kodo/
    :alt: Download format
.. image:: https://pypip.in/license/kodo/badge.svg
    :target: https://pypi.python.org/pypi/kodo/
    :alt: License

License
=======

If you wish to use this library, please obtain a valid license. To do so
**you must fill out the license request** form_.

This project is available under a research and educational friendly licensee,
see the details in the README.rst file.

.. _form: http://steinwurf.com/license/

Installation
============
We provide a pip package for easy installation of the kodo-python
library.

To install this you'll need python and pip installed:
 - To get python `go here <https://www.python.org/downloads/>`_.
 - To install pip `follow this guide
   <https://pip.pypa.io/en/latest/installing.html>`_.

Depending on your platform, additional requirements may be needed.
This is due to the fact that we only provide pre-built versions for some
specific configurations, but not all.

This table shows which platforms are supported by a wheel (pre-built) and which
are supported by a source package.

+---------------------------+---------------+--------------+----------------+--------------+--------------+----------------+
| Platform / Python version | MacOs  32 Bit | Linux 32 Bit | Windows 32 Bit | MacOs 64 Bit | Linux 64 Bit | Windows 64 Bit |
+===========================+===============+==============+================+==============+==============+================+
| **Python 2.7 32 Bit**     | Source        |  Source      | **Wheel**      | Source       |  Source      | **Wheel**      |
+---------------------------+---------------+--------------+----------------+--------------+--------------+----------------+
| **Python 2.7 64 Bit**     | Source        |  Source      | Source         | **Wheel* *   |  Source      | Source         |
+---------------------------+---------------+--------------+----------------+--------------+--------------+----------------+
| **Python 3.4 32 Bit**     | Source        |  Source      | Source         | Source       |  Source      | Source         |
+---------------------------+---------------+--------------+----------------+--------------+--------------+----------------+
| **Python 3.4 64 Bit**     | Source        |  Source      | Source         | Source       |  Source      | Source         |
+---------------------------+---------------+--------------+----------------+--------------+--------------+----------------+

If we do not have a pre-built version for your configuration, you'll need the
requirements specified in `Requirements for Building From Source`_ to install
the package.

Requirements for Building From Source
=====================================
Depending the platform, different steps are needed for building
kodo-python.

Please note, the compiler you download should be recent. The compilers used by
Steinwurf is listed on the `buildbot page <http://buildbot.steinwurf.com>`_.

Linux
-----
These steps may not work with your specific Linux distribution, but they may
at least guide you in the right direction.

First acquire the needed requirements from your package management system::

  sudo apt-get update
  sudo apt-get install python git build-essential libpython-dev

If you are using python 3, you'll need to install ``libpython3-dev`` instead.

MacOS
-----

Follow `this guide
<https://help.github.com/articles/set-up-git#setting-up-git>`_ to install git.

Install Xcode and Commandline Tools from the Mac Store.

Windows
-------
Install a 32-bit Python and Visual Studio 2013.
Now set the following environment variable ``VS90COMNTOOLS`` to::

  C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\

so that Python distutils can detect your new compiler.

Building From Source
====================
Now that you have all requirements, you are ready to clone, configure and build
the project::
::

    git clone https://github.com/steinwurf/kodo-python.git

configure and build the project::

  cd kodo-python
  python waf configure
  python waf build

Now the project is built and you should be able to find the resulting
kodo.so file here::

  build/linux/src/fifi_python/kodo.so

To use it simply add it to your python path and import it in your python
script like so::

  >>> import kodo
