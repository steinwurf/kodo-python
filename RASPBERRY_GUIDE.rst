Compiling on Raspberry Pi (both 1 and 2)
========================================

Before you start, make sure you have the following at your ready:

* A valid license to the Kodo library.
* Fully functioning Raspberry Pi with Raspbian or something similar installed.
* An **empty** USB stick with minimum of 1GB space (only needed for Raspberry Pi 1).
* Some way to interact with your Raspberry Pi (keyboard or SSH access).
* An Internet Connection for your Raspberry Pi.
* ~30 minutes worth of coffee or similar beverage (~1.5 hours for Raspberry Pi 1).

Start by booting up the Raspberry Pi and open up a terminal - if you have a
headless install, this should be fairly trivial.

First, update your package manager::

    sudo apt-get update

You are now ready to install the required packages::

    sudo apt-get install git-core build-essential python-dev g++-4.9

Also make sure that ``libpython`` is installed (this might be done automatically when 
``python-dev`` is installed). On my Raspberry Pi 1, the package was called `libpython2.7`,
but on my Raspberry Pi 2 it was called `libpython-dev`. This will depend on your
distribution.

We need to make sure that the default compiler is g++ 4.9 (or later) on the Raspberry Pi.
You can quickly check the default version like this::

    g++ --version
    
    g++ (Raspbian 4.9.2-10+deb8u1) 4.9.2
    Copyright (C) 2014 Free Software Foundation, Inc.
    This is free software; see the source for copying conditions.  There is NO
    warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.


If you see an older version, you can change the default using update-alternatives::

    sudo update-alternatives --remove-all gcc
    sudo update-alternatives --remove-all g++
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.9 40 \
    --slave /usr/bin/g++ g++ /usr/bin/g++-4.9

You have configured the Raspberry Pi so that it's ready for compiling kodo-python.
Clone the repository::

    git clone https://github.com/steinwurf/kodo-python.git

Change directory to the repository::

    cd kodo-python

Run the waf configure step - in this process you will be queried for your
Github username and password multiple times::

    python waf configure

When the configuration has finished successfully, you can run the build command
(note that ``-j1`` forces waf to use a single CPU core, otherwise you will surely
exhaust the available RAM)::

    python waf build -j1
    
If the build step fails with an error like this, then you need to add more swap space
following the instructions below::
    
    virtual memory exhausted: Cannot allocate memory

The build step takes about 30 minutes on RPi 2 and several hours on RPi 1.

.. image:: https://imgs.xkcd.com/comics/compiling.png
    :target: https://xkcd.com/303/
    :alt: Compiling

Finally enjoy your freshly made kodo-python for the Raspberry Pi!!!

Adding swap on a USB drive
..........................

Because the compilation of kodo-python is rather memory-intensive, the installed
memory and swap are not sufficient (the Pi 1 runs out of memory).
Therefore we need to use an external USB drive as extra swap. To set this up,
plug in the USB drive and execute the following command to find the drive ID::

    sudo fdisk -l

When you found the drive ID, execute the following commands, replacing `sdx1`
with your drive ID. The drive ID I got was `sda1`::

    sudo umount /dev/sdx1
    sudo mkswap /dev/sdx1
    sudo swapon -p 32767 /dev/sdx1

To check whether the swap was installed correctly execute the following command:

    cat /proc/swaps

The command should output something like this::

    Filename      Type          Size  Used  Priority
    /var/swap     file        102396     0        -1
    /dev/sdx1     partition  4029096     0     32767
