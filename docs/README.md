# Hyprland Workspace Indicator

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg?style=plastic)](https://www.gnu.org/licenses/gpl-3.0) ![C](https://img.shields.io/badge/c-%2300599C.svg?style=plastic&logo=c&logoColor=white) ![SASS](https://img.shields.io/badge/SASS-hotpink.svg?style=plastic&logo=SASS&logoColor=white) ![CMake](https://img.shields.io/badge/CMake-%23008FBA.svg?style=plastic&logo=cmake&logoColor=white) ![GitHub Actions](https://img.shields.io/badge/github%20actions-%232671E5.svg?style=plastic&logo=githubactions&logoColor=white) [![GitHub issues-open](https://img.shields.io/github/issues-open/Jayden876212/hyprland-workspace-indicator.svg)](https://GitHub.com/Jayden876212/hyprland-workspace-indicator/issues?q=is%3Aissue+is%3Aopen) [![GitHub open pull-requests](https://img.shields.io/github/issues-pr/Jayden876212/hyprland-workspace-indicator.svg?style=plastic)](https://github.com/Jayden876212/hyprland-workspace-indicator/pull/) [![GitHub latest commit](https://badgen.net/github/last-commit/Jayden876212/hyprland-workspace-indicator)](https://GitHub.com/Jayden876212/hyprland-workspace-indicator/commit/)

## About

![Animated demonstration](demonstration.gif)

A multi-monitor workspace indicator for [Hyprland](https://github.com/hyprwm/Hyprland) that can be integrated with [EWW](https://github.com/elkowar/eww) to display available workspaces.

It is super lightweight and written in the C programming language. It supports up to 10 workspaces [1,2,3,4,5,6,7,8,9,0] (although more are planned in the future).

The key feature is the ability to see individual workspaces on each monitor respectively, reducing clutter and increasing readability. For example, you might see one numbered workspace (4) on one monitor, and 3 other numbered workspaces (7,8, and 9) on another monitor.

## Integrating with the compositor (Hyprland)

The program will not work with any wayland compositors that aren't Hyprland. The program produces a JSON output for each monitor like this:

```JSON
[
  {
    "activeworkspaces": [
      false,
      false,
      false,
      false,
      false,
      false,
      false,
      false,
      false,
      false
    ],
    "workspaces": [
      false,
      false,
      false,
      false,
      false,
      true,
      true,
      false,
      true,
      false
    ]
  }
]
```

For each monitor, this JSON data is stored in an array as each index, where the index is the monitor ID assigned to by your Hyprland configuration.

The data is outputted automatically when a workspace-related event is triggered.

You can look at the [following section](README.md#running-with-an-example-widget) for an example of how to implement the `hyprland-workspace-indicator`.

## Running with an example widget

The example widget, script, and other configuration files are located in [`../example/.config/eww`](example) to show you what an implementation would look like.

When `hyprland-workspaces` is installed to your `$PATH`, EWW should be able to execute it in a script:

`.config/eww/scripts/workspaces.sh`

```sh
#! /bin/sh

hyprland-workspaces
```

EWW should import this script as a listener:

`.config/eww/variables.yuck`

```yuck
(deflisten workspaces 'scripts/workspaces.sh')
```

A working widget to implement this script looks something like this:

`.config/eww/widgets/example-widget.yuck`

```yuck
(defwidget workspaceicon [monitor workspace]
  (revealer :reveal "${workspaces[monitor].workspaces[workspace]}" :transition "slideright" :duration "500ms"
    (overlay :visible "${workspaces[monitor].workspaces[workspace]}"
      (box :class "${workspaces[monitor].workspaces[workspace] ? "inactive-circle-indicator-margin" : "inactive-circle-indicator"}"
           :width 40
           :spacing 0
        (label :text "${workspace}"))
      (revealer :reveal "${workspaces[monitor].activeworkspaces[workspace]}"
                :transition "crossfade"
        (box :width 40 :spacing 0 :class "${workspaces[monitor].workspaces[workspace] ? "active-circle-indicator-margin" : "active-circle-indicator"}"
          (label :text "${workspace}"))))))

(defwidget workspaceindicator [monitor]
  (box :class "container"
       :halign "start"
       :width 20
       :space-evenly "false"
       :spacing 0
       :orientation "h"
    (for workspace in "[1,2,3,4,5,6,7,8,9,0]"
      (workspaceicon :monitor monitor
                     :workspace workspace))))
```

The widget is then placed in a window:

`.config/eww/windows/example-window.yuck`

```yuck
(include "./widgets/example-widget.yuck")

(defwindow example-window
           :monitor 0
           :geometry (geometry :x "0%"
                               :y "10px"
                               :width "99%"
                               :height "30px"
                               :anchor "top center")
           :stacking "fg"
           :exclusive true
           :reserve (struts :distance "40px" :side "top")
           :windowtype "dock"
           :wm-ignore false
  (box
    (workspaceindicator :monitor 0)
  )
)
```

## Contributing

To contribute, there is a [*contributing guide*](CONTRIBUTING.md).

## Developing

To guide developers, there is a [*developer handbook*](DEVELOPER.md).

## License

This project is developed under the [GNU GPL v3 License](https://www.gnu.org/licenses/gpl-3.0).
