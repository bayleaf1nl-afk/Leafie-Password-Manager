import re
import sys
from pathlib import Path

"""usage: python3 fix_qt_includes.py <directory>. it is recursive."""


QT_HEADER_MAP = {
    # QtCore
    "qabstractitemmodel": "QAbstractItemModel",
    "qabstractlistmodel": "QAbstractListModel",
    "qabstracttablemodel": "QAbstractTableModel",
    "qbytearray": "QByteArray",
    "qcoreapplication": "QCoreApplication",
    "qcryptographichash": "QCryptographicHash",
    "qdatastream": "QDataStream",
    "qdatetime": "QDateTime",
    "qdebug": "QDebug",
    "qdir": "QDir",
    "qelapsedtimer": "QElapsedTimer",
    "qeventloop": "QEventLoop",
    "qfile": "QFile",
    "qfiledevice": "QFileDevice",
    "qfileinfo": "QFileInfo",
    "qhash": "QHash",
    "qiodevice": "QIODevice",
    "qjsonarray": "QJsonArray",
    "qjsondocument": "QJsonDocument",
    "qjsonobject": "QJsonObject",
    "qjsonvalue": "QJsonValue",
    "qline": "QLine",
    "qlist": "QList",
    "qlocale": "QLocale",
    "qlogging": "QDebug",
    "qnamespace": "Qt",
    "qmap": "QMap",
    "qmimedata": "QMimeData",
    "qmutex": "QMutex",
    "qobject": "QObject",
    "qpair": "QPair",
    "qpluginloader": "QPluginLoader",
    "qpoint": "QPoint",
    "qpointer": "QPointer",
    "qprocess": "QProcess",
    "qqueue": "QQueue",
    "qrandomgenerator": "QRandomGenerator",
    "qrect": "QRect",
    "qregularexpression": "QRegularExpression",
    "qresource": "QResource",
    "qrunnable": "QRunnable",
    "qsemaphore": "QSemaphore",
    "qset": "QSet",
    "qsettings": "QSettings",
    "qsharedpointer": "QSharedPointer",
    "qsignalmapper": "QSignalMapper",
    "qsize": "QSize",
    "qsortfilterproxymodel": "QSortFilterProxyModel",
    "qstack": "QStack",
    "qstandardpaths": "QStandardPaths",
    "qstring": "QString",
    "qstringlist": "QStringList",
    "qstringlistmodel": "QStringListModel",
    "qtemporaryfile": "QTemporaryFile",
    "qtextstream": "QTextStream",
    "qthread": "QThread",
    "qthreadpool": "QThreadPool",
    "qtimer": "QTimer",
    "qtranslator": "QTranslator",
    "qurl": "QUrl",
    "quuid": "QUuid",
    "qvariant": "QVariant",
    "qvector": "QVector",
    "qversionnumber": "QVersionNumber",

    # QtGui
    "qaction": "QAction",
    "qactiongroup": "QActionGroup",
    "qbitmap": "QBitmap",
    "qbrush": "QBrush",
    "qclipboard": "QClipboard",
    "qcloseevent": "QCloseEvent",
    "qcolor": "QColor",
    "qcontextmenuevent": "QContextMenuEvent",
    "qcursor": "QCursor",
    "qdrag": "QDrag",
    "qdragenterevent": "QDragEnterEvent",
    "qdropevent": "QDropEvent",
    "qfont": "QFont",
    "qfontdatabase": "QFontDatabase",
    "qfontmetrics": "QFontMetrics",
    "qicon": "QIcon",
    "qimage": "QImage",
    "qkeyevent": "QKeyEvent",
    "qkeysequence": "QKeySequence",
    "qmouseevent": "QMouseEvent",
    "qmovie": "QMovie",
    "qpainter": "QPainter",
    "qpaintevent": "QPaintEvent",
    "qpalette": "QPalette",
    "qpen": "QPen",
    "qpicture": "QPicture",
    "qpixmap": "QPixmap",
    "qresizeevent": "QResizeEvent",
    "qscreen": "QScreen",
    "qshowevent": "QShowEvent",
    "qstandarditemmodel": "QStandardItemModel",
    "qtextcursor": "QTextCursor",
    "qtextdocument": "QTextDocument",
    "qvalidator": "QValidator",
    "qwheelevent": "QWheelEvent",
    "qwindow": "QWindow",

    # QtWidgets
    "qabstractbutton": "QAbstractButton",
    "qabstractitemview": "QAbstractItemView",
    "qapplication": "QApplication",
    "qboxlayout": "QBoxLayout",
    "qbuttongroup": "QButtonGroup",
    "qcalendarwidget": "QCalendarWidget",
    "qcheckbox": "QCheckBox",
    "qcolordialog": "QColorDialog",
    "qcombobox": "QComboBox",
    "qcompleter": "QCompleter",
    "qdatetimeedit": "QDateTimeEdit",
    "qdial": "QDial",
    "qdialog": "QDialog",
    "qdialogbuttonbox": "QDialogButtonBox",
    "qdockwidget": "QDockWidget",
    "qdoublespinbox": "QDoubleSpinBox",
    "qfiledialog": "QFileDialog",
    "qfontdialog": "QFontDialog",
    "qformlayout": "QFormLayout",
    "qframe": "QFrame",
    "qgraphicsitem": "QGraphicsItem",
    "qgraphicsscene": "QGraphicsScene",
    "qgraphicsview": "QGraphicsView",
    "qgridlayout": "QGridLayout",
    "qgroupbox": "QGroupBox",
    "qheaderview": "QHeaderView",
    "qhboxlayout": "QHBoxLayout",
    "qinputdialog": "QInputDialog",
    "qitemdelegate": "QItemDelegate",
    "qlabel": "QLabel",
    "qlayout": "QLayout",
    "qlayoutitem": "QLayoutItem",
    "qlineedit": "QLineEdit",
    "qlistview": "QListView",
    "qlistwidget": "QListWidget",
    "qmainwindow": "QMainWindow",
    "qmenu": "QMenu",
    "qmenubar": "QMenuBar",
    "qmessagebox": "QMessageBox",
    "qplaintextedit": "QPlainTextEdit",
    "qprogressbar": "QProgressBar",
    "qprogressdialog": "QProgressDialog",
    "qpushbutton": "QPushButton",
    "qradiobutton": "QRadioButton",
    "qscrollarea": "QScrollArea",
    "qscrollbar": "QScrollBar",
    "qshortcut": "QShortcut",
    "qsizepolicy": "QSizePolicy",
    "qslider": "QSlider",
    "qspinbox": "QSpinBox",
    "qsplashscreen": "QSplashScreen",
    "qsplitter": "QSplitter",
    "qstackedwidget": "QStackedWidget",
    "qstatusbar": "QStatusBar",
    "qstyle": "QStyle",
    "qstyleoption": "QStyleOption",
    "qsystemtrayicon": "QSystemTrayIcon",
    "qtabbar": "QTabBar",
    "qtablewidget": "QTableWidget",
    "qtabwidget": "QTabWidget",
    "qtextbrowser": "QTextBrowser",
    "qtextedit": "QTextEdit",
    "qtoolbar": "QToolBar",
    "qtoolbutton": "QToolButton",
    "qtooltip": "QToolTip",
    "qtreeview": "QTreeView",
    "qtreewidget": "QTreeWidget",
    "qundostack": "QUndoStack",
    "qvboxlayout": "QVBoxLayout",
    "qwidget": "QWidget",
    "qwidgetaction": "QWidgetAction",
    "qwizard": "QWizard",

    # QtNetwork (common ones)
    "qhostaddress": "QHostAddress",
    "qnetworkaccessmanager": "QNetworkAccessManager",
    "qnetworkreply": "QNetworkReply",
    "qnetworkrequest": "QNetworkRequest",
    "qtcpserver": "QTcpServer",
    "qtcpsocket": "QTcpSocket",
    "qudpsocket": "QUdpSocket",
}

