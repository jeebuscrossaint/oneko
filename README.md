# oneko
=====

The ever popular kitty-that-follows-your-mouse-pointer toy.


Neko is Japanese for cat. According to  [Wikipedia](https://en.wikipedia.org/wiki/Neko_(software)) it was originally written for the NEC PC-9801.

There are many many versions. Generally it is considered Public Domain software, so I am placing my version under the Unlicense.

This version is written in Python for Windows 10/11. It uses PyQt6 for the GUI, pillow in the build script to generate an ico file, and pyinstaller to generate an executable, and pywin to do windows related stuff if the user wishes.

At the time of this writing the code is known to work on python 3.10.11, though that shouldn't really matter given there is an executable.

The program creates a tray icon that can be used to hide/show the cat, and quit the program.

The image files are ancient public domain under the `gif` directory.
The Python code for the program (c) 2025 Amarnath Patel

If you want to build it from 'source' of course have python and pip installed.
```bash
git clone https://github.com/jeebuscrossaint/oneko.git
cd oneko
pip install -r requirements.txt
python build.py
```

It will generate the executable in the `dist` directory.

Note that the build script has a couple CLI arguments.
You can build it with no arguments, if you run it with `--install` it will install it locally to your windows user, if you run with --install --autostart it will install locally and automatically autostart the program on your user login, and of course you can always uninstall the program with python build.py --uninstall. You can of course also enable autostart of the program from the tray icon.

The code is incredibly self explanatory. If you want to change any settings you can simply edit the soruce code. Then build it again. The code in itself is only 260 lines long.

links
=====

[Download](https://github.com/jeebuscrossaint/oneko/releases)

[Author](https://github.com/jeebuscrossaint)
