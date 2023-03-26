# OVERVIEW

# TODO
still need a purge function
and a dependencies manager
future release will require RSA/JWT to upload to registry.
Registry will exist on remote server
Registry will also need environments and more advanced version control.
Need to distinguish private and public source code. (SFTP)

This is a package manager to help to power the Jericho Distributed System.
This package manager is currently only for unix based systems

### Installation

After cloning the repo, run ```sudo ./bin/install``` script
This will install the necessary binaries and setup up the directories
needed for comet to manage your packages.

Upon installation running ```sudo comet init``` will set up your environment.

To install a package a script must first be uploaded with the package name
and the name of the user uploading 

For more info on commands enter ```comet help```

### Commands

```
================================================================================
                            Comet Package Manager
================================================================================
install                            Install a package within the registry
uninstall                          Uninstall a package within the registry
list                               List all packages with detailed information
upload <script> <user>             Upload a package to the registry with API token
show <package_name>                Get detailed information about a package
update <package_name> <new_script> Update package with a new script
script <package_name>              For debug purposes, get the ascii bash script
load                               Reads package names from a comet.txt file to install required packages
```