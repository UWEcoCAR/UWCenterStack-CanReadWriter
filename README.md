UWCenterStack-CanReadWriter
============================

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

#### Publishing
To publish, setup credentials with (using the credentials from the Google Doc):
```
uwcs-crw && npm adduser
```
Then update the version number in `package.json` and run:

```
uwcs-crw-publish
```
