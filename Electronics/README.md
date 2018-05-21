# Electronics Folder

This folder contains files related to the robot electronics: PCB and wiring.

For PCB creation, the free software  [Designspark 8.0](https://www.rs-online.com/designspark/pcb-download-and-installation) was used.

### GlobalArchitecture

A global electrical diagram of the robot, showing the full architecture. All the components are represented, along with wiring links between them.

### Libraries
This folder contains two PCB component libraries used in the main PCB:
 - pinheader : this library contains many footprints for 2.54mm-spaced standard pin headers.
 - Eurobot : a miscellaneous library containing all the custom components used, or that  had been used for previous PCB for Eurobot.
 
To add these libraries to Designspark, go to Libraries (Ctrl + L), Folders, Add and add the path to the Libraries folder.

### BeageboneCape

This is the main (and only) PCB realized this year. It is a cape covering the Beaglebone (like an Arduino shield), responsible for all the interface between the Beaglebone
and the other components, as well as power distribution. See BeagleboneCape.sch and BeagleboneCape.pcb for schematic and layout, respectively.
