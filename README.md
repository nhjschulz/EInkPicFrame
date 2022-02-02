# EInkPicFrame (Work In Progress)

A low power battery driven picture frame with a 7 color E-Ink display.

![Frame One](design/history/frame1.png)

This is a hobbyist project. I have >20 years background in software development,
but lack the electronic engineering background. I'm using this project to
clear the fog in these mysterious regions below a HAL software layer. The project
therefore includes a hardware design as well. Keep this in mind if you plan to
rebuild it. As with any open source, you use this AT YOUR OWN RISK!.
## Requirements

A set of project requirements is collected here: [Requirements](design/Requirements.md)

## Concept

Block diagram of the system:

![Context](http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/nhjschulz/EInkPicFrame/master/design/plantuml/SwContext.plantuml)

## Used Tools

The project is build using open source tools only. All of them are available for free. The following table shows the tools and versions used during development:

|Tool                  |   Usage                   | Version   | Link                                                           |
|----------------------|---------------------------|-----------|---------------------------------------------------------------------|
| Visual Studio Code   | Editor and IDE            | 1.63.2    | [https://code.visualstudio.com/](https://code.visualstudio.com/)   |
| PlatformIO           | Softwate bUild tools      | 5.2.4     | [https://platformio.org/](https://platformio.org/)          |
| Python               | Script programming        | 3.10.1    | [https://www.python.org/](https://www.python.org/)                 |
| GIMP                 | Image Manipulation        | 2.10.30   | [https://www.gimp.org/](https://www.gimp.org/)                     |
| GIT                  | Version Control           | 2.19      | [https://git-scm.com/](https://git-scm.com/)                       |
| KiCad EDA            | Hardware Design           | 5.1.12    | [https://www.kicad.org//](https://www.kicad.org/)                  |

## Software Architecture

The implemented software architecture design is shown on the [Software Architecture](design/SwArchitecture.md) page.

## Hardware

The system is assembled using the following parts: [Partlist](design/Parts.md).

Pin assignment for the microcontroller is defined here: [Pin Usage](design/Pins.md).

The PCB was designed using [KiCAD](https://www.kicad.org/). The design files are in the hw folder.

## Prototype Progress

* 2021-12-11: System functional on breadboard (V1 hardware)
  ![breadboard](design/history/breadboard_v1.png)

* 2021-01-22: V1 PCBs arrived and work
  ![PCBv1](design/history/pcb_v1.png)

* 2022-02-01: First frame starts long run test in the living room
  ![Frame One](design/history/frame1.png)

  Back side:

  ![Back](design/history/frameBG.png)