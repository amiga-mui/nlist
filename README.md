# NList MUI Classes

[![Build Status](https://travis-ci.org/amiga-mui/nlist.svg?branch=master)](https://travis-ci.org/amiga-mui/nlist) [![Code Climate](https://codeclimate.com/github/amiga-mui/nlist/badges/gpa.svg)](https://codeclimate.com/github/amiga-mui/nlist) [![License](http://img.shields.io/:license-lgpl2-blue.svg?style=flat)](http://www.gnu.org/licenses/lgpl-2.1.html) [![Github Issues](http://githubbadges.herokuapp.com/amiga-mui/nlist/issues.svg)](https://github.com/amiga-mui/nlist/issues)

This is a MUI custom class package containing replacements for the
List/Listtree/Listviews/Floattext/Bitmap/Balance MUI built-in classes.

Horizontal scrolling is fully supported, as well as smooth scrolling,
word wrap, clipboard support, the classic char selection possibility,
drag & drop and many other features. The user can change the column
width with the mouse, and column titles can act as buttons.

A NListviews.mcp prefs class is included, allowing the user to change
backgrounds, pens, fonts, scrollbars mode, drag & drop mode, multiselect
mode, keys, qualifiers and more.

Also included is a Floattext.mui replacement that uses NFloattext.mcc,
enabling copy to clipboard and key moving.

The 'NList'-package includes the following MCC classes:

* NList.mcc – List.mui replacement class
* NListview.mcc – Listview.mui replacement class
* NFloattext.mcc – Floattext.mui replacement class
* NListtree.mcc – Listtree.mcc replacement class
* NBitmap.mcc – Bitmap.mui replacement class
* NBalance.mcc – Balance.mui replacement class
* NListviews.mcp – Preference class for NListview.mcc
* NListtree.mcp – Preference class for NListtree.mcc

NList MUI Classes are available for AmigaOS3, AmigaOS4, MorphOS and AROS.

## Downloads/Releases

All releases up to the most current ones can be downloaded from our
[central releases management](https://github.com/amiga-mui/nlist/releases).

## Bug Reports / Enhancement Requests

To report bugs use the [bug/issue tracker](https://github.com/amiga-mui/nlist/issues).

## Manual Installation

1. Extract the archive to a temporary directory.
   ```
   > cd RAM:
   > lha x MCC_NList.lha
   ```

2. Go to the `MCC_NList/Libs/MUI/<OS>` directory where `<OS>` is the directory
   matching the operating system you want to install NList for:
   ```
   > cd MCC_NList/Libs/MUI/AmigaOS4
   ```

3. copy all `#?.mcc` and `#?.mcp` files found in that `<OS>` directory to the
   global `MUI:Libs/mui/` directory on your system partition:
   ```
   > copy #?.mcc MUI:Libs/mui/
   > copy #?.mcp MUI:Libs/mui/
   ```

4. reboot and enjoy the new version ;)

## License / Copyright

The NList/NListview/NFloattext classes were originally written in 1996
and are Copyright (C) 1996-1997 by Gilles Masson, whereas the NListtree
classes were originally written in 1999 and are Copyright (C) 1999-2001
by Carsten Scholling. Since 2001, both class families are maintained and
Copyright (C) 2001-2018 NList Open Source Team.

All NList classes are distributed and licensed under the GNU Lesser General
Public License Version 2.1. See [COPYING](COPYING) for more detailed information.

## Authors

* Carsten Scholling
* Daniel Allsopp
* Gilles Masson
* Gunther Nikl
* Jens Maus
* Matthias Rustler
* Przemyslaw Grunchala
* Richard Poser
* Sebastian Bauer
* Thore Böckelmann
