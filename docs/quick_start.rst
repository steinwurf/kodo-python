Quick Start
===========

.. note:: You will need a valid license to access and use the
          library. For more information on how to obtain a license,
          visit https://steinwurf.com/license.

If you already installed a C++14 compiler, git and python on your system,
then you can clone this repository to a suitable folder::

    git clone git@github.com:steinwurf/kodo-python.git

After cloning the repository you will have the latest development snapshot.
Unless you need to work on the latest and greatest we recommend that you
switch to a released version of the library.

.. parsed-literal::

  cd kodo-python
  git checkout \|release\|

Where \|release\| should be replaced with the latests release.

Using the Waf buildsystem
-------------------------
Waf is the primary buildsystem used at Steinwurf use this to build the library
along with the tests, examples, and application.

Configure and build the project::

    python waf configure
    python waf build
    python waf install --destdir=kodo_python_install

The final ``install`` step will create a folder containing all the necessary
files needed to use the library. You can change the output folder by passing a
different path to ``--destdir``.
You can also copy/move this install folder anywhere you like.
