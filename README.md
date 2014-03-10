UWCenterStack-CanReadWriter
============================
NOTE: This repository only builds on Ubuntu that has Kvaser canlib installed and
is designed for use with the Kvaser usbCAN II HS/SWC module.

It is assumed you have node, npm, and kvaser canlib. If not, do that first: (for Ubuntu 13.10)
```
sudo apt-get install nodejs
sudo apt-get install npm
sudo ln -s /usr/bin/nodejs /usr/bin/node
```
Instructions for installing Kvaser canlib: http://www.kvaser.com/support/knowledge-base/?knowledge_base_category=getting-started#/driver-and-hardware-installation-for-linux

## Installation
```
npm install uwcenterstack-canreadwriter
```

## Developer setup
Add to `~/.bash_profile` (Mac) or `~/.bashrc` (Linux):
```
export UWCENTERSTACK_CANREADWRITER_HOME=path/to/UWCenterStack-CanReadWriter <-- REPLACE WITH CORRECT PATH!!!
source $UWCENTERSTACK_CANREADWRITER_HOME/shellCommands.sh
```

#### Compiling
To recompile the c++, run:
```
uwcs-crw-rebuild
```


#### Publishing
To publish, setup credentials with (using the credentials from the Google Doc):
```
uwcs-crw && npm adduser
```
Then update the version number in `package.json` and run:

```
uwcs-crw-publish
```
