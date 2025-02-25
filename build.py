from PIL import Image
import os
import subprocess
import sys

def create_ico():
    print("Creating icon file...")
    try:
        img = Image.open(os.path.join("gif", "25.GIF"))
        img = img.convert("RGBA")
        img.save("oneko.ico")
        print("Icon created successfully!")
        return True
    except Exception as e:
        print(f"Error creating icon: {e}")
        return False

def build_exe():
    print("Building executable...")
    try:
        cmd = [
            "pyinstaller",
            "--onefile",
            "--noconsole",
            "--add-data", "gif;gif",
            "--icon=oneko.ico",
            "oneko.py"
        ]
        subprocess.run(cmd, check=True)
        print("\nBuild successful! Executable can be found in the 'dist' folder.")
        return True
    except subprocess.CalledProcessError as e:
        print(f"Error building executable: {e}")
        return False
    except Exception as e:
        print(f"Unexpected error: {e}")
        return False

if __name__ == "__main__":
    print("Starting Oneko build process...")
    if create_ico() and build_exe():
        print("\nBuild completed successfully!")
        print("You can find your executable in the 'dist' folder.")
    else:
        print("\nBuild failed!")
        sys.exit(1)
