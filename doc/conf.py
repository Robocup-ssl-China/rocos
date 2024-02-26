# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

import os, sys

sys.path.append(os.path.abspath("./_ext"))

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

source_suffix = ['.rst', '.md']

project = 'Rocos'
copyright = '2024, Turing-zero'
author = 'Turing-zero'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    'bilibili',
    "myst_parser", 
    "sphinx_design", 
    'sphinx_copybutton',
    'sphinxcontrib.youtube',
    'sphinxcontrib.images',
    'comments', # fix sphinx-comments bug
]

comments_config = {
   "utterances": {
      "repo": "Robocup-ssl-China/rocos",
      "optional": "config",
   }
}

myst_enable_extensions = [
    "colon_fence",
    "attrs_inline",
]
myst_footnote_transition = False

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

language = 'zh_CN'
# html_logo = '_static/logo.png'
# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_material'
# import sphinx_pdj_theme
# html_theme = 'sphinx_pdj_theme'
# html_theme_path = [sphinx_pdj_theme.get_html_theme_path()]
html_show_sourcelink = False
html_theme_options = {

    # Set the name of the project to appear in the navigation.
    'nav_title': 'Rocos\'s Wiki!',

    # Set you GA account ID to enable tracking
    'google_analytics_account': 'G-XG5RNS0XG5',

    # Specify a base_url used to generate sitemap.xml. If not
    # specified, then no sitemap will be built.
    # 'base_url': 'https://project.github.io/project',

    # Set the color and the accent color
    'color_primary': '279BE6',
    # 'color_accent': '#279BE6',

    # Set the repo location to get a badge with stats
    'repo_url': 'https://github.com/Robocup-ssl-China/rocos',
    'repo_name': 'Rocos',

    'html_minify': True,
    'css_minify': True,
    'logo_icon': '&#xe869',
    # Visible levels of the global TOC; -1 means unlimited
    'globaltoc_depth': 1,
    # If False, expand all TOC entries
    'globaltoc_collapse': True,
    # If True, show hidden TOC entries
    'globaltoc_includehidden': False,
}
html_sidebars = {
    "**": ["logo-text.html", "globaltoc.html", "localtoc.html", "searchbox.html"]
}


html_static_path = ['_static']
