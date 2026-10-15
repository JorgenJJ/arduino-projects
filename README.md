# arduino-projects

A collection of Arduino projects.

## Structure

```
index.html            # website (Docsify)
_sidebar.md           # website navigation
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
4. Rename `_template.ino` in the **Code** section of the `README.md`.
5. Add the project to [_sidebar.md](_sidebar.md).

## Website

The site uses [Docsify](https://docsify.js.org). It renders the Markdown files in the browser, so there is no build step.

- **Preview:** run `python -m http.server 3000` and open http://localhost:3000.
- **Publish:** in GitHub, go to **Settings → Pages**. Set **Source** to **Deploy from a branch**, branch `main`, folder `/ (root)`.

## Arduino IDE

Set **File → Preferences → Sketchbook location** to this repo. The IDE then lists all projects and finds shared code in `libraries/`.
