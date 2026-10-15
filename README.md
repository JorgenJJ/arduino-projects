# arduino-projects

A collection of Arduino projects.

## Structure

```
inventory.md          # all components available
projects/
  _template/          # copy this to start a new project
    _template.ino
    README.md
  <project-name>/
    <project-name>.ino  # folder and .ino name must match
    README.md           # components, wiring table
    wiring.jpg          # optional photo of the build
libraries/            # shared libraries (add when needed)
```

## New project

1. Copy `projects/_template` to `projects/<project-name>`.
2. Rename `_template.ino` to `<project-name>.ino`.
3. Fill in the project `README.md`. Pick components from [inventory.md](inventory.md).

## Arduino IDE

Set **File → Preferences → Sketchbook location** to this repo. The IDE then lists all projects and finds shared code in `libraries/`.
