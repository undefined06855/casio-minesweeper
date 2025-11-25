# Mini Minesweeper

Minesweeper for the CASIO FX-CG50/Prizm (and similar calculators - if it can take a .g3a it can probably run it!)

Press F1 to flag and F2 to reveal the cell.

## Installation

Take the .g3a file from the releases panel on the right (if I remember to upload it)

## Compilation

See [the PrizmSDK setup guide](https://prizm.cemetech.net/Tutorials/PrizmSDK_Setup_Guide/), then clone this repo into the `projects` folder, and run the `make.bat`

For VSCode using clangd and the Makefiles extension, I would recommend adding a `.vscode/settings.json` file, containing the following:
```json
{
    "makefile.makePath": "path/to/PrizmSDK-win-0.6/bin",
}
```
...and then adding a `.clangd` file with
```yaml
CompileFlags:
  Add: [
    "-Ipath/to/PrizmSDK-win-0.6/include"
  ]
```
to get proper syntax highlighting, and be able to Ctrl+Shift+P -> Makefile: Build the current target


god i hate this 5 year old half-broken sdk (calling sys_calloc crashes??) with no support apart from ancient forum threads!
