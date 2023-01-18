# WinGameAlpha Texture Editor

## Settings
There must be a file named `settings.wgas` in the immediate directory of execution (note that if using a command line to spawn the application, this may not be the same as the directory the executable is in) which contains information in the following format
```
colour: 0xff0000, 0x00ff00, 0x0000ff
size: 5
```
the `colour` tag is followed by up to 9 hex codes which will be binded to the keyboard keys 1-9 with 0 being the alpha toggle i.e. the eraser.
the `size` tag is the width and height of each 'pixel' in relation to a percentage of the window height (this must be at least 1). Note that this size will override any saved size within a .wgat file, to use the size within a loaded texture file set the `size` field to 0.

## Using the editor
You can draw within the darker gray area (with the exception of some small border which is dependant on how well the pixel size divides).

By default the active colour is black, to switch to your imported colours use the keyboard keys 1-9 and 0 for erasing. The indicator in the top left will display the current colour you have selected.

To toggle the dot grid press `G` on your keyboard. This is for drawing assistance and will not be saved with the texture.

To save the work as a .wgat file, press `Ctrl+S` and a submatrix will be cropped from the canvas and saved as `texture.wgat`, you are free to rename this file.

When the editor is opened it will search its directory and subdirectories for .wgat files and it will load the first one it finds into the editor and save it to the same file on `Ctrl+S`. Note that this implies only one file can be in the directory at a time, it is recommended to store made textures in a subfolder and bring them out to the parent folder where the executable is located to edit.

The width and height of the saved texture will display to the console when saving along with any errors, warnings or notices. Ideally start the application from a terminal. (Terminal will not be spawned if exe is double clicked)

Important Note: When erasing rapidly its possible the submatrix boundaries are not correctly adjusted and the matrix will save with alot of extra transparent space, this is simply fixed by clicking again in the area of the white space and shouldn't be a significant bug but keep an eye on the save size (in the terminal)


