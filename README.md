T-shirt Cannon Robot
=============

Welcome to the T-Shirt Cannon Robot repository! This repository contains the
source code for the T-shirt Cannon Robot developed at the University of Texas
at Dallas.

This README contains information about the contents of this repo and how to
find information about the robot itself.

Note that the various links in this README may have been changed between the
time of its writting and now. If this happens, your best bet is to just google
the before of after them.

Contents
-------------
doc/ :  This folder contains the overview.tex document along with any of its
dependencies. If you're unfamiliar with LaTeX, see the Documentation section
below.

java/ : This folder contains the source code for the GUI that is used to
operate the robot. We use ant (http://ant.apache.org) to compile, run, and
jar this app.
    Please familiarize yourself with this build tool before trying to
develop this app further. More information about the GUI and how it works can
be seen in the overview document mentioned above.

yun/ :  This folder contains the source code and configuration files that we
developed for the yun microcontroller (http://www.arduino.cc/en/Guide/ArduinoYun)
onboard the robot.
    In yun/AVR is the source code for the ATmega32U4, which is written in a
traditional 'Arduino' style. The yun/AVR/backup contains a sketch for to
communicate with atheros chip in case its wireless interface doesn't start up.
yun/atheros contains a README that has additional information on how to handle
this situation.
    In yun/atheros is the source code that runs on the Atheros AR9331. The only
important piece of code is atherosd.c. If we haven't already removed them from
the folder, the rest are for debugging. Additionally, yun/atheros/README
contains the information needed for compiling and copying this code to the
Atheros chip.

Running the video stream from outside the app
-------------
nc 192.168.1.1 5001 | mplayer -fps 60 -cache 1024 -

Site to install xbox controller drivers
-------------
OSX: http://tattiebogle.net/index.php/ProjectRoot/Xbox360Controller/OsxDriver

Documentation
-------------
The documentation for our project will be stored in overview.tex within doc/ of
this repo. Those who are familiar with LaTeX and have it installed can either
compile it however they know how, or run 'make overview' within this directory.

Those unfamiliar with LaTeX may want to visit this site (http://latex-project.org)
and learn more about it. For this project, it is enough to install LaTeX, cd
into doc/ and run 'make overview'
