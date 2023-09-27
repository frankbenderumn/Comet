# Overview

Welcome to comet. Comet aims to provide a package manager for the Jericho
distributed system framework. Similar to the ruby gem package manager, 
apt-get and python's pipenv this package manager is designed for c++ CMAKE
based installations with support for other installation programs like pkg
and autoconf. 

Slowly but surely I am working toward a
cross-platform package manager with enhanced cli options for dealing with
various environments, distributed systems, cloud delivery networks, and 
database manipulation. A primary focus of comet is maintaining private
repo security while also allowing for the modification of source code of
various packages.

# Requirements

Currently, this library has been tested for Ubuntu-based systems.
Arm-based Macbooks will be the next system environment tested
with Windows support to come eventually.

# Features
- multiple environment management
- easy installation of package codebases built from source
- database support for Postgres and MongoDB
- comet.lock files for batch installation support
- token management to preserve private repo and codebases
- useful cli with help command output to assist troubleshooting

# Getting Started

After installing the repo navigate to the root level repo directory and run:
```
sudo ./install
```
You will be prompted on where you want to install the base comet repo.
The default directory is `/usr/local/comet` which is the recommended install
location.

Once the script finishes, run ```comet help``` for further assistance.

To get started installing packages just type comet install.

A list of available packages will be viewable with ```comet manifest```.
This feature is not currently implemented will be available shortly.