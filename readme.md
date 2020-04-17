# File Tree Pane Widget Module
QT5 Widget

# Project Organization
Top Level
* Plugin
  * The plugin controller
* Interface.cpp
  * The plugin interface that is meant to be included by the user application
* /UI/
  * All user interface code or anything that uses QT
  * In an Model-View-Controller (MVC) design this is the View
* /core/
  * All non-user interface code. Does not include QT.
  * Although QT includes non-UI features, the entire QT framework is considered as UI since the non-UI features would not be seperable in case the UI framework was changed to another framework.
  * In an Model-View-Controller (MVC) design this is the Model


# Coding Practices
* header guards are all caps and use underscore spacing
