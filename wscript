#! /usr/bin/env python
# encoding: utf-8

import os

APPNAME = 'kodo-python'
VERSION = '12.0.0'

codecs = ['nocode', 'rlnc', 'perpetual', 'fulcrum']


def options(opt):

    opt.load('python')

    opts = opt.add_option_group('kodo-python options')

    opts.add_option(
        '--disable_rlnc', default=None, dest='disable_rlnc',
        action='store_true', help="Disable the basic RLNC codecs")

    opts.add_option(
        '--disable_perpetual', default=None, dest='disable_perpetual',
        action='store_true', help="Disable the Perpetual RLNC codecs")

    opts.add_option(
        '--disable_fulcrum', default=None, dest='disable_fulcrum',
        action='store_true', help="Disable the Fulcrum RLNC codec")

    opts.add_option(
        '--enable_codecs', default=None, dest='enable_codecs',
        help="Enable the chosen codec or codecs, and disable all others. "
             "A comma-separated list of these values: {0}".format(codecs))


def configure(conf):

    conf.env['DEFINES_KODO_PYTHON_COMMON'] = []

    disabled_codec_groups = 0

    if conf.has_tool_option('disable_rlnc') or \
       not conf.has_dependency_path('kodo-rlnc'):
        conf.env['DEFINES_KODO_PYTHON_COMMON'] += ['KODO_PYTHON_DISABLE_RLNC']
        disabled_codec_groups += 1
    if conf.has_tool_option('disable_perpetual') or \
       not conf.has_dependency_path('kodo-perpetual'):
        conf.env['DEFINES_KODO_PYTHON_COMMON'] += \
            ['KODO_PYTHON_DISABLE_PERPETUAL']
        disabled_codec_groups += 1
    if conf.has_tool_option('disable_fulcrum') or \
       not conf.has_dependency_path('kodo-fulcrum'):
        conf.env['DEFINES_KODO_PYTHON_COMMON'] += \
            ['KODO_PYTHON_DISABLE_FULCRUM']
        disabled_codec_groups += 1

    if disabled_codec_groups == 3:
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

    extra_linkflags = []
    CXX = bld.env.get_flat("CXX")
    if 'g++' in CXX or 'clang' in CXX:
        bld.env.append_value('CXXFLAGS', '-fPIC')
    # Matches MSVC
    if 'CL.exe' in CXX or 'cl.exe' in CXX:
        extra_linkflags = ['/EXPORT:initkodo']

    bld(features='cxx cxxshlib pyext',
        source=bld.path.ant_glob('src/kodo_python/**/*.cpp'),
        target='kodo',
        name='kodo-python',
        linkflags=extra_linkflags,
        use=[
            'STEINWURF_VERSION',
            'KODO_PYTHON_COMMON',
            'boost_includes',
            'boost_python',
            'kodo_core',
            'kodo_rlnc',
            'kodo_perpetual',
            'kodo_fulcrum'
        ]
    )

    if bld.is_toplevel():
        if bld.has_tool_option('run_tests'):
            bld.add_post_fun(exec_test_python)


def exec_test_python(bld):
    python = bld.env['PYTHON'][0]
    env = dict(os.environ)
    env['PYTHONPATH'] = os.path.join(bld.out_dir)

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
                print('-------------------------------\n')
