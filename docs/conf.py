# -*- coding: utf-8 -*-

import os
import sys
from sphinx.errors import ConfigError

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.

kodo_dir = os.path.join("..", "build_current")

if not os.path.exists(kodo_dir):
    raise ConfigError(
        '\nUnable to find the path "{}"'.format(kodo_dir)
        + "\nDid you successfully build to kodo-python library?"
    )

sys.path.insert(0, os.path.abspath(kodo_dir))

# -- General configuration ----------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    "sphinx.ext.autodoc",
    "sphinx.ext.doctest",
    "sphinx.ext.intersphinx",
    "sphinx.ext.todo",
    "sphinx.ext.coverage",
    "sphinx.ext.mathjax",
    "sphinx.ext.ifconfig",
    "sphinx.ext.autosectionlabel",
    "guzzle_sphinx_theme",
]

# Add any paths that contain templates here, relative to this directory.
templates_path = []

# The suffix of source filenames.
source_suffix = ".rst"

# The master toctree document.
master_doc = "index"

# General information about the project.
project = u"Kodo Python"
copyright = u"2020, Steinwurf"
author = u"Steinwurf"

# The version info for the project you're documenting, acts as replacement for
# |version| and |release|, also used in various other places throughout the
# built documents.
#
# The short X.Y version.
version = u""
# The full version, including alpha/beta/rc tags.
release = u""

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This patterns also effect to html_static_path and html_extra_path
# exclude_patterns = []

# The name of the Pygments (syntax highlighting) style to use.
pygments_style = "sphinx"

# -- Options for HTML output --------------------------------------------------

try:
    import guzzle_sphinx_theme

    html_theme = "guzzle_sphinx_theme"
    html_theme_path = guzzle_sphinx_theme.html_theme_path()
except ImportError:
    print(
        "Unable to import the used theme.\n"
        "Please install requirements.txt before building"
    )
    pass

html_sidebars = {
    "**": ["logo.html", "logo-text.html", "globaltoc.html", "searchbox.html"]
}

# Theme options are theme-specific and customize the look and feel of a theme
# further.  For a list of options available for each theme, see the
# documentation.
#
html_theme_options = {
    "h1_background_color": "a771b91A",
    "sidebar_hover_color": "a771b9aA",
    "logo_text_background_color": "a771b9",
    "link_color": "a771b9",
    "link_hover_color": "785185",
    "code_color": "6f16ab",
    "target_highlight_color": "a771b9",
    "highlighted_color": "d6afe3",
    "search_form_focus_color": "d6afe3",
}

# The name of an image file (relative to this directory) to place at the top
# of the sidebar.
html_logo = "images/icon.svg"

# The name of an image file (relative to this directory) to use as a favicon of
# the docs.  This file should be a Windows icon file (.ico) being 16x16 or 32x32
# pixels large.
html_favicon = "images/icon.ico"

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = []

# If true, prefix each section label with the name of the document it is in,
# followed by a colon.
autosectionlabel_prefix_document = True
