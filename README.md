# vscope
*Command Line Data Visualizer*

vscope is a small data visualizer for safe and quick cassification of unknown datasets. It generates a 2D histogram, which is specific to the internal structure of the input data and displays it right inside the terminal.

## Usage
```
Usage: cat [FILE] | ./vscope [OPTION]...

Visualizes STDIN by writing a 2D histogram to STDOUT.
Useful for classifing unknown datasets.

Options:
  -h, --help         Shows this message and exits.
  -s, --size=NUM     Changes the size of the heatmap. If this flag is not set,
                     the map will be sized to fit the terminal window.
  -b, --bytes=NUM    Only reads the first NUM bytes from STDIN.
  -c, --color        Colors the heatmap using ANSI escape codes. This might
                     not look as intendet, depending on your terminal.
  -g, --gray         Enables a gray-scale version of --color. This flag
                     cannot be set in combination with --color.
  -p, --picture      Write pgm image data directly to STDOUT.
  -t, --thold=[NUM]  Checks if the value is higher than the threshold and
                     outputs either 00 or ff. If no NUM is defined, the
                     threshold will be set to 0.
```

## Example
This is what you get, when you feed the source code of the visualizer into the visualizer itself:
```
$ cat vscope.c | ./vscope -s 32
----------------------------------------------------------------
--ff----0f1a--48--------01----30--------------------------------
----------------------------------------------------------------
----------------------------------------------------------------
--16----a5610b400106--035b404b17--------------------------------
--01----522c1b010402031b481c3e08--------------------------------
--------1e0a1214----0106------05--------------------------------
--------36341014------011a0e0404--------------------------------
--------060401--06040905----------------------------------------
--------08----020d0c0906----------------------------------------
--------0e01----02120e01----------------------------------------
--------080c06020502060108131f02--------------------------------
--1f----54290401--02--015a93ac23--------------------------------
--22----352301010403042c92b47804--------------------------------
--19----5527--08--0104019b906505--------------------------------
--30----13--06--------0613210308--------------------------------
----------------------------------------------------------------
----------------------------------------------------------------
----------------------------------------------------------------
----------------------------------------------------------------
----------------------------------------------------------------
----------------------------------------------------------------
----------------------------------------------------------------
----------------------------------------------------------------
----------------------------------------------------------------
----------------------------------------------------------------
----------------------------------------------------------------
----------------------------------------------------------------
----------------------------------------------------------------
----------------------------------------------------------------
----------------------------------------------------------------
----------------------------------------------------------------
```

## Build
I included a makefile, so this part shouldn't be too difficult.

Compile the program:
```
make
```
Install the program:
```
make install
```

## Extra Credit
This project is inspired by the works of Christopher Domas: https://github.com/xoreaxeaxeax