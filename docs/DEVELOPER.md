# Hyprland Workspace Indicator - Developer Handbook 

## Overview

This is a developer handbook that will guide developers with:

- Building
- Architecture
- Standards & Conventions
- Designs
- Testing

## Contributing

To contribute, there is a [*contributing guide*](CONTRIBUTING.md).

## How to build and install (eww)

Build requirements:

- cJSON
- Make

1. Run `make`
2. Edit the workspaces_script in `bin/` to the path of the binary (which is produced in the `bin/` directory by default).
3. Set a `deflisten` variable in eww with the workspaces_script as the input.
4. Create a `defwidget` that uses that variable for `:reveal` or `:visible` as seen by the example code in `example/`.
5. Implement the widget in a `defwindow`, like your eww bar for example.
