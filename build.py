from PIL import Image
import os
import subprocess
import sys
from pathlib import Path
import winreg
import shutil
import ctypes
import argparse
import platform

def is_admin():
    try:
        return ctypes.windll.shell32.IsUserAnAdmin()
    except:
        return False

def create_ico():
    print("Creating icon file...")
    try:
        img = Image.open(os.path.join("gif", "25.GIF"))  # Use sitting cat for icon
        # Convert to RGBA if not already
        img = img.convert("RGBA")
        # Create icon in multiple sizes for better display
        icon_sizes = [(16,16), (24,24), (32,32), (48,48), (64,64), (128,128)]
        img.save("oneko.ico", sizes=icon_sizes)
        print("Icon created successfully!")
        return True
    except Exception as e:
        print(f"Error creating icon: {e}")
        return False

def build_exe():
    print("Building executable...")
    try:
        # Clean up previous build
        for path in ['build', 'dist']:
            if os.path.exists(path):
                shutil.rmtree(path)

        cmd = [
            "pyinstaller",
            "--onefile",
            "--noconsole",
            "--add-data", f"gif{os.pathsep}gif",
            "--add-data", f"oneko.ico{os.pathsep}.",  # Add ico file to root of bundle
            "--icon=oneko.ico",
            "oneko.py"
        ]
        subprocess.run(cmd, check=True)

        # Verify the build
        exe_path = os.path.join("dist", "oneko.exe")
        if not os.path.exists(exe_path):
            print("Error: Executable not created!")
            return False

        print("\nBuild successful! Executable can be found in the 'dist' folder.")
        return True
    except Exception as e:
        print(f"Error building executable: {e}")
        return False

def create_shortcut_in_startup(exe_path):
    try:
        startup_path = Path(os.path.expandvars('%APPDATA%\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\oneko.bat'))
        with open(startup_path, 'w') as f:
            f.write(f'start "" "{exe_path}"')
        print(f"Created startup shortcut")
        return True
    except Exception as e:
        print(f"Error creating startup shortcut: {e}")
        return False

def install_program(exe_path):
    try:
        # Get paths
        program_files = os.path.expandvars('%LOCALAPPDATA%\\Programs\\Oneko')
        start_menu = os.path.expandvars('%APPDATA%\\Microsoft\\Windows\\Start Menu\\Programs\\Oneko')

        # Create directories
        Path(program_files).mkdir(parents=True, exist_ok=True)
        Path(start_menu).mkdir(parents=True, exist_ok=True)

        # Copy executable and resources
        shutil.copy2(exe_path, os.path.join(program_files, 'oneko.exe'))

        # Create Start Menu shortcut
        with winreg.CreateKey(winreg.HKEY_CURRENT_USER,
                            "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\oneko.exe") as key:
            winreg.SetValue(key, "", winreg.REG_SZ, os.path.join(program_files, 'oneko.exe'))

        # Create uninstall entry
        with winreg.CreateKey(winreg.HKEY_CURRENT_USER,
                            "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Oneko") as key:
            winreg.SetValueEx(key, "DisplayName", 0, winreg.REG_SZ, "Oneko")
            winreg.SetValueEx(key, "UninstallString", 0, winreg.REG_SZ,
                            f'"{os.path.join(program_files, "oneko.exe")}" --uninstall')
            winreg.SetValueEx(key, "DisplayIcon", 0, winreg.REG_SZ,
                            os.path.join(program_files, "oneko.exe"))
            winreg.SetValueEx(key, "Publisher", 0, winreg.REG_SZ, "Oneko")
            winreg.SetValueEx(key, "NoModify", 0, winreg.REG_DWORD, 1)
            winreg.SetValueEx(key, "NoRepair", 0, winreg.REG_DWORD, 1)

        # Create shortcut in Start Menu
        create_shortcut(os.path.join(program_files, 'oneko.exe'),
                       os.path.join(start_menu, 'Oneko.lnk'))

        print("Installation completed successfully!")
        return True
    except Exception as e:
        print(f"Error during installation: {e}")
        return False

def uninstall_program():
    try:
        # Remove program files
        program_files = os.path.expandvars('%LOCALAPPDATA%\\Programs\\Oneko')
        start_menu = os.path.expandvars('%APPDATA%\\Microsoft\\Windows\\Start Menu\\Programs\\Oneko')

        if Path(program_files).exists():
            shutil.rmtree(program_files)
        if Path(start_menu).exists():
            shutil.rmtree(start_menu)

        # Remove registry entries
        try:
            winreg.DeleteKey(winreg.HKEY_CURRENT_USER,
                           "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\oneko.exe")
        except WindowsError:
            pass

        try:
            winreg.DeleteKey(winreg.HKEY_CURRENT_USER,
                           "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Oneko")
        except WindowsError:
            pass

        # Remove startup entry if exists
        startup_path = Path(os.path.expandvars('%APPDATA%\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\oneko.bat'))
        if startup_path.exists():
            startup_path.unlink()

        print("Uninstallation completed successfully!")
        return True
    except Exception as e:
        print(f"Error during uninstallation: {e}")
        return False

def create_shortcut(target, shortcut_path):
    import pythoncom
    from win32com.client import Dispatch

    shell = Dispatch('WScript.Shell')
    shortcut = shell.CreateShortCut(shortcut_path)
    shortcut.Targetpath = target
    shortcut.save()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Build and install Oneko desktop pet')
    parser.add_argument('--install', action='store_true', help='Install Oneko globally')
    parser.add_argument('--uninstall', action='store_true', help='Uninstall Oneko')
    parser.add_argument('--autostart', action='store_true', help='Setup autostart on Windows login')
    args = parser.parse_args()

    if args.uninstall:
        if not is_admin() and platform.system() == 'Windows':
            ctypes.windll.shell32.ShellExecuteW(None, "runas", sys.executable, " ".join(sys.argv), None, 1)
            sys.exit()
        uninstall_program()
        sys.exit(0)

    # Normal build process
    if create_ico() and build_exe():
        print("\nBuild completed successfully!")

        if args.install:
            if not is_admin() and platform.system() == 'Windows':
                ctypes.windll.shell32.ShellExecuteW(None, "runas", sys.executable, " ".join(sys.argv), None, 1)
                sys.exit()
            exe_path = Path('dist/oneko.exe').absolute()
            if install_program(exe_path):
                print("Installation completed!")
                if args.autostart:
                    print("Setting up autostart...")
                    create_shortcut_in_startup(exe_path)
        else:
            print("You can find your executable in the 'dist' folder.")
