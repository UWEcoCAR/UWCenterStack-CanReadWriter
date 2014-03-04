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

To publish, setup credentials with (using the credentials from the Google Doc):
```
uwcs-crw && npm adduser
```

and then run

```
uwcs-crw-publish
```
