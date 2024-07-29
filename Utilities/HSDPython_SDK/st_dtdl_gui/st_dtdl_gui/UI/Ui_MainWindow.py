# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'Ui_MainWindow.ui'
##
## Created by: Qt User Interface Compiler version 6.6.1
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide6.QtCore import (QCoreApplication, QDate, QDateTime, QLocale,
    QMetaObject, QObject, QPoint, QRect,
    QSize, QTime, QUrl, Qt)
from PySide6.QtGui import (QBrush, QColor, QConicalGradient, QCursor,
    QFont, QFontDatabase, QGradient, QIcon,
    QImage, QKeySequence, QLinearGradient, QPainter,
    QPalette, QPixmap, QRadialGradient, QTransform)
from PySide6.QtWidgets import (QAbstractItemView, QApplication, QCheckBox, QFrame,
    QGroupBox, QHBoxLayout, QLabel, QLineEdit,
    QListView, QListWidget, QListWidgetItem, QMainWindow,
    QPushButton, QScrollArea, QSizePolicy, QSpacerItem,
    QSplitter, QStackedWidget, QTextEdit, QVBoxLayout,
    QWidget)
import st_dtdl_gui.UI.resources_rc

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        if not MainWindow.objectName():
            MainWindow.setObjectName(u"MainWindow")
        MainWindow.resize(1077, 872)
        MainWindow.setMinimumSize(QSize(0, 0))
        palette = QPalette()
        brush = QBrush(QColor(0, 0, 0, 0))
        brush.setStyle(Qt.SolidPattern)
        palette.setBrush(QPalette.Active, QPalette.Button, brush)
        palette.setBrush(QPalette.Active, QPalette.Base, brush)
        palette.setBrush(QPalette.Active, QPalette.Window, brush)
        palette.setBrush(QPalette.Inactive, QPalette.Button, brush)
        palette.setBrush(QPalette.Inactive, QPalette.Base, brush)
        palette.setBrush(QPalette.Inactive, QPalette.Window, brush)
        palette.setBrush(QPalette.Disabled, QPalette.Button, brush)
        palette.setBrush(QPalette.Disabled, QPalette.Base, brush)
        palette.setBrush(QPalette.Disabled, QPalette.Window, brush)
        MainWindow.setPalette(palette)
        font = QFont()
        font.setPointSize(10)
        MainWindow.setFont(font)
        icon = QIcon()
        icon.addFile(u":/icons/icons/ST16418_ST-Logo-Ico_3.ico", QSize(), QIcon.Normal, QIcon.Off)
        MainWindow.setWindowIcon(icon)
        MainWindow.setStyleSheet(u"QMainWindow {background: transparent; }\n"
"QToolTip {\n"
"	color: #ffffff;\n"
"	background-color: rgba(27, 29, 35, 160);\n"
"	border: 1px solid rgb(40, 40, 40);\n"
"	border-radius: 2px;\n"
"}")
        self.centralwidget = QWidget(MainWindow)
        self.centralwidget.setObjectName(u"centralwidget")
        self.centralwidget.setStyleSheet(u"background: transparent;\n"
"color: rgb(210, 210, 210);")
        self.horizontalLayout = QHBoxLayout(self.centralwidget)
        self.horizontalLayout.setSpacing(0)
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.horizontalLayout.setContentsMargins(10, 10, 10, 10)
        self.frame_main = QFrame(self.centralwidget)
        self.frame_main.setObjectName(u"frame_main")
        self.frame_main.setStyleSheet(u"/* LINE EDIT */\n"
"QLineEdit {\n"
"	height: 25px;\n"
"	background-color: rgb(27, 29, 35);\n"
"	border-radius: 5px;\n"
"	border: 2px solid rgb(27, 29, 35);\n"
"	padding-left: 10px;\n"
"}\n"
"QLineEdit:hover {\n"
"	border: 2px solid rgb(64, 71, 88);\n"
"}\n"
"QLineEdit:focus {\n"
"	border: 2px solid rgb(91, 101, 124);\n"
"}\n"
"QLineEdit[text=\"\"]{\n"
"	color:rgb(90,90,90);\n"
"}\n"
"\n"
"QLineEdit:disabled {\n"
"	color: rgb(60,60,60);\n"
"	background-color: rgb(60, 60, 60);\n"
"	border: 2px solid rgb(32, 32, 32);\n"
"}\n"
"\n"
"/* SCROLL BARS */\n"
"QScrollBar:horizontal {\n"
"    border: none;\n"
"    background: rgb(52, 59, 72);\n"
"    height: 14px;\n"
"    margin: 0px 21px 0 21px;\n"
"	border-radius: 0px;\n"
"}\n"
"QScrollBar::handle:horizontal {\n"
"    background: rgb(32, 178, 170);\n"
"    min-width: 25px;\n"
"	border-radius: 7px\n"
"}\n"
"QScrollBar::add-line:horizontal {\n"
"    border: none;\n"
"    background: rgb(55, 63, 77);\n"
"    width: 20px;\n"
"	border-top-right-radius: 7px;\n"
"    border-b"
                        "ottom-right-radius: 7px;\n"
"    subcontrol-position: right;\n"
"    subcontrol-origin: margin;\n"
"}\n"
"QScrollBar::sub-line:horizontal {\n"
"    border: none;\n"
"    background: rgb(55, 63, 77);\n"
"    width: 20px;\n"
"	border-top-left-radius: 7px;\n"
"    border-bottom-left-radius: 7px;\n"
"    subcontrol-position: left;\n"
"    subcontrol-origin: margin;\n"
"}\n"
"QScrollBar::up-arrow:horizontal, QScrollBar::down-arrow:horizontal\n"
"{\n"
"     background: none;\n"
"}\n"
"QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal\n"
"{\n"
"     background: none;\n"
"}\n"
" QScrollBar:vertical {\n"
"	border: none;\n"
"    background: rgb(52, 59, 72);\n"
"    width: 14px;\n"
"    margin: 21px 0 21px 0;\n"
"	border-radius: 0px;\n"
" }\n"
" QScrollBar::handle:vertical {	\n"
"	background: rgb(32, 178, 170);\n"
"    min-height: 25px;\n"
"	border-radius: 7px\n"
" }\n"
" QScrollBar::add-line:vertical {\n"
"     border: none;\n"
"    background: rgb(55, 63, 77);\n"
"     height: 20px;\n"
"	border-bottom-le"
                        "ft-radius: 7px;\n"
"    border-bottom-right-radius: 7px;\n"
"     subcontrol-position: bottom;\n"
"     subcontrol-origin: margin;\n"
" }\n"
" QScrollBar::sub-line:vertical {\n"
"	border: none;\n"
"    background: rgb(55, 63, 77);\n"
"     height: 20px;\n"
"	border-top-left-radius: 7px;\n"
"    border-top-right-radius: 7px;\n"
"     subcontrol-position: top;\n"
"     subcontrol-origin: margin;\n"
" }\n"
" QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical {\n"
"     background: none;\n"
" }\n"
"\n"
" QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {\n"
"     background: none;\n"
" }\n"
"\n"
"/* CHECKBOX */\n"
"QCheckBox::indicator {\n"
"    border: 3px solid rgb(27, 29, 35);\n"
"	width: 15px;\n"
"	height: 15px;\n"
"	border-radius: 10px;\n"
"    background: rgb(44, 49, 60);\n"
"}\n"
"QCheckBox::indicator:hover {\n"
"    border: 3px solid rgb(58, 66, 81);\n"
"}\n"
"QCheckBox::indicator:checked {\n"
"    background: 3px solid rgb(52, 59, 72);\n"
"	border: 3px solid rgb(52, 59, 72);	\n"
"	"
                        "background-image: url(:/icons/icons/outline_check_white_18dp.png);\n"
"}\n"
"\n"
"/* RADIO BUTTON */\n"
"QRadioButton::indicator {\n"
"    border: 3px rgb(27, 29, 35);\n"
"	width: 15px;\n"
"	height: 15px;\n"
"	border-radius: 10px;\n"
"    background: rgb(44, 49, 60);\n"
"}\n"
"QRadioButton::indicator:hover {\n"
"    border: 3px solid rgb(58, 66, 81);\n"
"}\n"
"QRadioButton::indicator:checked {\n"
"    background: 3px solid rgb(94, 106, 130);\n"
"	border: 3px solid rgb(52, 59, 72);	\n"
"}\n"
"\n"
"/* COMBOBOX */\n"
"QComboBox{\n"
"	background-color: rgb(27, 29, 35);\n"
"	border-radius: 5px;\n"
"	border: 2px solid rgb(27, 29, 35);\n"
"	padding: 5px;\n"
"	padding-left: 10px;\n"
"}\n"
"QComboBox:hover{\n"
"	border: 2px solid rgb(64, 71, 88);\n"
"}\n"
"QComboBox::drop-down {\n"
"	subcontrol-origin: padding;\n"
"	subcontrol-position: top right;\n"
"	width: 25px; \n"
"	border-left-width: 3px;\n"
"	border-left-color: rgba(39, 44, 54, 150);\n"
"	border-left-style: solid;\n"
"	border-top-right-radius: 3px;\n"
"	border-b"
                        "ottom-right-radius: 3px;	\n"
"	background-image: url(:/icons/icons/outline_arrow_bottom_white_18dp.png);\n"
"	background-position: center;\n"
"	background-repeat: no-reperat;\n"
" }\n"
"QComboBox QAbstractItemView {\n"
"	color: rgb(85, 170, 255);	\n"
"	background-color: rgb(27, 29, 35);\n"
"	padding: 10px;\n"
"	selection-background-color: rgb(39, 44, 54);\n"
"}\n"
"\n"
"/* SLIDERS */\n"
"QSlider::groove:horizontal {\n"
"    border-radius: 9px;\n"
"    height: 18px;\n"
"	margin: 0px;\n"
"	background-color: rgb(52, 59, 72);\n"
"}\n"
"QSlider::groove:horizontal:hover {\n"
"	background-color: rgb(55, 62, 76);\n"
"}\n"
"QSlider::handle:horizontal {\n"
"    background-color: rgb(32, 178, 170);\n"
"    border: none;\n"
"    height: 18px;\n"
"    width: 18px;\n"
"    margin: 0px;\n"
"	border-radius: 9px;\n"
"}\n"
"QSlider::handle:horizontal:hover {\n"
"    background-color: rgb(105, 180, 255);\n"
"}\n"
"QSlider::handle:horizontal:pressed {\n"
"    background-color: rgb(65, 130, 195);\n"
"}\n"
"\n"
"QSlider::groove:ver"
                        "tical {\n"
"    border-radius: 9px;\n"
"    width: 18px;\n"
"    margin: 0px;\n"
"	background-color: rgb(52, 59, 72);\n"
"}\n"
"QSlider::groove:vertical:hover {\n"
"	background-color: rgb(55, 62, 76);\n"
"}\n"
"QSlider::handle:vertical {\n"
"    background-color: rgb(32, 178, 170);\n"
"	border: none;\n"
"    height: 18px;\n"
"    width: 18px;\n"
"    margin: 0px;\n"
"	border-radius: 9px;\n"
"}\n"
"QSlider::handle:vertical:hover {\n"
"    background-color: rgb(105, 180, 255);\n"
"}\n"
"QSlider::handle:vertical:pressed {\n"
"    background-color: rgb(65, 130, 195);\n"
"}\n"
"\n"
"QScrollArea{\n"
"	border-radius: 5px;\n"
"	border: 2px solid rgb(39, 44, 54);\n"
"}\n"
"\n"
"/* PROGRESS BAR */\n"
"QProgressBar {\n"
"    border: 2px solid rgb(27, 29, 35);\n"
"    border-radius: 10px;\n"
"    text-align: center;\n"
"}\n"
"\n"
"QProgressBar::chunk {\n"
"    background-color: rgb(32, 178, 170);\n"
"    margin: 4px;\n"
"    border-bottom-right-radius: 6px;\n"
"    border-bottom-left-radius: 6px;\n"
"    border-top-right-"
                        "radius: 6px;\n"
"    border-top-left-radius: 6px;\n"
"}")
        self.frame_main.setFrameShape(QFrame.NoFrame)
        self.frame_main.setFrameShadow(QFrame.Raised)
        self.verticalLayout = QVBoxLayout(self.frame_main)
        self.verticalLayout.setSpacing(0)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.verticalLayout.setContentsMargins(0, 0, 0, 0)
        self.frame_center = QFrame(self.frame_main)
        self.frame_center.setObjectName(u"frame_center")
        sizePolicy = QSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.frame_center.sizePolicy().hasHeightForWidth())
        self.frame_center.setSizePolicy(sizePolicy)
        self.frame_center.setStyleSheet(u"background-color: rgb(40, 44, 52);")
        self.frame_center.setFrameShape(QFrame.NoFrame)
        self.frame_center.setFrameShadow(QFrame.Raised)
        self.horizontalLayout_2 = QHBoxLayout(self.frame_center)
        self.horizontalLayout_2.setSpacing(0)
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.horizontalLayout_2.setContentsMargins(0, 0, 0, 0)
        self.frame_left_menu = QFrame(self.frame_center)
        self.frame_left_menu.setObjectName(u"frame_left_menu")
        sizePolicy1 = QSizePolicy(QSizePolicy.Preferred, QSizePolicy.Preferred)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.frame_left_menu.sizePolicy().hasHeightForWidth())
        self.frame_left_menu.setSizePolicy(sizePolicy1)
        self.frame_left_menu.setMinimumSize(QSize(70, 0))
        self.frame_left_menu.setMaximumSize(QSize(70, 16777215))
        self.frame_left_menu.setLayoutDirection(Qt.LeftToRight)
        self.frame_left_menu.setStyleSheet(u"background-color: rgb(27, 29, 35);")
        self.frame_left_menu.setFrameShape(QFrame.NoFrame)
        self.frame_left_menu.setFrameShadow(QFrame.Raised)
        self.verticalLayout_5 = QVBoxLayout(self.frame_left_menu)
        self.verticalLayout_5.setSpacing(1)
        self.verticalLayout_5.setObjectName(u"verticalLayout_5")
        self.verticalLayout_5.setContentsMargins(0, 0, 0, 0)
        self.frame_menus = QFrame(self.frame_left_menu)
        self.frame_menus.setObjectName(u"frame_menus")
        self.frame_menus.setMinimumSize(QSize(0, 0))
        self.frame_menus.setFrameShape(QFrame.NoFrame)
        self.frame_menus.setFrameShadow(QFrame.Raised)
        self.layout_menus = QVBoxLayout(self.frame_menus)
        self.layout_menus.setSpacing(0)
        self.layout_menus.setObjectName(u"layout_menus")
        self.layout_menus.setContentsMargins(0, 0, 0, 0)
        self.menu_btn_connection = QPushButton(self.frame_menus)
        self.menu_btn_connection.setObjectName(u"menu_btn_connection")
        self.menu_btn_connection.setEnabled(True)
        sizePolicy2 = QSizePolicy(QSizePolicy.Expanding, QSizePolicy.Fixed)
        sizePolicy2.setHorizontalStretch(0)
        sizePolicy2.setVerticalStretch(0)
        sizePolicy2.setHeightForWidth(self.menu_btn_connection.sizePolicy().hasHeightForWidth())
        self.menu_btn_connection.setSizePolicy(sizePolicy2)
        self.menu_btn_connection.setMinimumSize(QSize(0, 60))
        font1 = QFont()
        self.menu_btn_connection.setFont(font1)
        self.menu_btn_connection.setLayoutDirection(Qt.LeftToRight)
        self.menu_btn_connection.setStyleSheet(u"QPushButton {	\n"
"	background-image: url(:/icons/icons/outline_settings_input_com_white_18dp.png);\n"
"	background-position: center;\n"
"    background-repeat: no-repeat;\n"
"	border: none;\n"
"    border-right: 5px solid rgb(134, 26, 34);\n"
"	background-color: rgb(27, 29, 35);\n"
"	text-align: left;\n"
"}\n"
"QPushButton:hover {\n"
"	background-color: rgb(33, 37, 43);\n"
"}\n"
"QPushButton:pressed {	\n"
"	background-color: rgb(32, 178, 170);\n"
"}")
        self.menu_btn_connection.setCheckable(False)

        self.layout_menus.addWidget(self.menu_btn_connection)

        self.menu_btn_device_conf = QPushButton(self.frame_menus)
        self.menu_btn_device_conf.setObjectName(u"menu_btn_device_conf")
        self.menu_btn_device_conf.setEnabled(True)
        sizePolicy2.setHeightForWidth(self.menu_btn_device_conf.sizePolicy().hasHeightForWidth())
        self.menu_btn_device_conf.setSizePolicy(sizePolicy2)
        self.menu_btn_device_conf.setMinimumSize(QSize(0, 60))
        self.menu_btn_device_conf.setFont(font1)
        self.menu_btn_device_conf.setLayoutDirection(Qt.LeftToRight)
        self.menu_btn_device_conf.setStyleSheet(u"QPushButton {	\n"
"	background-image: url(:/icons/icons/baseline_tune_white_18dp.png);\n"
"	background-position: center;\n"
"    background-repeat: no-repeat;\n"
"	border: none;\n"
"	background-color: rgb(27, 29, 35);\n"
"	text-align: left;\n"
"}\n"
"QPushButton:hover {\n"
"	background-color: rgb(33, 37, 43);\n"
"\n"
"}\n"
"QPushButton:pressed {	\n"
"	background-color: rgb(32, 178, 170);\n"
"}")

        self.layout_menus.addWidget(self.menu_btn_device_conf)

        self.menu_btn_experimental_features = QPushButton(self.frame_menus)
        self.menu_btn_experimental_features.setObjectName(u"menu_btn_experimental_features")
        self.menu_btn_experimental_features.setEnabled(True)
        sizePolicy2.setHeightForWidth(self.menu_btn_experimental_features.sizePolicy().hasHeightForWidth())
        self.menu_btn_experimental_features.setSizePolicy(sizePolicy2)
        self.menu_btn_experimental_features.setMinimumSize(QSize(0, 60))
        self.menu_btn_experimental_features.setFont(font1)
        self.menu_btn_experimental_features.setLayoutDirection(Qt.LeftToRight)
        self.menu_btn_experimental_features.setStyleSheet(u"QPushButton {	\n"
"	background-image: url(:/icons/icons/science_18dp_E8EAED.svg);\n"
"	background-position: center;\n"
"    background-repeat: no-repeat;\n"
"	border: none;\n"
"	background-color: rgb(27, 29, 35);\n"
"	text-align: left;\n"
"}\n"
"QPushButton:hover {\n"
"	background-color: rgb(33, 37, 43);\n"
"\n"
"}\n"
"QPushButton:pressed {	\n"
"	background-color: rgb(32, 178, 170);\n"
"}")

        self.layout_menus.addWidget(self.menu_btn_experimental_features)


        self.verticalLayout_5.addWidget(self.frame_menus, 0, Qt.AlignTop)

        self.frame_extra_menus = QFrame(self.frame_left_menu)
        self.frame_extra_menus.setObjectName(u"frame_extra_menus")
        sizePolicy1.setHeightForWidth(self.frame_extra_menus.sizePolicy().hasHeightForWidth())
        self.frame_extra_menus.setSizePolicy(sizePolicy1)
        self.frame_extra_menus.setFrameShape(QFrame.NoFrame)
        self.frame_extra_menus.setFrameShadow(QFrame.Raised)
        self.layout_menu_bottom = QVBoxLayout(self.frame_extra_menus)
        self.layout_menu_bottom.setSpacing(10)
        self.layout_menu_bottom.setObjectName(u"layout_menu_bottom")
        self.layout_menu_bottom.setContentsMargins(0, 0, 0, 25)
        self.menu_btn_about = QPushButton(self.frame_extra_menus)
        self.menu_btn_about.setObjectName(u"menu_btn_about")
        self.menu_btn_about.setEnabled(False)
        sizePolicy2.setHeightForWidth(self.menu_btn_about.sizePolicy().hasHeightForWidth())
        self.menu_btn_about.setSizePolicy(sizePolicy2)
        self.menu_btn_about.setMinimumSize(QSize(0, 60))
        self.menu_btn_about.setFont(font1)
        self.menu_btn_about.setLayoutDirection(Qt.LeftToRight)
        self.menu_btn_about.setStyleSheet(u"color: transparent;\n"
"border: none;\n"
"/*QPushButton {\n"
"	background-image: url(:/icons/icons/outline_info_white_18dp.png);\n"
"	background-position: center;\n"
"    background-repeat: no-repeat;\n"
"	border: none;\n"
"	background-color: rgb(27, 29, 35);\n"
"	text-align: left;\n"
"}\n"
"QPushButton:hover {\n"
"	background-color: rgb(33, 37, 43);\n"
"}\n"
"QPushButton:pressed {\n"
"	background-color: rgb(32, 178, 170);\n"
"}*/")

        self.layout_menu_bottom.addWidget(self.menu_btn_about)

        self.menu_btn_show_log_file = QPushButton(self.frame_extra_menus)
        self.menu_btn_show_log_file.setObjectName(u"menu_btn_show_log_file")
        self.menu_btn_show_log_file.setEnabled(True)
        sizePolicy2.setHeightForWidth(self.menu_btn_show_log_file.sizePolicy().hasHeightForWidth())
        self.menu_btn_show_log_file.setSizePolicy(sizePolicy2)
        self.menu_btn_show_log_file.setMinimumSize(QSize(0, 60))
        self.menu_btn_show_log_file.setFont(font1)
        self.menu_btn_show_log_file.setLayoutDirection(Qt.LeftToRight)
        self.menu_btn_show_log_file.setStyleSheet(u"QPushButton {	\n"
"	background-image: url(:/icons/icons/outline_description_white_18dp.png);\n"
"	background-position: center;\n"
"    background-repeat: no-repeat;\n"
"	border: none;\n"
"	background-color: rgb(27, 29, 35);\n"
"	text-align: left;\n"
"}\n"
"QPushButton:hover {\n"
"	background-color: rgb(33, 37, 43);\n"
"\n"
"}\n"
"QPushButton:pressed {	\n"
"	background-color: rgb(32, 178, 170);\n"
"}")

        self.layout_menu_bottom.addWidget(self.menu_btn_show_log_file)


        self.verticalLayout_5.addWidget(self.frame_extra_menus, 0, Qt.AlignBottom)


        self.horizontalLayout_2.addWidget(self.frame_left_menu)

        self.frame_content_right = QFrame(self.frame_center)
        self.frame_content_right.setObjectName(u"frame_content_right")
        self.frame_content_right.setStyleSheet(u"background-color: rgb(44, 49, 60);")
        self.frame_content_right.setFrameShape(QFrame.NoFrame)
        self.frame_content_right.setFrameShadow(QFrame.Raised)
        self.verticalLayout_4 = QVBoxLayout(self.frame_content_right)
        self.verticalLayout_4.setSpacing(0)
        self.verticalLayout_4.setObjectName(u"verticalLayout_4")
        self.verticalLayout_4.setContentsMargins(0, 0, 0, 0)
        self.frame_content = QFrame(self.frame_content_right)
        self.frame_content.setObjectName(u"frame_content")
        sizePolicy1.setHeightForWidth(self.frame_content.sizePolicy().hasHeightForWidth())
        self.frame_content.setSizePolicy(sizePolicy1)
        self.frame_content.setMinimumSize(QSize(0, 0))
        self.frame_content.setFrameShape(QFrame.NoFrame)
        self.frame_content.setFrameShadow(QFrame.Raised)
        self.verticalLayout_9 = QVBoxLayout(self.frame_content)
        self.verticalLayout_9.setSpacing(0)
        self.verticalLayout_9.setObjectName(u"verticalLayout_9")
        self.verticalLayout_9.setContentsMargins(5, 5, 5, 5)
        self.stacked_widget = QStackedWidget(self.frame_content)
        self.stacked_widget.setObjectName(u"stacked_widget")
        self.stacked_widget.setMinimumSize(QSize(0, 0))
        self.stacked_widget.setStyleSheet(u"background: transparent;")
        self.page_connection = QWidget()
        self.page_connection.setObjectName(u"page_connection")
        self.page_connection_vlayout = QVBoxLayout(self.page_connection)
        self.page_connection_vlayout.setObjectName(u"page_connection_vlayout")
        self.additional_settings_frame = QFrame(self.page_connection)
        self.additional_settings_frame.setObjectName(u"additional_settings_frame")
        self.additional_settings_frame.setMinimumSize(QSize(0, 0))
        self.additional_settings_frame.setFrameShape(QFrame.StyledPanel)
        self.additional_settings_frame.setFrameShadow(QFrame.Raised)
        self.verticalLayout_17 = QVBoxLayout(self.additional_settings_frame)
        self.verticalLayout_17.setObjectName(u"verticalLayout_17")
        self.frame_log_file_options = QFrame(self.additional_settings_frame)
        self.frame_log_file_options.setObjectName(u"frame_log_file_options")
        self.frame_log_file_options.setFrameShape(QFrame.StyledPanel)
        self.frame_log_file_options.setFrameShadow(QFrame.Raised)
        self.horizontalLayout_4 = QHBoxLayout(self.frame_log_file_options)
        self.horizontalLayout_4.setObjectName(u"horizontalLayout_4")

        self.verticalLayout_17.addWidget(self.frame_log_file_options)

        self.frame_dt_settings = QFrame(self.additional_settings_frame)
        self.frame_dt_settings.setObjectName(u"frame_dt_settings")
        self.frame_dt_settings.setFrameShape(QFrame.StyledPanel)
        self.frame_dt_settings.setFrameShadow(QFrame.Raised)
        self.horizontalLayout_8 = QHBoxLayout(self.frame_dt_settings)
        self.horizontalLayout_8.setObjectName(u"horizontalLayout_8")

        self.verticalLayout_17.addWidget(self.frame_dt_settings)


        self.page_connection_vlayout.addWidget(self.additional_settings_frame, 0, Qt.AlignRight)

        self.verticalSpacer = QSpacerItem(20, 40, QSizePolicy.Minimum, QSizePolicy.Expanding)

        self.page_connection_vlayout.addItem(self.verticalSpacer)

        self.frame_4 = QFrame(self.page_connection)
        self.frame_4.setObjectName(u"frame_4")
        sizePolicy3 = QSizePolicy(QSizePolicy.Preferred, QSizePolicy.Fixed)
        sizePolicy3.setHorizontalStretch(0)
        sizePolicy3.setVerticalStretch(0)
        sizePolicy3.setHeightForWidth(self.frame_4.sizePolicy().hasHeightForWidth())
        self.frame_4.setSizePolicy(sizePolicy3)
        self.frame_4.setMinimumSize(QSize(600, 0))
        self.frame_4.setMaximumSize(QSize(16777215, 400))
        self.frame_4.setStyleSheet(u"border-radius: 5px;")
        self.frame_4.setFrameShape(QFrame.StyledPanel)
        self.frame_4.setFrameShadow(QFrame.Raised)
        self.verticalLayout_12 = QVBoxLayout(self.frame_4)
        self.verticalLayout_12.setSpacing(0)
        self.verticalLayout_12.setObjectName(u"verticalLayout_12")
        self.verticalLayout_12.setContentsMargins(24, 0, 24, 0)
        self.label_app_title = QLabel(self.frame_4)
        self.label_app_title.setObjectName(u"label_app_title")
        self.label_app_title.setMaximumSize(QSize(16777215, 16777215))
        font2 = QFont()
        font2.setPointSize(48)
        font2.setBold(True)
        self.label_app_title.setFont(font2)
        self.label_app_title.setAlignment(Qt.AlignCenter)

        self.verticalLayout_12.addWidget(self.label_app_title)


        self.page_connection_vlayout.addWidget(self.frame_4)

        self.stacked_widget.addWidget(self.page_connection)
        self.page_device_config = QWidget()
        self.page_device_config.setObjectName(u"page_device_config")
        self.page_device_config.setEnabled(True)
        sizePolicy1.setHeightForWidth(self.page_device_config.sizePolicy().hasHeightForWidth())
        self.page_device_config.setSizePolicy(sizePolicy1)
        self.page_device_config.setMinimumSize(QSize(0, 0))
        self.verticalLayout_6 = QVBoxLayout(self.page_device_config)
        self.verticalLayout_6.setSpacing(6)
        self.verticalLayout_6.setObjectName(u"verticalLayout_6")
        self.verticalLayout_6.setContentsMargins(0, 0, 0, 0)
        self.frame_device_config = QFrame(self.page_device_config)
        self.frame_device_config.setObjectName(u"frame_device_config")
        self.frame_device_config.setEnabled(True)
        self.frame_device_config.setStyleSheet(u"")
        self.verticalLayout_2 = QVBoxLayout(self.frame_device_config)
        self.verticalLayout_2.setObjectName(u"verticalLayout_2")
        self.widget_header = QWidget(self.frame_device_config)
        self.widget_header.setObjectName(u"widget_header")
        self.widget_header.setMinimumSize(QSize(0, 0))
        self.widget_header.setStyleSheet(u"")
        self.horizontalLayout_3 = QHBoxLayout(self.widget_header)
        self.horizontalLayout_3.setObjectName(u"horizontalLayout_3")
        self.horizontalLayout_3.setContentsMargins(0, 0, 0, 0)

        self.verticalLayout_2.addWidget(self.widget_header)

        self.splitter = QSplitter(self.frame_device_config)
        self.splitter.setObjectName(u"splitter")
        sizePolicy.setHeightForWidth(self.splitter.sizePolicy().hasHeightForWidth())
        self.splitter.setSizePolicy(sizePolicy)
        self.splitter.setOrientation(Qt.Horizontal)
        self.splitter.setOpaqueResize(True)
        self.scrollArea_device_config = QScrollArea(self.splitter)
        self.scrollArea_device_config.setObjectName(u"scrollArea_device_config")
        sizePolicy4 = QSizePolicy(QSizePolicy.Fixed, QSizePolicy.Expanding)
        sizePolicy4.setHorizontalStretch(0)
        sizePolicy4.setVerticalStretch(0)
        sizePolicy4.setHeightForWidth(self.scrollArea_device_config.sizePolicy().hasHeightForWidth())
        self.scrollArea_device_config.setSizePolicy(sizePolicy4)
        self.scrollArea_device_config.setMinimumSize(QSize(500, 0))
        self.scrollArea_device_config.setMaximumSize(QSize(500, 16777215))
        self.scrollArea_device_config.setWidgetResizable(True)
        self.widget_device_config_main = QWidget()
        self.widget_device_config_main.setObjectName(u"widget_device_config_main")
        self.widget_device_config_main.setGeometry(QRect(0, 0, 482, 45))
        self.verticalLayout_3 = QVBoxLayout(self.widget_device_config_main)
        self.verticalLayout_3.setSpacing(0)
        self.verticalLayout_3.setObjectName(u"verticalLayout_3")
        self.verticalLayout_3.setContentsMargins(3, 3, 3, 3)
        self.widget_special_components = QWidget(self.widget_device_config_main)
        self.widget_special_components.setObjectName(u"widget_special_components")
        self.widget_special_components.setMinimumSize(QSize(0, 0))
        self.verticalLayout_13 = QVBoxLayout(self.widget_special_components)
        self.verticalLayout_13.setSpacing(0)
        self.verticalLayout_13.setObjectName(u"verticalLayout_13")
        self.verticalLayout_13.setContentsMargins(0, 0, 0, 0)

        self.verticalLayout_3.addWidget(self.widget_special_components)

        self.line = QFrame(self.widget_device_config_main)
        self.line.setObjectName(u"line")
        self.line.setStyleSheet(u"color: rgb(27, 29, 35);")
        self.line.setFrameShadow(QFrame.Plain)
        self.line.setLineWidth(3)
        self.line.setFrameShape(QFrame.HLine)

        self.verticalLayout_3.addWidget(self.line)

        self.widget_device_config = QWidget(self.widget_device_config_main)
        self.widget_device_config.setObjectName(u"widget_device_config")
        self.widget_device_config.setMinimumSize(QSize(0, 0))
        self.verticalLayout_11 = QVBoxLayout(self.widget_device_config)
        self.verticalLayout_11.setSpacing(0)
        self.verticalLayout_11.setObjectName(u"verticalLayout_11")
        self.verticalLayout_11.setContentsMargins(0, 0, 0, 0)
        self.select_all_frame = QFrame(self.widget_device_config)
        self.select_all_frame.setObjectName(u"select_all_frame")
        self.select_all_frame.setMinimumSize(QSize(0, 0))
        self.select_all_frame.setFrameShape(QFrame.StyledPanel)
        self.select_all_frame.setFrameShadow(QFrame.Raised)
        self.horizontalLayout_5 = QHBoxLayout(self.select_all_frame)
        self.horizontalLayout_5.setObjectName(u"horizontalLayout_5")
        self.select_all_label = QLabel(self.select_all_frame)
        self.select_all_label.setObjectName(u"select_all_label")
        font3 = QFont()
        font3.setBold(True)
        self.select_all_label.setFont(font3)

        self.horizontalLayout_5.addWidget(self.select_all_label, 0, Qt.AlignRight)


        self.verticalLayout_11.addWidget(self.select_all_frame)


        self.verticalLayout_3.addWidget(self.widget_device_config)

        self.verticalSpacer_2 = QSpacerItem(20, 40, QSizePolicy.Minimum, QSizePolicy.Expanding)

        self.verticalLayout_3.addItem(self.verticalSpacer_2)

        self.scrollArea_device_config.setWidget(self.widget_device_config_main)
        self.splitter.addWidget(self.scrollArea_device_config)
        self.scrollArea_plots = QScrollArea(self.splitter)
        self.scrollArea_plots.setObjectName(u"scrollArea_plots")
        self.scrollArea_plots.setWidgetResizable(True)
        self.widget_plots = QWidget()
        self.widget_plots.setObjectName(u"widget_plots")
        self.widget_plots.setGeometry(QRect(0, 0, 20, 16))
        sizePolicy5 = QSizePolicy(QSizePolicy.Expanding, QSizePolicy.Preferred)
        sizePolicy5.setHorizontalStretch(0)
        sizePolicy5.setVerticalStretch(0)
        sizePolicy5.setHeightForWidth(self.widget_plots.sizePolicy().hasHeightForWidth())
        self.widget_plots.setSizePolicy(sizePolicy5)
        self.verticalLayout_7 = QVBoxLayout(self.widget_plots)
        self.verticalLayout_7.setSpacing(0)
        self.verticalLayout_7.setObjectName(u"verticalLayout_7")
        self.verticalLayout_7.setContentsMargins(3, 3, 3, 3)
        self.scrollArea_plots.setWidget(self.widget_plots)
        self.splitter.addWidget(self.scrollArea_plots)

        self.verticalLayout_2.addWidget(self.splitter)


        self.verticalLayout_6.addWidget(self.frame_device_config)

        self.stacked_widget.addWidget(self.page_device_config)
        self.page_experimental_features = QWidget()
        self.page_experimental_features.setObjectName(u"page_experimental_features")
        self.verticalLayout_15 = QVBoxLayout(self.page_experimental_features)
        self.verticalLayout_15.setObjectName(u"verticalLayout_15")
        self.label = QLabel(self.page_experimental_features)
        self.label.setObjectName(u"label")
        font4 = QFont()
        font4.setFamilies([u"Consolas"])
        self.label.setFont(font4)
        self.label.setStyleSheet(u"padding: 20;")
        self.label.setTextFormat(Qt.MarkdownText)
        self.label.setWordWrap(True)

        self.verticalLayout_15.addWidget(self.label)

        self.splitter_2 = QSplitter(self.page_experimental_features)
        self.splitter_2.setObjectName(u"splitter_2")
        sizePolicy6 = QSizePolicy(QSizePolicy.Preferred, QSizePolicy.Expanding)
        sizePolicy6.setHorizontalStretch(0)
        sizePolicy6.setVerticalStretch(0)
        sizePolicy6.setHeightForWidth(self.splitter_2.sizePolicy().hasHeightForWidth())
        self.splitter_2.setSizePolicy(sizePolicy6)
        self.splitter_2.setOrientation(Qt.Horizontal)
        self.splitter_2.setOpaqueResize(False)
        self.splitter_2.setChildrenCollapsible(True)
        self.scrollArea_dt_settings = QScrollArea(self.splitter_2)
        self.scrollArea_dt_settings.setObjectName(u"scrollArea_dt_settings")
        sizePolicy1.setHeightForWidth(self.scrollArea_dt_settings.sizePolicy().hasHeightForWidth())
        self.scrollArea_dt_settings.setSizePolicy(sizePolicy1)
        self.scrollArea_dt_settings.setMinimumSize(QSize(300, 0))
        self.scrollArea_dt_settings.setMaximumSize(QSize(16777215, 16777215))
        self.scrollArea_dt_settings.setWidgetResizable(True)
        self.widget_data_toolkit = QWidget()
        self.widget_data_toolkit.setObjectName(u"widget_data_toolkit")
        self.widget_data_toolkit.setGeometry(QRect(0, 0, 463, 595))
        self.verticalLayout_16 = QVBoxLayout(self.widget_data_toolkit)
        self.verticalLayout_16.setSpacing(0)
        self.verticalLayout_16.setObjectName(u"verticalLayout_16")
        self.verticalLayout_16.setContentsMargins(3, 3, 3, 3)
        self.dt_settings_frame = QFrame(self.widget_data_toolkit)
        self.dt_settings_frame.setObjectName(u"dt_settings_frame")
        self.dt_settings_frame.setFrameShape(QFrame.StyledPanel)
        self.dt_settings_frame.setFrameShadow(QFrame.Raised)
        self.verticalLayout_19 = QVBoxLayout(self.dt_settings_frame)
        self.verticalLayout_19.setObjectName(u"verticalLayout_19")
        self.verticalLayout_19.setContentsMargins(0, 0, 0, 0)
        self.dt_enabled_checkBox = QCheckBox(self.dt_settings_frame)
        self.dt_enabled_checkBox.setObjectName(u"dt_enabled_checkBox")
        self.dt_enabled_checkBox.setEnabled(True)
        sizePolicy1.setHeightForWidth(self.dt_enabled_checkBox.sizePolicy().hasHeightForWidth())
        self.dt_enabled_checkBox.setSizePolicy(sizePolicy1)
        self.dt_enabled_checkBox.setStyleSheet(u"font: 700 12pt \"Segoe UI\";\n"
"background-color: rgb(39, 44, 54);\n"
"padding: 9;")
        self.dt_enabled_checkBox.setChecked(False)

        self.verticalLayout_19.addWidget(self.dt_enabled_checkBox)

        self.dt_frame_content = QFrame(self.dt_settings_frame)
        self.dt_frame_content.setObjectName(u"dt_frame_content")
        self.dt_frame_content.setEnabled(True)
        sizePolicy1.setHeightForWidth(self.dt_frame_content.sizePolicy().hasHeightForWidth())
        self.dt_frame_content.setSizePolicy(sizePolicy1)
        self.dt_frame_content.setStyleSheet(u"border-radius: 5px;")
        self.dt_frame_content.setFrameShape(QFrame.StyledPanel)
        self.dt_frame_content.setFrameShadow(QFrame.Raised)
        self.verticalLayout_20 = QVBoxLayout(self.dt_frame_content)
        self.verticalLayout_20.setSpacing(6)
        self.verticalLayout_20.setObjectName(u"verticalLayout_20")
        self.verticalLayout_20.setContentsMargins(0, 9, 0, 0)
        self.groupBox_dt_settings = QGroupBox(self.dt_frame_content)
        self.groupBox_dt_settings.setObjectName(u"groupBox_dt_settings")
        self.groupBox_dt_settings.setMinimumSize(QSize(300, 0))
        self.groupBox_dt_settings.setStyleSheet(u"QGroupBox {\n"
"	border-radius: 5px;\n"
"	border: 2px solid rgb(27, 29, 35);\n"
"}\n"
"\n"
"QRadioButton::indicator {\n"
"    border: 3px solid rgb(52, 59, 72);\n"
"	width: 15px;\n"
"	height: 15px;\n"
"	border-radius: 10px;\n"
"    background: rgb(44, 49, 60);\n"
"}\n"
"QRadioButton::indicator:hover {\n"
"    border: 3px solid rgb(58, 66, 81);\n"
"}\n"
"QRadioButton::indicator:checked {\n"
"    background: 3px solid rgb(94, 106, 130);\n"
"	border: 3px solid rgb(52, 59, 72);	\n"
"}")
        self.groupBox_dt_settings.setFlat(False)
        self.groupBox_dt_settings.setCheckable(False)
        self.verticalLayout_43 = QVBoxLayout(self.groupBox_dt_settings)
        self.verticalLayout_43.setSpacing(6)
        self.verticalLayout_43.setObjectName(u"verticalLayout_43")
        self.verticalLayout_43.setContentsMargins(6, 18, 6, 9)
        self.inner_dt_hframe = QFrame(self.groupBox_dt_settings)
        self.inner_dt_hframe.setObjectName(u"inner_dt_hframe")
        self.inner_dt_hframe.setMinimumSize(QSize(0, 0))
        self.inner_dt_hframe.setStyleSheet(u"")
        self.inner_dt_hframe.setFrameShape(QFrame.StyledPanel)
        self.inner_dt_hframe.setFrameShadow(QFrame.Raised)
        self.horizontalLayout_38 = QHBoxLayout(self.inner_dt_hframe)
        self.horizontalLayout_38.setSpacing(9)
        self.horizontalLayout_38.setObjectName(u"horizontalLayout_38")
        self.horizontalLayout_38.setContentsMargins(2, 0, 2, 0)
        self.dt_plugins_folder_lineEdit = QLineEdit(self.inner_dt_hframe)
        self.dt_plugins_folder_lineEdit.setObjectName(u"dt_plugins_folder_lineEdit")
        self.dt_plugins_folder_lineEdit.setMinimumSize(QSize(0, 30))
        self.dt_plugins_folder_lineEdit.setMaximumSize(QSize(16777215, 30))
        self.dt_plugins_folder_lineEdit.setStyleSheet(u"QLineEdit {\n"
"	height: 25px;\n"
"	background-color: rgb(27, 29, 35);\n"
"	border-radius: 5px;\n"
"	border: 2px solid rgb(27, 29, 35);\n"
"	padding-left: 10px;\n"
"}\n"
"QLineEdit:hover {\n"
"	border: 2px solid rgb(64, 71, 88);\n"
"}\n"
"QLineEdit:focus {\n"
"	border: 2px solid rgb(91, 101, 124);\n"
"}\n"
"QLineEdit[text=\"\"]{\n"
"	color:rgb(90,90,90);\n"
"}\n"
"\n"
"QLineEdit:disabled {\n"
"	color: rgb(60,60,60);\n"
"	background-color: rgb(60, 60, 60);\n"
"	border: 2px solid rgb(32, 32, 32);\n"
"}")

        self.horizontalLayout_38.addWidget(self.dt_plugins_folder_lineEdit)

        self.dt_plugins_folder_button = QPushButton(self.inner_dt_hframe)
        self.dt_plugins_folder_button.setObjectName(u"dt_plugins_folder_button")
        sizePolicy1.setHeightForWidth(self.dt_plugins_folder_button.sizePolicy().hasHeightForWidth())
        self.dt_plugins_folder_button.setSizePolicy(sizePolicy1)
        self.dt_plugins_folder_button.setMinimumSize(QSize(80, 45))
        self.dt_plugins_folder_button.setMaximumSize(QSize(16777215, 16777215))
        self.dt_plugins_folder_button.setStyleSheet(u"QPushButton {\n"
"	border: 2px solid rgb(52, 59, 72);\n"
"	border-radius: 5px;	\n"
"	background-color: rgb(52, 59, 72);\n"
"}\n"
"QPushButton:hover {\n"
"	background-color: rgb(57, 65, 80);\n"
"	border: 2px solid rgb(61, 70, 86);\n"
"}\n"
"QPushButton:pressed {	\n"
"	background-color: rgb(35, 40, 49);\n"
"	border: 2px solid rgb(43, 50, 61);\n"
"}\n"
"QPushButton:disabled {	\n"
"	background-color: rgb(35, 40, 49);\n"
"	border: 2px solid rgb(43, 50, 61);\n"
"}")
        icon1 = QIcon()
        icon1.addFile(u":/icons/icons/baseline_folder_open_white_18dp.png", QSize(), QIcon.Normal, QIcon.Off)
        self.dt_plugins_folder_button.setIcon(icon1)

        self.horizontalLayout_38.addWidget(self.dt_plugins_folder_button)


        self.verticalLayout_43.addWidget(self.inner_dt_hframe)


        self.verticalLayout_20.addWidget(self.groupBox_dt_settings)

        self.groupBox_dt_plugin_list = QGroupBox(self.dt_frame_content)
        self.groupBox_dt_plugin_list.setObjectName(u"groupBox_dt_plugin_list")
        self.groupBox_dt_plugin_list.setMinimumSize(QSize(300, 0))
        self.groupBox_dt_plugin_list.setStyleSheet(u"QGroupBox {\n"
"	border-radius: 5px;\n"
"	border: 2px solid rgb(27, 29, 35);\n"
"}\n"
"\n"
"QRadioButton::indicator {\n"
"    border: 3px solid rgb(52, 59, 72);\n"
"	width: 15px;\n"
"	height: 15px;\n"
"	border-radius: 10px;\n"
"    background: rgb(44, 49, 60);\n"
"}\n"
"QRadioButton::indicator:hover {\n"
"    border: 3px solid rgb(58, 66, 81);\n"
"}\n"
"QRadioButton::indicator:checked {\n"
"    background: 3px solid rgb(94, 106, 130);\n"
"	border: 3px solid rgb(52, 59, 72);	\n"
"}")
        self.groupBox_dt_plugin_list.setFlat(False)
        self.groupBox_dt_plugin_list.setCheckable(False)
        self.verticalLayout_51 = QVBoxLayout(self.groupBox_dt_plugin_list)
        self.verticalLayout_51.setSpacing(6)
        self.verticalLayout_51.setObjectName(u"verticalLayout_51")
        self.verticalLayout_51.setContentsMargins(9, 18, 9, 9)
        self.dt_plugin_listWidget = QListWidget(self.groupBox_dt_plugin_list)
        self.dt_plugin_listWidget.setObjectName(u"dt_plugin_listWidget")
        self.dt_plugin_listWidget.setTabKeyNavigation(False)
        self.dt_plugin_listWidget.setProperty("showDropIndicator", False)
        self.dt_plugin_listWidget.setDragEnabled(False)
        self.dt_plugin_listWidget.setDragDropOverwriteMode(False)
        self.dt_plugin_listWidget.setDragDropMode(QAbstractItemView.NoDragDrop)
        self.dt_plugin_listWidget.setDefaultDropAction(Qt.CopyAction)
        self.dt_plugin_listWidget.setAlternatingRowColors(False)
        self.dt_plugin_listWidget.setSelectionMode(QAbstractItemView.ExtendedSelection)
        self.dt_plugin_listWidget.setTextElideMode(Qt.ElideNone)
        self.dt_plugin_listWidget.setMovement(QListView.Static)
        self.dt_plugin_listWidget.setResizeMode(QListView.Fixed)
        self.dt_plugin_listWidget.setViewMode(QListView.ListMode)

        self.verticalLayout_51.addWidget(self.dt_plugin_listWidget)


        self.verticalLayout_20.addWidget(self.groupBox_dt_plugin_list)


        self.verticalLayout_19.addWidget(self.dt_frame_content)

        self.label_2 = QLabel(self.dt_settings_frame)
        self.label_2.setObjectName(u"label_2")
        self.label_2.setFont(font4)
        self.label_2.setStyleSheet(u"padding: 20;")
        self.label_2.setTextFormat(Qt.MarkdownText)
        self.label_2.setWordWrap(True)

        self.verticalLayout_19.addWidget(self.label_2)


        self.verticalLayout_16.addWidget(self.dt_settings_frame)

        self.scrollArea_dt_settings.setWidget(self.widget_data_toolkit)
        self.splitter_2.addWidget(self.scrollArea_dt_settings)
        self.scrollArea_acq_upload = QScrollArea(self.splitter_2)
        self.scrollArea_acq_upload.setObjectName(u"scrollArea_acq_upload")
        sizePolicy5.setHeightForWidth(self.scrollArea_acq_upload.sizePolicy().hasHeightForWidth())
        self.scrollArea_acq_upload.setSizePolicy(sizePolicy5)
        self.scrollArea_acq_upload.setWidgetResizable(True)
        self.widget_acq_upload = QWidget()
        self.widget_acq_upload.setObjectName(u"widget_acq_upload")
        self.widget_acq_upload.setGeometry(QRect(0, 0, 483, 595))
        sizePolicy1.setHeightForWidth(self.widget_acq_upload.sizePolicy().hasHeightForWidth())
        self.widget_acq_upload.setSizePolicy(sizePolicy1)
        self.verticalLayout_24 = QVBoxLayout(self.widget_acq_upload)
        self.verticalLayout_24.setSpacing(0)
        self.verticalLayout_24.setObjectName(u"verticalLayout_24")
        self.verticalLayout_24.setContentsMargins(3, 3, 3, 3)
        self.acq_upload_title_2 = QLabel(self.widget_acq_upload)
        self.acq_upload_title_2.setObjectName(u"acq_upload_title_2")
        self.acq_upload_title_2.setMinimumSize(QSize(0, 0))
        self.acq_upload_title_2.setStyleSheet(u"font: 700 12pt \"Segoe UI\";\n"
"background-color: rgb(39, 44, 54);")
        self.acq_upload_title_2.setMargin(9)
        self.acq_upload_title_2.setIndent(3)

        self.verticalLayout_24.addWidget(self.acq_upload_title_2)

        self.verticalSpacer_3 = QSpacerItem(20, 40, QSizePolicy.Minimum, QSizePolicy.Expanding)

        self.verticalLayout_24.addItem(self.verticalSpacer_3)

        self.acq_upload_frame = QFrame(self.widget_acq_upload)
        self.acq_upload_frame.setObjectName(u"acq_upload_frame")
        self.acq_upload_frame.setFrameShape(QFrame.StyledPanel)
        self.acq_upload_frame.setFrameShadow(QFrame.Raised)
        self.verticalLayout_18 = QVBoxLayout(self.acq_upload_frame)
        self.verticalLayout_18.setObjectName(u"verticalLayout_18")
        self.verticalLayout_18.setContentsMargins(0, 0, 0, 0)
        self.acq_upload_title = QLabel(self.acq_upload_frame)
        self.acq_upload_title.setObjectName(u"acq_upload_title")
        self.acq_upload_title.setMinimumSize(QSize(0, 0))
        self.acq_upload_title.setStyleSheet(u"font: 700 12pt \"Segoe UI\";\n"
"background-color: rgb(39, 44, 54);")
        self.acq_upload_title.setMargin(9)
        self.acq_upload_title.setIndent(3)

        self.verticalLayout_18.addWidget(self.acq_upload_title)

        self.acq_upload_frame_content = QFrame(self.acq_upload_frame)
        self.acq_upload_frame_content.setObjectName(u"acq_upload_frame_content")
        sizePolicy1.setHeightForWidth(self.acq_upload_frame_content.sizePolicy().hasHeightForWidth())
        self.acq_upload_frame_content.setSizePolicy(sizePolicy1)
        self.acq_upload_frame_content.setStyleSheet(u"border-radius: 5px;")
        self.acq_upload_frame_content.setFrameShape(QFrame.StyledPanel)
        self.acq_upload_frame_content.setFrameShadow(QFrame.Raised)
        self.verticalLayout_14 = QVBoxLayout(self.acq_upload_frame_content)
        self.verticalLayout_14.setSpacing(6)
        self.verticalLayout_14.setObjectName(u"verticalLayout_14")
        self.verticalLayout_14.setContentsMargins(0, 0, 0, 0)
        self.groupBox_login = QFrame(self.acq_upload_frame_content)
        self.groupBox_login.setObjectName(u"groupBox_login")
        self.groupBox_login.setEnabled(True)
        self.groupBox_login.setMinimumSize(QSize(0, 0))
        self.groupBox_login.setCursor(QCursor(Qt.ArrowCursor))
        self.groupBox_login.setStyleSheet(u"QComboBox{\n"
"	background-color: rgb(27, 29, 35);\n"
"	border-radius: 5px;\n"
"	border: 2px solid rgb(27, 29, 35);\n"
"	padding: 5px;\n"
"	padding-left: 10px;\n"
"}\n"
"QComboBox:hover{\n"
"	border: 2px solid rgb(64, 71, 88);\n"
"}\n"
"QComboBox QAbstractItemView {\n"
"	color: rgb(85, 170, 255);	\n"
"	background-color: rgb(27, 29, 35);\n"
"	padding: 10px;\n"
"	selection-background-color: rgb(39, 44, 54);\n"
"}\n"
"QComboBox:disabled{\n"
"	color: rgb(60,60,60);\n"
"	border: 2px solid rgb(32, 32, 32);\n"
"}")
        self.groupBox_login.setFrameShadow(QFrame.Raised)
        self.horizontalLayout_41 = QHBoxLayout(self.groupBox_login)
        self.horizontalLayout_41.setSpacing(3)
        self.horizontalLayout_41.setObjectName(u"horizontalLayout_41")
        self.horizontalLayout_41.setContentsMargins(0, 9, 0, 0)
        self.groupBox_inner_login = QGroupBox(self.groupBox_login)
        self.groupBox_inner_login.setObjectName(u"groupBox_inner_login")
        self.groupBox_inner_login.setMinimumSize(QSize(300, 0))
        self.groupBox_inner_login.setStyleSheet(u"QGroupBox {\n"
"	border-radius: 5px;\n"
"	border: 2px solid rgb(27, 29, 35);\n"
"}\n"
"\n"
"QRadioButton::indicator {\n"
"    border: 3px solid rgb(52, 59, 72);\n"
"	width: 15px;\n"
"	height: 15px;\n"
"	border-radius: 10px;\n"
"    background: rgb(44, 49, 60);\n"
"}\n"
"QRadioButton::indicator:hover {\n"
"    border: 3px solid rgb(58, 66, 81);\n"
"}\n"
"QRadioButton::indicator:checked {\n"
"    background: 3px solid rgb(94, 106, 130);\n"
"	border: 3px solid rgb(52, 59, 72);	\n"
"}")
        self.groupBox_inner_login.setFlat(False)
        self.groupBox_inner_login.setCheckable(False)
        self.verticalLayout_49 = QVBoxLayout(self.groupBox_inner_login)
        self.verticalLayout_49.setSpacing(0)
        self.verticalLayout_49.setObjectName(u"verticalLayout_49")
        self.verticalLayout_49.setContentsMargins(0, 0, 0, 0)
        self.userLogin_frame = QFrame(self.groupBox_inner_login)
        self.userLogin_frame.setObjectName(u"userLogin_frame")
        self.userLogin_frame.setMinimumSize(QSize(0, 0))
        self.userLogin_frame.setFrameShape(QFrame.StyledPanel)
        self.userLogin_frame.setFrameShadow(QFrame.Raised)
        self.horizontalLayout_49 = QHBoxLayout(self.userLogin_frame)
        self.horizontalLayout_49.setObjectName(u"horizontalLayout_49")
        self.horizontalLayout_49.setContentsMargins(9, 18, 9, -1)
        self.login_button = QPushButton(self.userLogin_frame)
        self.login_button.setObjectName(u"login_button")
        sizePolicy1.setHeightForWidth(self.login_button.sizePolicy().hasHeightForWidth())
        self.login_button.setSizePolicy(sizePolicy1)
        self.login_button.setMinimumSize(QSize(80, 45))
        self.login_button.setStyleSheet(u"QPushButton {\n"
"	border: 2px solid rgb(51, 71, 51);\n"
"	border-radius: 5px;	\n"
"	background-color: rgb(51, 71, 51);\n"
"	font: 700 9pt \"Segoe UI\";\n"
"}\n"
"QPushButton:hover {\n"
"	background-color: rgb(57, 81, 57);\n"
"	border: 2px solid rgb(61, 87, 61);\n"
"}\n"
"QPushButton:pressed {	\n"
"	background-color: rgb(34, 48, 34);\n"
"	border: 2px solid rgb(42, 60, 42);\n"
"}\n"
"QPushButton:disabled {\n"
"	background-color: rgb(29,33,41);\n"
"	border: 2px solid rgb(29,33,41);\n"
"}")

        self.horizontalLayout_49.addWidget(self.login_button)


        self.verticalLayout_49.addWidget(self.userLogin_frame)


        self.horizontalLayout_41.addWidget(self.groupBox_inner_login)


        self.verticalLayout_14.addWidget(self.groupBox_login)

        self.groupBox_base_acquisition_selection = QFrame(self.acq_upload_frame_content)
        self.groupBox_base_acquisition_selection.setObjectName(u"groupBox_base_acquisition_selection")
        self.groupBox_base_acquisition_selection.setEnabled(True)
        self.groupBox_base_acquisition_selection.setMinimumSize(QSize(0, 0))
        self.groupBox_base_acquisition_selection.setCursor(QCursor(Qt.ArrowCursor))
        self.groupBox_base_acquisition_selection.setStyleSheet(u"QComboBox{\n"
"	background-color: rgb(27, 29, 35);\n"
"	border-radius: 5px;\n"
"	border: 2px solid rgb(27, 29, 35);\n"
"	padding: 5px;\n"
"	padding-left: 10px;\n"
"}\n"
"QComboBox:hover{\n"
"	border: 2px solid rgb(64, 71, 88);\n"
"}\n"
"QComboBox QAbstractItemView {\n"
"	color: rgb(85, 170, 255);	\n"
"	background-color: rgb(27, 29, 35);\n"
"	padding: 10px;\n"
"	selection-background-color: rgb(39, 44, 54);\n"
"}\n"
"QComboBox:disabled{\n"
"	color: rgb(60,60,60);\n"
"	border: 2px solid rgb(32, 32, 32);\n"
"}")
        self.groupBox_base_acquisition_selection.setFrameShadow(QFrame.Raised)
        self.horizontalLayout_34 = QHBoxLayout(self.groupBox_base_acquisition_selection)
        self.horizontalLayout_34.setSpacing(3)
        self.horizontalLayout_34.setObjectName(u"horizontalLayout_34")
        self.horizontalLayout_34.setContentsMargins(0, 0, 0, 0)
        self.groupBox_inner_base_acquisition_selection = QGroupBox(self.groupBox_base_acquisition_selection)
        self.groupBox_inner_base_acquisition_selection.setObjectName(u"groupBox_inner_base_acquisition_selection")
        self.groupBox_inner_base_acquisition_selection.setMinimumSize(QSize(300, 0))
        self.groupBox_inner_base_acquisition_selection.setStyleSheet(u"QGroupBox {\n"
"	border-radius: 5px;\n"
"	border: 2px solid rgb(27, 29, 35);\n"
"}\n"
"\n"
"QRadioButton::indicator {\n"
"    border: 3px solid rgb(52, 59, 72);\n"
"	width: 15px;\n"
"	height: 15px;\n"
"	border-radius: 10px;\n"
"    background: rgb(44, 49, 60);\n"
"}\n"
"QRadioButton::indicator:hover {\n"
"    border: 3px solid rgb(58, 66, 81);\n"
"}\n"
"QRadioButton::indicator:checked {\n"
"    background: 3px solid rgb(94, 106, 130);\n"
"	border: 3px solid rgb(52, 59, 72);	\n"
"}")
        self.groupBox_inner_base_acquisition_selection.setFlat(False)
        self.groupBox_inner_base_acquisition_selection.setCheckable(False)
        self.verticalLayout_41 = QVBoxLayout(self.groupBox_inner_base_acquisition_selection)
        self.verticalLayout_41.setSpacing(6)
        self.verticalLayout_41.setObjectName(u"verticalLayout_41")
        self.verticalLayout_41.setContentsMargins(6, 18, 6, 9)
        self.inner_hframe = QFrame(self.groupBox_inner_base_acquisition_selection)
        self.inner_hframe.setObjectName(u"inner_hframe")
        self.inner_hframe.setMinimumSize(QSize(0, 0))
        self.inner_hframe.setStyleSheet(u"")
        self.inner_hframe.setFrameShape(QFrame.StyledPanel)
        self.inner_hframe.setFrameShadow(QFrame.Raised)
        self.horizontalLayout_36 = QHBoxLayout(self.inner_hframe)
        self.horizontalLayout_36.setSpacing(9)
        self.horizontalLayout_36.setObjectName(u"horizontalLayout_36")
        self.horizontalLayout_36.setContentsMargins(2, 0, 2, 0)
        self.base_acq_folder_label = QLabel(self.inner_hframe)
        self.base_acq_folder_label.setObjectName(u"base_acq_folder_label")
        sizePolicy1.setHeightForWidth(self.base_acq_folder_label.sizePolicy().hasHeightForWidth())
        self.base_acq_folder_label.setSizePolicy(sizePolicy1)
        self.base_acq_folder_label.setMinimumSize(QSize(75, 0))
        self.base_acq_folder_label.setStyleSheet(u"border-color:transparent")
        self.base_acq_folder_label.setTextFormat(Qt.MarkdownText)

        self.horizontalLayout_36.addWidget(self.base_acq_folder_label)

        self.base_acq_folder_textEdit = QLineEdit(self.inner_hframe)
        self.base_acq_folder_textEdit.setObjectName(u"base_acq_folder_textEdit")
        self.base_acq_folder_textEdit.setMinimumSize(QSize(0, 30))
        self.base_acq_folder_textEdit.setMaximumSize(QSize(16777215, 30))
        self.base_acq_folder_textEdit.setStyleSheet(u"QLineEdit {\n"
"	height: 25px;\n"
"	background-color: rgb(27, 29, 35);\n"
"	border-radius: 5px;\n"
"	border: 2px solid rgb(27, 29, 35);\n"
"	padding-left: 10px;\n"
"}\n"
"QLineEdit:hover {\n"
"	border: 2px solid rgb(64, 71, 88);\n"
"}\n"
"QLineEdit:focus {\n"
"	border: 2px solid rgb(91, 101, 124);\n"
"}\n"
"QLineEdit[text=\"\"]{\n"
"	color:rgb(90,90,90);\n"
"}\n"
"\n"
"QLineEdit:disabled {\n"
"	color: rgb(60,60,60);\n"
"	background-color: rgb(60, 60, 60);\n"
"	border: 2px solid rgb(32, 32, 32);\n"
"}")

        self.horizontalLayout_36.addWidget(self.base_acq_folder_textEdit)

        self.base_acq_folder_button = QPushButton(self.inner_hframe)
        self.base_acq_folder_button.setObjectName(u"base_acq_folder_button")
        sizePolicy1.setHeightForWidth(self.base_acq_folder_button.sizePolicy().hasHeightForWidth())
        self.base_acq_folder_button.setSizePolicy(sizePolicy1)
        self.base_acq_folder_button.setMinimumSize(QSize(80, 45))
        self.base_acq_folder_button.setMaximumSize(QSize(16777215, 16777215))
        self.base_acq_folder_button.setStyleSheet(u"QPushButton {\n"
"	border: 2px solid rgb(52, 59, 72);\n"
"	border-radius: 5px;	\n"
"	background-color: rgb(52, 59, 72);\n"
"}\n"
"QPushButton:hover {\n"
"	background-color: rgb(57, 65, 80);\n"
"	border: 2px solid rgb(61, 70, 86);\n"
"}\n"
"QPushButton:pressed {	\n"
"	background-color: rgb(35, 40, 49);\n"
"	border: 2px solid rgb(43, 50, 61);\n"
"}\n"
"QPushButton:disabled {	\n"
"	background-color: rgb(35, 40, 49);\n"
"	border: 2px solid rgb(43, 50, 61);\n"
"}")
        self.base_acq_folder_button.setIcon(icon1)

        self.horizontalLayout_36.addWidget(self.base_acq_folder_button)


        self.verticalLayout_41.addWidget(self.inner_hframe)


        self.horizontalLayout_34.addWidget(self.groupBox_inner_base_acquisition_selection)


        self.verticalLayout_14.addWidget(self.groupBox_base_acquisition_selection)

        self.groupBox_acquisitions_list = QGroupBox(self.acq_upload_frame_content)
        self.groupBox_acquisitions_list.setObjectName(u"groupBox_acquisitions_list")
        self.groupBox_acquisitions_list.setMinimumSize(QSize(300, 0))
        self.groupBox_acquisitions_list.setStyleSheet(u"QGroupBox {\n"
"	border-radius: 5px;\n"
"	border: 2px solid rgb(27, 29, 35);\n"
"}\n"
"\n"
"QRadioButton::indicator {\n"
"    border: 3px solid rgb(52, 59, 72);\n"
"	width: 15px;\n"
"	height: 15px;\n"
"	border-radius: 10px;\n"
"    background: rgb(44, 49, 60);\n"
"}\n"
"QRadioButton::indicator:hover {\n"
"    border: 3px solid rgb(58, 66, 81);\n"
"}\n"
"QRadioButton::indicator:checked {\n"
"    background: 3px solid rgb(94, 106, 130);\n"
"	border: 3px solid rgb(52, 59, 72);	\n"
"}")
        self.groupBox_acquisitions_list.setFlat(False)
        self.groupBox_acquisitions_list.setCheckable(False)
        self.verticalLayout_47 = QVBoxLayout(self.groupBox_acquisitions_list)
        self.verticalLayout_47.setSpacing(6)
        self.verticalLayout_47.setObjectName(u"verticalLayout_47")
        self.verticalLayout_47.setContentsMargins(9, 18, 9, 9)
        self.acquisitions_listWidget = QListWidget(self.groupBox_acquisitions_list)
        self.acquisitions_listWidget.setObjectName(u"acquisitions_listWidget")
        self.acquisitions_listWidget.setTabKeyNavigation(False)
        self.acquisitions_listWidget.setProperty("showDropIndicator", False)
        self.acquisitions_listWidget.setDragEnabled(False)
        self.acquisitions_listWidget.setDragDropOverwriteMode(False)
        self.acquisitions_listWidget.setDragDropMode(QAbstractItemView.NoDragDrop)
        self.acquisitions_listWidget.setDefaultDropAction(Qt.CopyAction)
        self.acquisitions_listWidget.setAlternatingRowColors(False)
        self.acquisitions_listWidget.setSelectionMode(QAbstractItemView.ExtendedSelection)
        self.acquisitions_listWidget.setTextElideMode(Qt.ElideNone)
        self.acquisitions_listWidget.setMovement(QListView.Static)
        self.acquisitions_listWidget.setResizeMode(QListView.Fixed)
        self.acquisitions_listWidget.setViewMode(QListView.ListMode)

        self.verticalLayout_47.addWidget(self.acquisitions_listWidget)


        self.verticalLayout_14.addWidget(self.groupBox_acquisitions_list)

        self.groupBox_upload_settings = QGroupBox(self.acq_upload_frame_content)
        self.groupBox_upload_settings.setObjectName(u"groupBox_upload_settings")
        self.groupBox_upload_settings.setMinimumSize(QSize(300, 0))
        self.groupBox_upload_settings.setStyleSheet(u"QGroupBox {\n"
"	border-radius: 5px;\n"
"	border: 2px solid rgb(27, 29, 35);\n"
"}\n"
"\n"
"QRadioButton::indicator {\n"
"    border: 3px solid rgb(52, 59, 72);\n"
"	width: 15px;\n"
"	height: 15px;\n"
"	border-radius: 10px;\n"
"    background: rgb(44, 49, 60);\n"
"}\n"
"QRadioButton::indicator:hover {\n"
"    border: 3px solid rgb(58, 66, 81);\n"
"}\n"
"QRadioButton::indicator:checked {\n"
"    background: 3px solid rgb(94, 106, 130);\n"
"	border: 3px solid rgb(52, 59, 72);	\n"
"}")
        self.groupBox_upload_settings.setFlat(False)
        self.groupBox_upload_settings.setCheckable(False)
        self.verticalLayout_48 = QVBoxLayout(self.groupBox_upload_settings)
        self.verticalLayout_48.setSpacing(6)
        self.verticalLayout_48.setObjectName(u"verticalLayout_48")
        self.verticalLayout_48.setContentsMargins(9, 18, 9, 9)
        self.upload_acquisition_button = QPushButton(self.groupBox_upload_settings)
        self.upload_acquisition_button.setObjectName(u"upload_acquisition_button")
        sizePolicy1.setHeightForWidth(self.upload_acquisition_button.sizePolicy().hasHeightForWidth())
        self.upload_acquisition_button.setSizePolicy(sizePolicy1)
        self.upload_acquisition_button.setMinimumSize(QSize(60, 30))
        self.upload_acquisition_button.setMaximumSize(QSize(16777215, 30))
        self.upload_acquisition_button.setStyleSheet(u"QPushButton {\n"
"	border: 2px solid rgb(52, 59, 72);\n"
"	border-radius: 5px;	\n"
"	background-color: rgb(52, 59, 72);\n"
"}\n"
"QPushButton:hover {\n"
"	background-color: rgb(57, 65, 80);\n"
"	border: 2px solid rgb(61, 70, 86);\n"
"}\n"
"QPushButton:pressed {	\n"
"	background-color: rgb(35, 40, 49);\n"
"	border: 2px solid rgb(43, 50, 61);\n"
"}\n"
"QPushButton:disabled {	\n"
"	background-color: rgb(35, 40, 49);\n"
"	border: 2px solid rgb(43, 50, 61);\n"
"}")
        self.upload_acquisition_button.setIcon(icon1)

        self.verticalLayout_48.addWidget(self.upload_acquisition_button)


        self.verticalLayout_14.addWidget(self.groupBox_upload_settings)


        self.verticalLayout_18.addWidget(self.acq_upload_frame_content)


        self.verticalLayout_24.addWidget(self.acq_upload_frame)

        self.scrollArea_acq_upload.setWidget(self.widget_acq_upload)
        self.splitter_2.addWidget(self.scrollArea_acq_upload)

        self.verticalLayout_15.addWidget(self.splitter_2)

        self.stacked_widget.addWidget(self.page_experimental_features)
        self.page_app_log_file = QWidget()
        self.page_app_log_file.setObjectName(u"page_app_log_file")
        self.verticalLayout_10 = QVBoxLayout(self.page_app_log_file)
        self.verticalLayout_10.setObjectName(u"verticalLayout_10")
        self.frame = QFrame(self.page_app_log_file)
        self.frame.setObjectName(u"frame")
        sizePolicy1.setHeightForWidth(self.frame.sizePolicy().hasHeightForWidth())
        self.frame.setSizePolicy(sizePolicy1)
        self.frame.setStyleSheet(u"border-radius: 5px;")
        self.frame.setFrameShape(QFrame.StyledPanel)
        self.frame.setFrameShadow(QFrame.Raised)
        self.verticalLayout_8 = QVBoxLayout(self.frame)
        self.verticalLayout_8.setSpacing(0)
        self.verticalLayout_8.setObjectName(u"verticalLayout_8")
        self.verticalLayout_8.setContentsMargins(0, 0, 0, 0)
        self.log_file_title = QLabel(self.frame)
        self.log_file_title.setObjectName(u"log_file_title")
        self.log_file_title.setMinimumSize(QSize(0, 0))
        self.log_file_title.setStyleSheet(u"font: 700 12pt \"Segoe UI\";\n"
"background-color: rgb(39, 44, 54);")
        self.log_file_title.setMargin(9)
        self.log_file_title.setIndent(3)

        self.verticalLayout_8.addWidget(self.log_file_title)

        self.log_file_textEdit = QTextEdit(self.frame)
        self.log_file_textEdit.setObjectName(u"log_file_textEdit")
        self.log_file_textEdit.setStyleSheet(u"")

        self.verticalLayout_8.addWidget(self.log_file_textEdit)


        self.verticalLayout_10.addWidget(self.frame)

        self.stacked_widget.addWidget(self.page_app_log_file)

        self.verticalLayout_9.addWidget(self.stacked_widget)


        self.verticalLayout_4.addWidget(self.frame_content)

        self.frame_grip = QFrame(self.frame_content_right)
        self.frame_grip.setObjectName(u"frame_grip")
        self.frame_grip.setMinimumSize(QSize(0, 25))
        self.frame_grip.setMaximumSize(QSize(16777215, 25))
        self.frame_grip.setStyleSheet(u"background-color: rgb(33, 37, 43);")
        self.frame_grip.setFrameShape(QFrame.NoFrame)
        self.frame_grip.setFrameShadow(QFrame.Raised)
        self.horizontalLayout_6 = QHBoxLayout(self.frame_grip)
        self.horizontalLayout_6.setSpacing(0)
        self.horizontalLayout_6.setObjectName(u"horizontalLayout_6")
        self.horizontalLayout_6.setContentsMargins(0, 0, 2, 0)
        self.frame_label_bottom = QFrame(self.frame_grip)
        self.frame_label_bottom.setObjectName(u"frame_label_bottom")
        self.frame_label_bottom.setFrameShape(QFrame.NoFrame)
        self.frame_label_bottom.setFrameShadow(QFrame.Raised)
        self.horizontalLayout_7 = QHBoxLayout(self.frame_label_bottom)
        self.horizontalLayout_7.setSpacing(0)
        self.horizontalLayout_7.setObjectName(u"horizontalLayout_7")
        self.horizontalLayout_7.setContentsMargins(10, 0, 10, 0)
        self.label_credits = QLabel(self.frame_label_bottom)
        self.label_credits.setObjectName(u"label_credits")
        self.label_credits.setFont(font1)
        self.label_credits.setStyleSheet(u"color: rgb(98, 103, 111);")

        self.horizontalLayout_7.addWidget(self.label_credits)

        self.label_version = QLabel(self.frame_label_bottom)
        self.label_version.setObjectName(u"label_version")
        self.label_version.setMaximumSize(QSize(100, 16777215))
        self.label_version.setFont(font1)
        self.label_version.setStyleSheet(u"color: rgb(98, 103, 111);")
        self.label_version.setAlignment(Qt.AlignRight|Qt.AlignTrailing|Qt.AlignVCenter)

        self.horizontalLayout_7.addWidget(self.label_version)


        self.horizontalLayout_6.addWidget(self.frame_label_bottom)


        self.verticalLayout_4.addWidget(self.frame_grip)


        self.horizontalLayout_2.addWidget(self.frame_content_right)


        self.verticalLayout.addWidget(self.frame_center)


        self.horizontalLayout.addWidget(self.frame_main)

        MainWindow.setCentralWidget(self.centralwidget)

        self.retranslateUi(MainWindow)

        self.stacked_widget.setCurrentIndex(2)


        QMetaObject.connectSlotsByName(MainWindow)
    # setupUi

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QCoreApplication.translate("MainWindow", u"ST DTDL GUI", None))
        self.menu_btn_connection.setText("")
        self.menu_btn_device_conf.setText("")
