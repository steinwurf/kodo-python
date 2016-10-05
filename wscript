#! /usr/bin/env python
# encoding: utf-8

import os
import waflib.extras.wurf_options
from waflib.TaskGen import feature, after_method

APPNAME = 'kodo-python'
VERSION = '10.0.0'

codecs = ['nocode', 'full_vector', 'on_the_fly', 'sliding_window',
          'perpetual', 'fulcrum']


def options(opt):

    opt.load('wurf_common_tools')
    opt.load('python')


def resolve(ctx):

    import waflib.extras.wurf_dependency_resolve as resolve

    ctx.load('wurf_common_tools')

    ctx.add_dependency(resolve.ResolveVersion(
        name='waf-tools',
        git_repository='github.com/steinwurf/waf-tools.git',
        major=3))

    ctx.add_dependency(resolve.ResolveVersion(
        name='boost',
        git_repository='github.com/steinwurf/boost.git',
        major=2))

    ctx.add_dependency(resolve.ResolveVersion(
        name='kodo-core',
        git_repository='github.com/steinwurf/kodo-core.git',
        major=6))

    ctx.add_dependency(resolve.ResolveVersion(
        name='kodo-rlnc',
        git_repository='github.com/steinwurf/kodo-rlnc.git',
        major=5),
        optional=True)

    ctx.add_dependency(resolve.ResolveVersion(
        name='kodo-fulcrum',
        git_repository='github.com/steinwurf/kodo-fulcrum.git',
        major=5),
        optional=True)

    opts = ctx.opt.add_option_group('kodo-python options')

    opts.add_option(
        '--disable_rlnc', default=None, dest='disable_rlnc',
        action='store_true', help="Disable the basic RLNC codecs")

    opts.add_option(
        '--disable_fulcrum', default=None, dest='disable_fulcrum',
        action='store_true', help="Disable the Fulcrum RLNC codec")

    opts.add_option(
        '--enable_codecs', default=None, dest='enable_codecs',
        help="Enable the chosen codec or codecs, and disable all others. "
             "A comma-separated list of these values: {0}".format(codecs))


def configure(conf):

    conf.load("wurf_common_tools")

    conf.env['DEFINES_KODO_PYTHON_COMMON'] = []

    disabled_codec_groups = 0

    if conf.has_tool_option('disable_rlnc') or \
       not conf.has_dependency_path('kodo-rlnc'):
        conf.env['DEFINES_KODO_PYTHON_COMMON'] += ['KODO_PYTHON_DISABLE_RLNC']
        disabled_codec_groups += 1
    if conf.has_tool_option('disable_fulcrum') or \
       not conf.has_dependency_path('kodo-fulcrum'):
        conf.env['DEFINES_KODO_PYTHON_COMMON'] += \
            ['KODO_PYTHON_DISABLE_FULCRUM']
        disabled_codec_groups += 1

    if disabled_codec_groups == 2:
        conf.fatal('All codec groups are disabled or unavailable. Please make '
                   'sure that you enable at least one codec group and you '
                   'have access to the corresponding repositories!')

    if conf.has_tool_option('enable_codecs'):
        enabled = conf.get_tool_option('enable_codecs').split(',')

        # Validate the chosen codecs
        for codec in enabled:
            if codec not in codecs:
                conf.fatal('Invalid codec: "{0}". Please use the following '
                           'codec names: {1}'.format(codec, codecs))

        # Disable the codecs that were not selected
        for codec in codecs:
            if codec not in enabled:
                conf.env['DEFINES_KODO_PYTHON_COMMON'] += \
                    ['KODO_PYTHON_DISABLE_{0}'.format(codec.upper())]


def build(bld):

    # Ensure that Python was configured properly in the configure step of
    # the boost wscript (boost-python needs to be configured in the boost repo)
    if not bld.env['BUILD_PYTHON']:
        bld.fatal('Python was not configured properly')

    bld.load("wurf_common_tools")

    bld.env.append_unique(
        'DEFINES_STEINWURF_VERSION',
        'STEINWURF_KODO_PYTHON_VERSION="{}"'.format(VERSION))

    # Remove NDEBUG which is added from conf.check_python_headers
    flag_to_remove = 'NDEBUG'
    defines = ['DEFINES_PYEMBED', 'DEFINES_PYEXT']
    for define in defines:
        while(flag_to_remove in bld.env[define]):
            bld.env[define].remove(flag_to_remove)

    bld.env['CFLAGS_PYEXT'] = []
    bld.env['CXXFLAGS_PYEXT'] = []

    CXX = bld.env.get_flat("CXX")
    # Matches both /usr/bin/g++ and /user/bin/clang++
    if 'g++' in CXX or 'clang' in CXX:
        bld.env.append_value('CXXFLAGS', '-fPIC')

    bld.recurse('src/kodo_python')


@feature('pyext')
@after_method('apply_link')
def test_kodo_python(self):
    # Only execute the tests within the current project
    if self.path.is_child_of(self.bld.srcnode):
        if self.bld.has_tool_option('run_tests'):
            self.bld.add_post_fun(exec_test_python)


def exec_test_python(bld):
    python = bld.env['PYTHON'][0]
    env = dict(os.environ)
    env['PYTHONPATH'] = os.path.join(bld.out_dir, 'src', 'kodo_python')

    # First, run the unit tests in the 'test' folder
    if os.path.exists('test'):
        for f in sorted(os.listdir('test')):
            if f.endswith('.py'):
                test = os.path.join('test', f)
                bld.cmd_and_log('{0} {1}\n'.format(python, test), env=env)

    # Then run the examples in the 'examples' folder
    if os.path.exists('examples'):
        for f in sorted(os.listdir('examples')):
            if f.endswith('.py'):
                example = os.path.join('examples', f)
                bld.cmd_and_log(
                    '{0} {1} --dry-run\n'.format(python, example), env=env)
