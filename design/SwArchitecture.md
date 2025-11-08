# Software Architecture

This page defines the software architecture of the EPaperFrame.

## Software Context

The software is developed in C/C++ and runs on the AVR328P micro controller. The AVR was chosen because of its easy to solder DIP packaging and builtin power saving modes. The limited amount of RAM (2Kb!) was also an interesting extra SW challenge, given that a single SD-CARD block already needs 25% of it.

The following picture shows the context of the software. The file system and display is controlled using a single SPI interface and two MOSFETs to disable their power in sleep.

The other interfaces are used for flashing (ISP) and debugging (LED, UART) purposes only.

![Context](http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/nhjschulz/EInkPicFrame/master/design/plantuml/SwContext.plantuml)

## System State Maschine

The application is a small state machine which toggles between updating the display and sleeping to save power.

![Context](http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/nhjschulz/EInkPicFrame/master/design/plantuml/StateMachine.plantuml)

## Class Diagram

A typical embedded 3 Layer Architecture is used in this project. Dependencies only go down to lower layers.

![Context](http://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.githubusercontent.com/nhjschulz/EInkPicFrame/master/design/plantuml/ClassDiagram.plantuml)

### App Layer

This part implements the application logic and utilizes a service layer for interacting with the system.

### Service Layer

This part implements generic services on top of Hal.

### Hal Layer

This part implements the interface to the AVR 328P microcontroller.

## Architecture Rules

* System is static, there is no heap usage or dynamic object generation.
* Classes are implemented as singletons or have only static elements as the system is static.
* Favour many short over fewer large classes (small is beautiful).
* Distribute code phases to states even if they are small (extensibility).
* Save power as much as possible by utilizing sleep modes and unpowering
  unused devices.
