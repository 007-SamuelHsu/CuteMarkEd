## CuteMarkEd

### DESCRIPTION

A Qt-based, free and open source markdown editor with live HTML preview, math expressions, code syntax highlighting and syntax highlighting of markdown document.

![screenshot](images/screenshot_06.png)

### DOWNLOAD

[Sources](https://github.com/cloose/CuteMarkEd/archive/v0.6.0.tar.gz)  
[MS Windows (ZIP file)](http://dl.bintray.com/cloose/CuteMarkEd/cutemarked-0.6.0.zip?direct)  
[OpenSUSE 12.3 (RPM)](https://build.opensuse.org/project/show?project=home%3Acloose1974)  

### NEWS

#### Version 0.6.0

New Features:

* `NEW` Show indicator in window title when the markdown document was modified
* `NEW` Added menu entry and shortcut to toggle between HTML preview and HTML source view
* `NEW` Added option to display markers in editor for hard linebreaks
* `NEW` Save/restore main window size, position and state
* `NEW` Added support for opening markdown files in the editor per
Drag & Drop
* `NEW` Added possibility to change the font used in the markdown editor
* `NEW` Added support for custom CSS styles. The css files are loaded from a subdirectory `styles` under the application data location. On Windows this is: `C:\Users\{User}\AppData\Local\CuteMarkEd Project\CuteMarkEd\styles`

Improvements:

* `IMPROVED` Load translation for text provided by Qt
* `IMPROVED` Support more image formats in the HTML preview on Windows
* `IMPROVED` SSL support on Windows

Fixes:

* `FIX` Do not crash application when a link with an empty href is clicked
* `FIX` Missing text in HTML preview
* `FIX` Fix print menu entry for Windows users
* `FIX` Show save changes dialog before opening another file



#### Version 0.5.0

New Features:

* `NEW` Added 'Recent File' submenu providing access to recently opened files
* `NEW` Added syntax highlighting to the HTML source view
* `NEW` Full Screen Mode
* `NEW` Help for the Markdown syntax in a dock widget
* `NEW` Created an application icon
* `NEW` Formatting commands: Blockquote, Hard Linebreak

Improvements:

* `IMPROVED` 'Export to HTML' function with option to include the current CSS style
* `IMPROVED` 'Find Next' and 'Find Previous' commands with keyboard shortcuts

Fixes:

* `CRASH FIX` Included missing platform plugin for Windows. This makes the application finally work for all, who didn't have Qt already installed on Windows.


#### Version 0.4.1

New Features:

* `NEW` Added Czech translation created by Pavel Fric


#### Version 0.4.0

New Features:

* `NEW` Added search & replace command to markdown editor
* `NEW` New CSS styles 'Clearness' and 'Clearness Dark'
* `NEW` Document statistics with line, word and character count
* `NEW` 'Center Paragraph' formatting command

Improvements:

* `IMPROVED` 'Export to PDF' function with selection of paper size and orientation
* `IMPROVED` Handling of links to local resource in HTML preview

Fixes:

* `FIX` Use fixed-pitch font for markdown editor on MS Windows
* `FIX` Missing character in HTML preview when markdown document did not end with a line break
* `FIX` `<style>` elements are not parsed anymore but included in the generated HTML


#### Version 0.3.0

New Features:

* `NEW`  Print HTML function
* `NEW`  Added several new functions for text formatting (strong, emphasize, strikethrough and inline code)
* `NEW`  Use [Kevin Burke's Markdown.css](http://kevinburke.bitbucket.org/markdowncss/) as default style for the live preview
* `NEW`  Synchronize scrollbars of markdown editor and live preview
* `NEW` Support for displaying math expressions in HTML using [MathJax](http://www.mathjax.org/)
* `NEW` Support for code syntax highlighting using [Highlight.js](http://softwaremaniacs.org/soft/highlight/en/)
* `NEW` Support for non-latin languages

Improvements:

* `IMPROVED` Add icons to main menu

Fixes:

* `FIX`  Blocking UI when inserting big text using copy&paste


#### Version 0.2.0

New Features:

* `NEW`  Add table of contents view with jump to header function
* `NEW`  New default theme for syntax highlighting of the markdown document
* `NEW`  German translation
* `NEW`  Copy HTML source to clipboard function added to edit menu

Fixes:

* `FIX`  Behavior of splitter during window resize


#### Version 0.1.0

First release with basic functionality working.

### DEPENDENCIES

* Qt 5.x (LGPL v2.1)
* [Discount 2.1.6](http://www.pell.portland.or.us/~orc/Code/discount/) (3-clause BSD)
* [PEG Markdown Highlight](http://hasseg.org/peg-markdown-highlight/) (MIT License)

### BUILD STATUS

[![Build Status](https://travis-ci.org/cloose/CuteMarkEd.png)](https://travis-ci.org/cloose/CuteMarkEd)

### LINKS

[http://www.ohloh.net/p/CuteMarkEd](http://www.ohloh.net/p/CuteMarkEd)  
[http://freecode.com/projects/cutemarked](http://freecode.com/projects/cutemarked)  
[http://qt-apps.org/content/show.php/CuteMarkEd?content=158801](http://qt-apps.org/content/show.php/CuteMarkEd?content=158801)  
[http://www.heise.de/download/cutemarked-1191267.html](http://www.heise.de/download/cutemarked-1191267.html)  
[http://www.softpedia.com/get/Programming/File-Editors/CuteMarkEd.shtml](http://www.softpedia.com/get/Programming/File-Editors/CuteMarkEd.shtml)

[![CuteMarkEd - Download - heise online](http://www.heise.de/software/icons/download_logo1.png)](http://www.heise.de/download/cutemarked-1191267.html)