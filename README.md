# Geiger Counter Logger
Version 0.1

Copyright (C) 2018 Christoph Haas, christoph.h@sprinternet.at

This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it.

gclogger is a lightweight daemon for reporting your 
Geiger counter readings to various radiation monitoring websites.

Tested Geiger counter devices:
- GMC-320 and GMC-320 Plus
  http://www.gqelectronicsllc.com/comersus/store/LeftStart.asp?idCategory=50


## Usage:

Compile the project using make.

Then edit the example config file and run gclogger:

```
./gclogger -c gclogger.ini -d
```
