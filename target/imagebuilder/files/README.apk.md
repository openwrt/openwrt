# ./packages folder

Add `.apk` packages to this folder will allow the ImageBuilder to install them.

For more complex setups consider adding a custom feed containing packages.

    file:///path/to/Packages.adb

Whenever the ImageBuilder builds a firmware image this folder will be reloaded
and a new package index created.