MATCHES = re.compile(r"""#include\s*<([a-z0-9_]+)\.h>""")
ANY_INCLUDE = re.compile(r"""^\s*#include\s*[<"].+[>"]\s*$""")


def fix_file(path: Path) -> bool:
    text = path.read_text(encoding='utf-8')
    changed = False
    unmapped = set()

    def replace(match: re.Match) -> str:
        nonlocal changed
        name = match.group(1)
        if name in QT_HEADER_MAP:
            changed = True
            return f"#include <{QT_HEADER_MAP[name]}>"
        if name.startswith("q"):
            unmapped.add(name)
        return match.group(0)

    new_text = MATCHES.sub(replace, text)

    if unmapped:
        print(f" [!] {path}: no mapping for: {', '.join(sorted(unmapped))}")
    if changed:
        path.write_text(new_text, encoding='utf-8')
        print(f"[x] fixed {path}")

    return changed


def dedup_file(path: Path) -> bool:
    lines = path.read_text(encoding='utf-8').splitlines(keepends=True)
    seen = set()
    out = []
    changed = False

    for line in lines:
        stripped = line.strip()
        if ANY_INCLUDE.match(stripped):
            if stripped in seen:
                changed = True
                continue
            seen.add(stripped)
        out.append(line)

    if changed:
        path.write_text("".join(out), encoding='utf-8')
        print(f"[x] deduped {path}")

    return changed


def main():
    if len(sys.argv) != 2:
        print("usage: python3 fix_qt_includes.py <directory>")
        sys.exit(1)

    root = Path(sys.argv[1])
    exts = {".cpp", ".h"}
    files = [p for p in root.rglob("*") if p.suffix in exts]

    any_changed = False
    for f in files:
        fixed = fix_file(f)
        deduped = dedup_file(f)
        if fixed or deduped:
            any_changed = True

    if not any_changed:
        print("No changes made (already clean or nothing matched)")

if __name__ == "__main__":
    main()