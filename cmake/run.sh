#!/bin/bash

cmake --version >/dev/null 2>&1 || {
    if [ -f /Applications/CMake.app/Contents/bin/cmake-gui ]; then
         echo "CMake doesn't added to PATH. Fixing..."
         sudo "/Applications/CMake.app/Contents/bin/cmake-gui" --install
		 echo "Rerun this script again, please."
    else
         echo "CMake doesn't installed."
    fi
    exit 1;
}

cd "$(dirname "$BASH_SOURCE")"

python "run.py" --platform macosx_xcode --build-path "../build/mac" --source-path "../.."
