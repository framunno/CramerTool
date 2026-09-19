# Third-party notices

CramerTool is released under the MIT License (see [LICENSE](LICENSE)). Its only
third-party dependency is the Qt framework.

| Component | Version | License | Used for | Shipped in the release ZIP |
|---|---|---|---|---|
| [Qt](https://www.qt.io/) | 6.2 or newer (built against 6.11.1) | LGPL v3 (or a commercial Qt licence) | Widgets, event loop, translations, QTest | Yes: the Qt libraries and plugins collected by `windeployqt` |

## Qt and the LGPL v3

CramerTool uses Qt under the **GNU Lesser General Public License version 3**.
The practical obligations that come with it, and how this project meets them:

1. **Dynamic linking.** Qt is used as shared libraries; the release package
   ships the Qt DLLs next to `CramerTool.exe` rather than linking them
   statically. A user is therefore free to replace them with their own build of
   the same Qt version.
2. **Notice.** This file states that Qt is used and under which licence, and the
   application repeats it in *Help ▸ About CramerTool*. *Help ▸ About Qt* shows
   Qt's own licence dialog.
3. **Source of Qt.** Qt's own source code is not redistributed here; it is
   available from [download.qt.io](https://download.qt.io/) and from
   [code.qt.io](https://code.qt.io/cgit/qt/qt5.git/).
4. **No modifications.** Qt is used unmodified. If it were modified, those
   changes would have to be published under the LGPL as well.
5. **CramerTool's own code** stays under the MIT License: the LGPL covers the
   library, not the application that links to it.

Qt is a trademark of The Qt Company Ltd. This project is not affiliated with,
nor endorsed by, The Qt Company.

## Other resources

The application icon in `resources/` was drawn for this project and is covered
by the MIT License above. The fonts used by the interface are the ones provided
by the operating system; none is redistributed.
