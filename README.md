# arduino-projects

A collection of Arduino projects.

## Structure

```
projects/
  _template/          # copy this to start a new project
    _template.ino
    README.md
  <project-name>/
    <project-name>.ino  # folder and .ino name must match
    README.md
libraries/            # shared libraries (add when needed)
```

## New project

1. Copy `projects/_template` to `projects/<project-name>`.
2. Rename `_template.ino` to `<project-name>.ino`.
3. Fill in the project `README.md`.

## Arduino IDE

Set **File → Preferences → Sketchbook location** to this repo. The IDE then lists all projects and finds shared code in `libraries/`.
