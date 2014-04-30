# Include npm bin to path. This may not be necessary, but sometimes is an issue.
export PATH=$PATH:/usr/local/share/npm/bin

# uwcs-crw - Changes the current directory to the UWCenterStack-CanReadWriter repo directory
alias uwcs-crw='cd $UWCENTERSTACK_CANREADWRITER_HOME'

# uwcs-crw-init - Downloads dependencies and rebuilds module
alias uwcs-crw-init='uwcs-crw && npm install'

# uwcs-crw-build - Recompiles the C++
alias uwcs-crw-rebuild='uwcs-crw && npm build .'

# uwcs-crw-test - Runs the test file
alias uwcs-crw-test='uwcs-crw && node test.js'

# uwcs-crw-publish - Pushes the package to npm
alias uwcs-crw-publish='uwcs-crw && npm version patch && npm publish'