#if QT_CONFIG(tooltip)
        self.menu_btn_experimental_features.setToolTip(QCoreApplication.translate("MainWindow", u"Experimental Features", None))
#endif // QT_CONFIG(tooltip)
        self.menu_btn_experimental_features.setText("")
        self.menu_btn_about.setText("")
#if QT_CONFIG(tooltip)
        self.menu_btn_show_log_file.setToolTip(QCoreApplication.translate("MainWindow", u"Show Application Log File", None))
#endif // QT_CONFIG(tooltip)
        self.menu_btn_show_log_file.setText("")
        self.label_app_title.setText(QCoreApplication.translate("MainWindow", u"Title", None))
        self.select_all_label.setText(QCoreApplication.translate("MainWindow", u"Select all:", None))
        self.label.setText(QCoreApplication.translate("MainWindow", u"<html><head/><body><p align=\"center\"><span style=\" font-size:24px; color:#ffffff;\">Welcome to the<br/></span><span style=\" font-size:36px; font-weight:700; color:#ffffff;\">Experimental Features page<br/></span><span style=\" font-size:24px; color:#ffffff;\">Here, you can explore and activate new functionalities that are currently in development, giving you an exclusive preview of upcoming innovations and enhancements</span></p></body></html>", None))
        self.dt_enabled_checkBox.setText(QCoreApplication.translate("MainWindow", u"Data Toolkit *", None))
        self.groupBox_dt_settings.setTitle(QCoreApplication.translate("MainWindow", u"Data Toolkit plugins folder", None))
        self.dt_plugins_folder_button.setText(QCoreApplication.translate("MainWindow", u"Browse", None))
        self.groupBox_dt_plugin_list.setTitle(QCoreApplication.translate("MainWindow", u"Selected Plugins List", None))
        self.label_2.setText(QCoreApplication.translate("MainWindow", u"<html><head/><body><p><span style=\" font-size:12pt; font-weight:700;\">*</span>By enabling this feature, the data streams acquired from the sensors of a connected board pass through a data pipeline composed of a series of plugins. Along with the PythonSDK, in the examples folder, we provide a folder named <span style=\" font-weight:700; text-decoration: underline;\">data_toolkit_plugins</span> that contains three example plugins. Each plugin must implement a function named <span style=\" font-weight:700;\">process</span>, which processes the data received from the previous plugin and passes it to the next plugin. Additionally, each plugin can implement a function named <span style=\" font-weight:700;\">create_plot_widget</span>, which should return a QWidget that is automatically inserted into a dedicated section of this GUI (on the acquisition management page).</p></body></html>", None))
        self.acq_upload_title_2.setText(QCoreApplication.translate("MainWindow", u"Coming soon...", None))
        self.acq_upload_title.setText(QCoreApplication.translate("MainWindow", u"ST AIoT Craft Acquisitions upload", None))
        self.groupBox_inner_login.setTitle(QCoreApplication.translate("MainWindow", u"ST AIoT Craft - User Login", None))
        self.login_button.setText(QCoreApplication.translate("MainWindow", u"Login", None))
        self.groupBox_inner_base_acquisition_selection.setTitle(QCoreApplication.translate("MainWindow", u"Acquisitions Base Folder", None))
        self.base_acq_folder_label.setText(QCoreApplication.translate("MainWindow", u"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><meta charset=\"utf-8\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"hr { height: 1px; border-width: 0; }\n"
"li.unchecked::marker { content: \"\\2610\"; }\n"
"li.checked::marker { content: \"\\2612\"; }\n"
"</style></head><body style=\" font-family:'Segoe UI'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Acquisition<br />Folder</p></body></html>", None))
        self.base_acq_folder_button.setText(QCoreApplication.translate("MainWindow", u"Browse", None))
        self.groupBox_acquisitions_list.setTitle(QCoreApplication.translate("MainWindow", u"Acquisitions List", None))
        self.groupBox_upload_settings.setTitle(QCoreApplication.translate("MainWindow", u"Upload Settings", None))
        self.upload_acquisition_button.setText(QCoreApplication.translate("MainWindow", u"Upload", None))
        self.log_file_title.setText(QCoreApplication.translate("MainWindow", u"Log file Title:", None))
        self.label_credits.setText(QCoreApplication.translate("MainWindow", u"ST DTDL GUI v1.0.0", None))
        self.label_version.setText(QCoreApplication.translate("MainWindow", u"v1.0.0", None))
    # retranslateUi

