#! /usr/bin/env python
# encoding: utf-8

import os
from waflib.Build import BuildContext

APPNAME = "kodo-python"
VERSION = "1.0.0"


def options(opt):
    opt.load("python")


def build(bld):

    bld(
        features="cxx cxxshlib pyext",
        source=bld.path.ant_glob("src/kodo_python/**/*.cpp"),
        target="kodo",
        name="kodo-python",
        use=["kodo", "pybind11_includes"],
        install_path="${PREFIX}/lib",
    )

    if bld.is_toplevel():
        if bld.has_tool_option("run_tests"):
            bld.add_post_fun(exec_test_python)


class ReleaseContext(BuildContext):
    cmd = "prepare_release"
    fun = "prepare_release"


def prepare_release(ctx):
    """Prepare a release."""

    # Rewrite versions
    with ctx.rewrite_file(filename="src/kodo_python/version.hpp") as f:

        pattern = r"#define STEINWURF_KODO_PYTHON_VERSION v\d+_\d+_\d+"
        replacement = "#define STEINWURF_KODO_PYTHON_VERSION v{}".format(
            VERSION.replace(".", "_")
        )

        f.regex_replace(pattern=pattern, replacement=replacement)

    with ctx.rewrite_file(filename="src/kodo_python/version.cpp") as f:
        pattern = r'return "\d+\.\d+\.\d+"'
        replacement = 'return "{}"'.format(VERSION)

        f.regex_replace(pattern=pattern, replacement=replacement)

    with ctx.rewrite_file(filename="setup.cfg") as f:

        pattern = r"version = \d+\.\d+\.\d+"
        replacement = "version = {}".format(VERSION)

        f.regex_replace(pattern=pattern, replacement=replacement)


def exec_test_python(bld):
    python = bld.env["PYTHON"][0]
    env = dict(os.environ)
    env["PYTHONPATH"] = os.path.join(bld.out_dir)

    # First, run the unit tests in the 'test' folder
    if os.path.exists("tests"):
        for f in sorted(os.listdir("tests")):
            if f.endswith(".py"):
                test = os.path.join("tests", f)
                bld.cmd_and_log("{0} {1}\n".format(python, test), env=env)

    # Then run the examples in the 'examples' folder
    if os.path.exists("examples"):
        for path, _, files in os.walk("examples"):
            for f in files:
                if f.endswith(".py"):
                    example = os.path.join(path, f)
                    bld.cmd_and_log(
                        "{0} {1} --dry-run\n".format(python, example), env=env
                    )
                    print("-------------------------------\n")


def docs(ctx):
    """Build the documentation in a virtualenv"""

    with ctx.create_virtualenv() as venv:

        # To update the requirements.txt just delete it - a fresh one
        # will be generated from test/requirements.in
        if not os.path.isfile("docs/requirements.txt"):
            venv.run("python -m pip install pip-tools")
            venv.run("pip-compile docs/requirements.in")

        venv.run("python -m pip install -r docs/requirements.txt")

        build_path = os.path.join(ctx.path.abspath(), "build", "site", "docs")

        venv.run(
            "giit clean . --build_path {}".format(build_path), cwd=ctx.path.abspath()
        )

        arguments = []
        arguments.append("--build_path {}".format(build_path))
        git_protocol = ctx.env.stored_options["git_protocol"]
        if git_protocol:
            arguments.append(
                "--variable git_protocol --git_protocol={}".format(git_protocol)
            )

        venv.run("giit sphinx . {}".format(" ".join(arguments)), cwd=ctx.path.abspath())
