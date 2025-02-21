from PIL import Image
import os

def create_spritesheet(gif_folder):
    print(f"Looking for GIFs in: {os.path.abspath(gif_folder)}")  # Show full path
    # Get all GIF files
    gif_files = sorted([f for f in os.listdir(gif_folder) if f.upper().endswith('.GIF')],
                          key=lambda x: int(x.split('.')[0]))

    # Load all frames from all GIFs
    frames = []
    frame_info = {}  # Store animation info for each state
    current_y = 0

    for gif_name in sorted(gif_files):
        gif_path = os.path.join(gif_folder, gif_name)
        gif = Image.open(gif_path)

        # Get all frames from this GIF
        state_frames = []
        try:
            while True:
                state_frames.append(gif.copy())
                gif.seek(gif.tell() + 1)
        except EOFError:
            pass

        # Store animation info
        state_name = gif_name[:-4]  # remove .gif
        frame_info[state_name] = {
            'y_offset': current_y,
            'frame_count': len(state_frames)
        }

        frames.extend(state_frames)
        current_y += 1

    # Get frame dimensions (assuming all frames are the same size)
    frame_width, frame_height = frames[0].size

    # Create the sprite sheet
    sheet_width = frame_width * max(len(f) for f in frame_info.values())
    sheet_height = frame_height * len(frame_info)
    spritesheet = Image.new('RGBA', (sheet_width, sheet_height), (0, 0, 0, 0))

    # Place frames on the sprite sheet
    current_frame = 0
    for state, info in frame_info.items():
        for i in range(info['frame_count']):
            x = i * frame_width
            y = info['y_offset'] * frame_height
            spritesheet.paste(frames[current_frame], (x, y))
            current_frame += 1

    return spritesheet, frame_info

def save_header_file(spritesheet, frame_info):
    # Save sprite sheet as PNG temporarily
    spritesheet.save('spritesheet.png')

    # Convert PNG to header file using xxd
    os.system('xxd -i spritesheet.png > spritesheet.h')

    # Generate frame info header
    with open('frame_info.h', 'w') as f:
        f.write('#pragma once\n\n')
        f.write('struct FrameInfo {\n')
        f.write('    int y_offset;\n')
        f.write('    int frame_count;\n')
        f.write('};\n\n')

        # Write frame info as C++ code
        for state, info in frame_info.items():
            f.write(f'const FrameInfo frame_{state}_info = {{ '
                f'{info["y_offset"]}, {info["frame_count"]} }};\n')

    # Clean up temporary PNG
    os.remove('spritesheet.png')

if __name__ == '__main__':
    spritesheet, frame_info = create_spritesheet('gif')
    save_header_file(spritesheet, frame_info)

    # Save a copy of the sprite sheet for reference
    spritesheet.save('spritesheet_reference.png')
