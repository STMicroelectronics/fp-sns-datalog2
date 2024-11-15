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
from PySide6.QtWidgets import (QApplication, QFrame, QGroupBox, QHBoxLayout,
    QLabel, QLineEdit, QMainWindow, QPushButton,
    QScrollArea, QSizePolicy, QSlider, QSpacerItem,
    QSplitter, QVBoxLayout, QWidget)
import Assisted_Segmentation_GUI.gui.UI.resources_rc

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        if not MainWindow.objectName():
            MainWindow.setObjectName(u"MainWindow")
        MainWindow.resize(1365, 859)
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
"    background-color: rgb(106, 234, 225);\n"
"}\n"
"QSlider::handle:horizontal:pressed {\n"
"    background-color: rgb(137, 255, 254);\n"
"}\n"
"QSlider::sub-page:horiz"
                        "ontal {\n"
"   background-color: rgb(0, 151, 144);\n"
"   border-radius: 9px;\n"
"}\n"
"\n"
"QSlider::groove:vertical {\n"
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
"    background-color: rgb(106, 234, 225);\n"
"}\n"
"QSlider::handle:vertical:pressed {\n"
"    background-color: rgb(137, 255, 254);\n"
"}\n"
"QSlider::sub-page:vertical {\n"
"   background-color: rgb(0, 151, 144);\n"
"   border-radius: 9px;\n"
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
"    text-al"
                        "ign: center;\n"
"}\n"
"\n"
"QProgressBar::chunk {\n"
"    background-color: rgb(32, 178, 170);\n"
"    margin: 4px;\n"
"    border-bottom-right-radius: 6px;\n"
"    border-bottom-left-radius: 6px;\n"
"    border-top-right-radius: 6px;\n"
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
        self.menu_btn_info = QPushButton(self.frame_extra_menus)
        self.menu_btn_info.setObjectName(u"menu_btn_info")
        self.menu_btn_info.setEnabled(True)
        sizePolicy2 = QSizePolicy(QSizePolicy.Expanding, QSizePolicy.Fixed)
        sizePolicy2.setHorizontalStretch(0)
        sizePolicy2.setVerticalStretch(0)
        sizePolicy2.setHeightForWidth(self.menu_btn_info.sizePolicy().hasHeightForWidth())
        self.menu_btn_info.setSizePolicy(sizePolicy2)
        self.menu_btn_info.setMinimumSize(QSize(0, 60))
        font1 = QFont()
        self.menu_btn_info.setFont(font1)
        self.menu_btn_info.setLayoutDirection(Qt.LeftToRight)
        self.menu_btn_info.setStyleSheet(u"QPushButton {	\n"
"	background-image: url(:/icons/icons/outline_info_white_18dp.png);\n"
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

        self.layout_menu_bottom.addWidget(self.menu_btn_info)


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
        self.frame_device_config = QFrame(self.frame_content)
        self.frame_device_config.setObjectName(u"frame_device_config")
        self.frame_device_config.setEnabled(True)
        self.frame_device_config.setStyleSheet(u"")
        self.verticalLayout_2 = QVBoxLayout(self.frame_device_config)
        self.verticalLayout_2.setObjectName(u"verticalLayout_2")
        self.splitter = QSplitter(self.frame_device_config)
        self.splitter.setObjectName(u"splitter")
        sizePolicy.setHeightForWidth(self.splitter.sizePolicy().hasHeightForWidth())
        self.splitter.setSizePolicy(sizePolicy)
        self.splitter.setOrientation(Qt.Horizontal)
        self.splitter.setOpaqueResize(True)
        self.scrollArea_settings = QScrollArea(self.splitter)
        self.scrollArea_settings.setObjectName(u"scrollArea_settings")
        sizePolicy3 = QSizePolicy(QSizePolicy.Fixed, QSizePolicy.Expanding)
        sizePolicy3.setHorizontalStretch(0)
        sizePolicy3.setVerticalStretch(0)
        sizePolicy3.setHeightForWidth(self.scrollArea_settings.sizePolicy().hasHeightForWidth())
        self.scrollArea_settings.setSizePolicy(sizePolicy3)
        self.scrollArea_settings.setMinimumSize(QSize(300, 0))
        self.scrollArea_settings.setMaximumSize(QSize(300, 16777215))
        self.scrollArea_settings.setWidgetResizable(True)
        self.widget_device_config_main = QWidget()
        self.widget_device_config_main.setObjectName(u"widget_device_config_main")
        self.widget_device_config_main.setGeometry(QRect(0, 0, 296, 782))
        self.verticalLayout_3 = QVBoxLayout(self.widget_device_config_main)
        self.verticalLayout_3.setSpacing(0)
        self.verticalLayout_3.setObjectName(u"verticalLayout_3")
        self.verticalLayout_3.setContentsMargins(3, 3, 3, 3)
        self.acq_upload_frame = QFrame(self.widget_device_config_main)
        self.acq_upload_frame.setObjectName(u"acq_upload_frame")
        self.acq_upload_frame.setFrameShape(QFrame.StyledPanel)
        self.acq_upload_frame.setFrameShadow(QFrame.Raised)
        self.verticalLayout_18 = QVBoxLayout(self.acq_upload_frame)
        self.verticalLayout_18.setObjectName(u"verticalLayout_18")
        self.verticalLayout_18.setContentsMargins(0, 0, 0, 0)
        self.settings_title_frame = QFrame(self.acq_upload_frame)
        self.settings_title_frame.setObjectName(u"settings_title_frame")
        self.settings_title_frame.setMaximumSize(QSize(16777215, 35))
        self.settings_title_frame.setCursor(QCursor(Qt.ArrowCursor))
        self.settings_title_frame.setStyleSheet(u"QFrame {\n"
"	border: transparent;\n"
"	background-color: rgb(39, 44, 54);\n"
"}")
        self.settings_title_frame.setFrameShape(QFrame.StyledPanel)
        self.settings_title_frame.setFrameShadow(QFrame.Raised)
        self.horizontalLayout_28 = QHBoxLayout(self.settings_title_frame)
        self.horizontalLayout_28.setObjectName(u"horizontalLayout_28")
        self.horizontalLayout_28.setContentsMargins(9, 9, 9, 9)
        self.settings_title_label = QLabel(self.settings_title_frame)
        self.settings_title_label.setObjectName(u"settings_title_label")
        self.settings_title_label.setStyleSheet(u"QLabel {\n"
"   text-align: left;\n"
"   font-size: 10pt;\n"
"   font-weight: bold;\n"
"}")

        self.horizontalLayout_28.addWidget(self.settings_title_label)


        self.verticalLayout_18.addWidget(self.settings_title_frame)

        self.groupBox_acquisition_selection = QGroupBox(self.acq_upload_frame)
        self.groupBox_acquisition_selection.setObjectName(u"groupBox_acquisition_selection")
        self.groupBox_acquisition_selection.setMinimumSize(QSize(0, 0))
        self.groupBox_acquisition_selection.setStyleSheet(u"QGroupBox {\n"
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
        self.groupBox_acquisition_selection.setFlat(False)
        self.groupBox_acquisition_selection.setCheckable(False)
        self.verticalLayout_41 = QVBoxLayout(self.groupBox_acquisition_selection)
        self.verticalLayout_41.setSpacing(6)
        self.verticalLayout_41.setObjectName(u"verticalLayout_41")
        self.verticalLayout_41.setContentsMargins(6, 18, 6, 9)
        self.acquisition_selection_frame = QFrame(self.groupBox_acquisition_selection)
        self.acquisition_selection_frame.setObjectName(u"acquisition_selection_frame")
        self.acquisition_selection_frame.setMinimumSize(QSize(0, 0))
        self.acquisition_selection_frame.setStyleSheet(u"")
        self.acquisition_selection_frame.setFrameShape(QFrame.StyledPanel)
        self.acquisition_selection_frame.setFrameShadow(QFrame.Raised)
        self.horizontalLayout_39 = QHBoxLayout(self.acquisition_selection_frame)
        self.horizontalLayout_39.setSpacing(9)
        self.horizontalLayout_39.setObjectName(u"horizontalLayout_39")
        self.horizontalLayout_39.setContentsMargins(2, 0, 2, 0)
        self.acq_folder_textEdit = QLineEdit(self.acquisition_selection_frame)
        self.acq_folder_textEdit.setObjectName(u"acq_folder_textEdit")
        self.acq_folder_textEdit.setMinimumSize(QSize(0, 30))
        self.acq_folder_textEdit.setMaximumSize(QSize(16777215, 30))
        self.acq_folder_textEdit.setStyleSheet(u"QLineEdit {\n"
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

        self.horizontalLayout_39.addWidget(self.acq_folder_textEdit)

        self.acq_folder_button = QPushButton(self.acquisition_selection_frame)
        self.acq_folder_button.setObjectName(u"acq_folder_button")
        sizePolicy1.setHeightForWidth(self.acq_folder_button.sizePolicy().hasHeightForWidth())
        self.acq_folder_button.setSizePolicy(sizePolicy1)
        self.acq_folder_button.setMinimumSize(QSize(30, 30))
        self.acq_folder_button.setMaximumSize(QSize(16777215, 16777215))
        self.acq_folder_button.setStyleSheet(u"QPushButton {\n"
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
        self.acq_folder_button.setIcon(icon1)

        self.horizontalLayout_39.addWidget(self.acq_folder_button)


        self.verticalLayout_41.addWidget(self.acquisition_selection_frame)


        self.verticalLayout_18.addWidget(self.groupBox_acquisition_selection)

        self.groupBox_components = QGroupBox(self.acq_upload_frame)
        self.groupBox_components.setObjectName(u"groupBox_components")
        self.groupBox_components.setMinimumSize(QSize(0, 0))
        self.groupBox_components.setStyleSheet(u"QGroupBox {\n"
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
        self.groupBox_components.setFlat(False)
        self.groupBox_components.setCheckable(False)
        self.verticalLayout_47 = QVBoxLayout(self.groupBox_components)
        self.verticalLayout_47.setSpacing(6)
        self.verticalLayout_47.setObjectName(u"verticalLayout_47")
        self.verticalLayout_47.setContentsMargins(9, 18, 9, 9)
        self.message_frame = QFrame(self.groupBox_components)
        self.message_frame.setObjectName(u"message_frame")
        sizePolicy1.setHeightForWidth(self.message_frame.sizePolicy().hasHeightForWidth())
        self.message_frame.setSizePolicy(sizePolicy1)
        self.message_frame.setMinimumSize(QSize(0, 30))
        self.message_frame.setStyleSheet(u"")
        self.message_frame.setFrameShape(QFrame.StyledPanel)
        self.message_frame.setFrameShadow(QFrame.Raised)
        self.verticalLayout_14 = QVBoxLayout(self.message_frame)
        self.verticalLayout_14.setSpacing(0)
        self.verticalLayout_14.setObjectName(u"verticalLayout_14")
        self.verticalLayout_14.setContentsMargins(0, 0, 0, 0)
        self.message_label = QLabel(self.message_frame)
        self.message_label.setObjectName(u"message_label")

        self.verticalLayout_14.addWidget(self.message_label)


        self.verticalLayout_47.addWidget(self.message_frame)

        self.components_frame = QFrame(self.groupBox_components)
        self.components_frame.setObjectName(u"components_frame")
        sizePolicy1.setHeightForWidth(self.components_frame.sizePolicy().hasHeightForWidth())
        self.components_frame.setSizePolicy(sizePolicy1)
        self.components_frame.setMinimumSize(QSize(0, 0))
        self.components_frame.setStyleSheet(u"")
        self.components_frame.setFrameShape(QFrame.StyledPanel)
        self.components_frame.setFrameShadow(QFrame.Raised)
        self.verticalLayout_17 = QVBoxLayout(self.components_frame)
        self.verticalLayout_17.setSpacing(0)
        self.verticalLayout_17.setObjectName(u"verticalLayout_17")
        self.verticalLayout_17.setContentsMargins(0, 0, 0, 0)

        self.verticalLayout_47.addWidget(self.components_frame)

        self.spacer_frame = QFrame(self.groupBox_components)
        self.spacer_frame.setObjectName(u"spacer_frame")
        sizePolicy1.setHeightForWidth(self.spacer_frame.sizePolicy().hasHeightForWidth())
        self.spacer_frame.setSizePolicy(sizePolicy1)
        self.spacer_frame.setMinimumSize(QSize(0, 0))
        self.spacer_frame.setStyleSheet(u"QPushButton {\n"
"	border: 2px solid rgb(52, 59, 72);\n"
"	border-radius: 5px;	\n"
"	background-color: rgb(52, 59, 72);\n"
"	font: 700 12pt \"Segoe UI\";\n"
"}\n"
"QPushButton:hover {\n"
"	background-color: rgb(57, 65, 80);\n"
"	border: 2px solid rgb(61, 70, 86);\n"
"}\n"
"QPushButton:pressed {	\n"
"	background-color: rgb(35, 40, 49);\n"
"	border: 2px solid rgb(43, 50, 61);\n"
"}\n"
"QPushButton:checked {\n"
"     border: 2px solid rgb(51, 71, 51);\n"
"     border-radius: 5px;	\n"
"     background-color: rgb(51, 71, 51); \n"
"}\n"
"QPushButton:disabled {	\n"
"	background-color: rgb(35, 40, 49);\n"
"	border: 2px solid rgb(43, 50, 61);\n"
"	color: rgb(43, 50, 61);\n"
"}")
        self.spacer_frame.setFrameShape(QFrame.StyledPanel)
        self.spacer_frame.setFrameShadow(QFrame.Raised)
        self.verticalLayout_15 = QVBoxLayout(self.spacer_frame)
        self.verticalLayout_15.setSpacing(9)
        self.verticalLayout_15.setObjectName(u"verticalLayout_15")
        self.verticalLayout_15.setContentsMargins(0, 0, 0, 0)
        self.verticalSpacer = QSpacerItem(20, 40, QSizePolicy.Minimum, QSizePolicy.Expanding)

        self.verticalLayout_15.addItem(self.verticalSpacer)


        self.verticalLayout_47.addWidget(self.spacer_frame)


        self.verticalLayout_18.addWidget(self.groupBox_components)

        self.run_segmentation_button = QPushButton(self.acq_upload_frame)
        self.run_segmentation_button.setObjectName(u"run_segmentation_button")
        self.run_segmentation_button.setEnabled(True)
        sizePolicy1.setHeightForWidth(self.run_segmentation_button.sizePolicy().hasHeightForWidth())
        self.run_segmentation_button.setSizePolicy(sizePolicy1)
        self.run_segmentation_button.setMinimumSize(QSize(60, 60))
        self.run_segmentation_button.setMaximumSize(QSize(16777215, 60))
        font2 = QFont()
        font2.setPointSize(16)
        font2.setBold(True)
        self.run_segmentation_button.setFont(font2)
        self.run_segmentation_button.setStyleSheet(u"QPushButton {\n"
"            border: 2px solid rgb(51, 71, 51);\n"
"            border-radius: 5px;	\n"
"            background-color: rgb(51, 71, 51);    \n"
"        }\n"
"        QPushButton:hover {\n"
"            background-color: rgb(57, 81, 57);\n"
"            border: 2px solid rgb(61, 87, 61);\n"
"        }\n"
"        QPushButton:pressed {	\n"
"            background-color: rgb(34, 48, 34);\n"
"            border: 2px solid rgb(42, 60, 42);\n"
"        }\n"
"        QPushButton:disabled {\n"
"            background-color: rgb(29,33,41);\n"
"            border: 2px solid rgb(60, 42, 42);\n"
"        }")
        icon2 = QIcon()
        icon2.addFile(u":/icons/icons/outline_play_arrow_white_18dp.png", QSize(), QIcon.Normal, QIcon.Off)
        self.run_segmentation_button.setIcon(icon2)
        self.run_segmentation_button.setIconSize(QSize(64, 64))

        self.verticalLayout_18.addWidget(self.run_segmentation_button)

        self.tag_acquisition_button = QPushButton(self.acq_upload_frame)
        self.tag_acquisition_button.setObjectName(u"tag_acquisition_button")
        self.tag_acquisition_button.setEnabled(True)
        sizePolicy1.setHeightForWidth(self.tag_acquisition_button.sizePolicy().hasHeightForWidth())
        self.tag_acquisition_button.setSizePolicy(sizePolicy1)
        self.tag_acquisition_button.setMinimumSize(QSize(60, 60))
        self.tag_acquisition_button.setMaximumSize(QSize(16777215, 60))
        self.tag_acquisition_button.setFont(font2)
        self.tag_acquisition_button.setStyleSheet(u"QPushButton {\n"
"            border: 2px solid rgb(51, 71, 51);\n"
"            border-radius: 5px;	\n"
"            background-color: rgb(51, 71, 51);    \n"
"        }\n"
"        QPushButton:hover {\n"
"            background-color: rgb(57, 81, 57);\n"
"            border: 2px solid rgb(61, 87, 61);\n"
"        }\n"
"        QPushButton:pressed {	\n"
"            background-color: rgb(34, 48, 34);\n"
"            border: 2px solid rgb(42, 60, 42);\n"
"        }\n"
"        QPushButton:disabled {\n"
"            background-color: rgb(29,33,41);\n"
"            border: 2px solid rgb(60, 42, 42);\n"
"        }")
        icon3 = QIcon()
        icon3.addFile(u":/icons/icons/outline_edit_white_18dp.png", QSize(), QIcon.Normal, QIcon.Off)
        self.tag_acquisition_button.setIcon(icon3)
        self.tag_acquisition_button.setIconSize(QSize(64, 64))

        self.verticalLayout_18.addWidget(self.tag_acquisition_button)

        self.tag_acquisition_label = QLabel(self.acq_upload_frame)
        self.tag_acquisition_label.setObjectName(u"tag_acquisition_label")
        self.tag_acquisition_label.setStyleSheet(u"")

        self.verticalLayout_18.addWidget(self.tag_acquisition_label)


        self.verticalLayout_3.addWidget(self.acq_upload_frame)

        self.scrollArea_settings.setWidget(self.widget_device_config_main)
        self.splitter.addWidget(self.scrollArea_settings)
        self.scrollArea_plots = QScrollArea(self.splitter)
        self.scrollArea_plots.setObjectName(u"scrollArea_plots")
        self.scrollArea_plots.setWidgetResizable(True)
        self.widget_plots = QWidget()
        self.widget_plots.setObjectName(u"widget_plots")
        self.widget_plots.setGeometry(QRect(0, 0, 938, 782))
        sizePolicy4 = QSizePolicy(QSizePolicy.Expanding, QSizePolicy.Preferred)
        sizePolicy4.setHorizontalStretch(0)
        sizePolicy4.setVerticalStretch(0)
        sizePolicy4.setHeightForWidth(self.widget_plots.sizePolicy().hasHeightForWidth())
        self.widget_plots.setSizePolicy(sizePolicy4)
        self.verticalLayout_7 = QVBoxLayout(self.widget_plots)
        self.verticalLayout_7.setSpacing(6)
        self.verticalLayout_7.setObjectName(u"verticalLayout_7")
        self.verticalLayout_7.setContentsMargins(3, 3, 3, 3)
        self.frame_datastream = QFrame(self.widget_plots)
        self.frame_datastream.setObjectName(u"frame_datastream")
        self.frame_datastream.setMinimumSize(QSize(0, 0))
        self.frame_datastream.setMaximumSize(QSize(16777215, 16777215))
        self.frame_datastream.setStyleSheet(u"background-color: rgb(41, 45, 56);\n"
"border-radius: 5px;\n"
"color: rgb(210, 210, 210);\n"
"")
        self.frame_datastream.setFrameShape(QFrame.NoFrame)
        self.frame_datastream.setFrameShadow(QFrame.Raised)
        self.verticalLayout_28 = QVBoxLayout(self.frame_datastream)
        self.verticalLayout_28.setSpacing(0)
        self.verticalLayout_28.setObjectName(u"verticalLayout_28")
        self.verticalLayout_28.setContentsMargins(0, 0, 0, 0)
        self.frame_datastream_title = QFrame(self.frame_datastream)
        self.frame_datastream_title.setObjectName(u"frame_datastream_title")
        self.frame_datastream_title.setMaximumSize(QSize(16777215, 35))
        self.frame_datastream_title.setCursor(QCursor(Qt.ArrowCursor))
        self.frame_datastream_title.setStyleSheet(u"QFrame {\n"
"	border: transparent;\n"
"	background-color: rgb(39, 44, 54);\n"
"}")
        self.frame_datastream_title.setFrameShape(QFrame.StyledPanel)
        self.frame_datastream_title.setFrameShadow(QFrame.Raised)
        self.horizontalLayout_27 = QHBoxLayout(self.frame_datastream_title)
        self.horizontalLayout_27.setObjectName(u"horizontalLayout_27")
        self.horizontalLayout_27.setContentsMargins(9, 9, 9, 9)
        self.label_datastream_title = QPushButton(self.frame_datastream_title)
        self.label_datastream_title.setObjectName(u"label_datastream_title")
        sizePolicy4.setHeightForWidth(self.label_datastream_title.sizePolicy().hasHeightForWidth())
        self.label_datastream_title.setSizePolicy(sizePolicy4)
        self.label_datastream_title.setMaximumSize(QSize(16777215, 16777215))
        font3 = QFont()
        font3.setPointSize(10)
        font3.setBold(True)
        self.label_datastream_title.setFont(font3)
        self.label_datastream_title.setStyleSheet(u"QPushButton{\n"
"	text-align: left\n"
"}")

        self.horizontalLayout_27.addWidget(self.label_datastream_title)


        self.verticalLayout_28.addWidget(self.frame_datastream_title)

        self.frame_datastream_contents = QFrame(self.frame_datastream)
        self.frame_datastream_contents.setObjectName(u"frame_datastream_contents")
        self.frame_datastream_contents.setMinimumSize(QSize(0, 0))
        self.frame_datastream_contents.setMaximumSize(QSize(16777215, 16777215))
        self.frame_datastream_contents.setCursor(QCursor(Qt.ArrowCursor))
        self.frame_datastream_contents.setStyleSheet(u"QFrame {\n"
"	 border: transparent;\n"
"}")
        self.frame_datastream_contents.setFrameShadow(QFrame.Raised)
        self.horizontalLayout_33 = QHBoxLayout(self.frame_datastream_contents)
        self.horizontalLayout_33.setSpacing(0)
        self.horizontalLayout_33.setObjectName(u"horizontalLayout_33")
        self.horizontalLayout_33.setContentsMargins(0, 0, 0, 0)
        self.frame_datastream_plot_contents = QFrame(self.frame_datastream_contents)
        self.frame_datastream_plot_contents.setObjectName(u"frame_datastream_plot_contents")
        self.frame_datastream_plot_contents.setMinimumSize(QSize(0, 0))
        self.frame_datastream_plot_contents.setMaximumSize(QSize(16777215, 16777215))
        self.frame_datastream_plot_contents.setCursor(QCursor(Qt.ArrowCursor))
        self.frame_datastream_plot_contents.setStyleSheet(u"")
        self.frame_datastream_plot_contents.setFrameShadow(QFrame.Raised)
        self.verticalLayout_6 = QVBoxLayout(self.frame_datastream_plot_contents)
        self.verticalLayout_6.setSpacing(6)
        self.verticalLayout_6.setObjectName(u"verticalLayout_6")
        self.verticalLayout_6.setContentsMargins(0, 0, 0, 0)
        self.frame_datastream_plot = QFrame(self.frame_datastream_plot_contents)
        self.frame_datastream_plot.setObjectName(u"frame_datastream_plot")
        self.frame_datastream_plot.setMinimumSize(QSize(0, 0))
        self.frame_datastream_plot.setMaximumSize(QSize(16777215, 16777215))
        self.frame_datastream_plot.setCursor(QCursor(Qt.ArrowCursor))
        self.frame_datastream_plot.setStyleSheet(u"")
        self.frame_datastream_plot.setFrameShadow(QFrame.Raised)
        self.horizontalLayout_34 = QHBoxLayout(self.frame_datastream_plot)
        self.horizontalLayout_34.setSpacing(24)
        self.horizontalLayout_34.setObjectName(u"horizontalLayout_34")
        self.horizontalLayout_34.setContentsMargins(9, 9, 9, 9)

        self.verticalLayout_6.addWidget(self.frame_datastream_plot)

        self.frame_datastream_nav = QFrame(self.frame_datastream_plot_contents)
        self.frame_datastream_nav.setObjectName(u"frame_datastream_nav")
        self.frame_datastream_nav.setMinimumSize(QSize(0, 0))
        self.frame_datastream_nav.setMaximumSize(QSize(16777215, 20))
        self.frame_datastream_nav.setCursor(QCursor(Qt.ArrowCursor))
        self.frame_datastream_nav.setStyleSheet(u"")
        self.frame_datastream_nav.setFrameShadow(QFrame.Raised)
        self.horizontalLayout_35 = QHBoxLayout(self.frame_datastream_nav)
        self.horizontalLayout_35.setSpacing(6)
        self.horizontalLayout_35.setObjectName(u"horizontalLayout_35")
        self.horizontalLayout_35.setContentsMargins(10, 0, 10, 0)
        self.label_3 = QLabel(self.frame_datastream_nav)
        self.label_3.setObjectName(u"label_3")
        sizePolicy5 = QSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)
        sizePolicy5.setHorizontalStretch(0)
        sizePolicy5.setVerticalStretch(0)
        sizePolicy5.setHeightForWidth(self.label_3.sizePolicy().hasHeightForWidth())
        self.label_3.setSizePolicy(sizePolicy5)
        self.label_3.setMinimumSize(QSize(18, 18))
        self.label_3.setPixmap(QPixmap(u":/icons/icons/outline_sync_alt_white_18.png"))

        self.horizontalLayout_35.addWidget(self.label_3)

        self.slider_scroll_datastream = QSlider(self.frame_datastream_nav)
        self.slider_scroll_datastream.setObjectName(u"slider_scroll_datastream")
        self.slider_scroll_datastream.setValue(50)
        self.slider_scroll_datastream.setOrientation(Qt.Horizontal)

        self.horizontalLayout_35.addWidget(self.slider_scroll_datastream)


        self.verticalLayout_6.addWidget(self.frame_datastream_nav)


        self.horizontalLayout_33.addWidget(self.frame_datastream_plot_contents)

        self.frame_datastream_plot_settings = QFrame(self.frame_datastream_contents)
        self.frame_datastream_plot_settings.setObjectName(u"frame_datastream_plot_settings")
        self.frame_datastream_plot_settings.setMinimumSize(QSize(40, 0))
        self.frame_datastream_plot_settings.setMaximumSize(QSize(40, 16777215))
        self.frame_datastream_plot_settings.setCursor(QCursor(Qt.ArrowCursor))
        self.frame_datastream_plot_settings.setStyleSheet(u"QFrame {\n"
"	 border: transparent\n"
"}\n"
"QLineEdit {\n"
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
"QLineEdit:disabled {\n"
"	background-color: rgb(36, 40, 48);\n"
"	border: 2px solid rgb(32, 32, 32);\n"
"}\n"
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
"QComboBox:focus {\n"
"	border: 2px solid rgb(91, 101, 124);\n"
"}\n"
"QComboBox QAbstractItemView {\n"
"	color: rgb(85, 170, 255);	\n"
"	background-color: rgb(27, 29, 35);\n"
"	padding: 10px;\n"
"	selection-background-color: rgb(39, 44, 54);\n"
"}\n"
"QComboBox:disabled {\n"
"	background-color: rgb(36, 40, 48);\n"
"	border: 2px solid rgb(32"
                        ", 32, 32);\n"
"}\n"
"\n"
"/* RADIO BUTTON */\n"
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
"}\n"
"")
        self.frame_datastream_plot_settings.setFrameShadow(QFrame.Raised)
        self.verticalLayout_10 = QVBoxLayout(self.frame_datastream_plot_settings)
        self.verticalLayout_10.setSpacing(6)
        self.verticalLayout_10.setObjectName(u"verticalLayout_10")
        self.verticalLayout_10.setContentsMargins(9, 9, 9, 9)
        self.label = QLabel(self.frame_datastream_plot_settings)
        self.label.setObjectName(u"label")
        sizePolicy5.setHeightForWidth(self.label.sizePolicy().hasHeightForWidth())
        self.label.setSizePolicy(sizePolicy5)
        self.label.setPixmap(QPixmap(u":/icons/icons/outline_zoom_in_white_18dp.png"))

        self.verticalLayout_10.addWidget(self.label)

        self.slider_zoom_datastream = QSlider(self.frame_datastream_plot_settings)
        self.slider_zoom_datastream.setObjectName(u"slider_zoom_datastream")
        sizePolicy6 = QSizePolicy(QSizePolicy.Fixed, QSizePolicy.Minimum)
        sizePolicy6.setHorizontalStretch(0)
        sizePolicy6.setVerticalStretch(0)
        sizePolicy6.setHeightForWidth(self.slider_zoom_datastream.sizePolicy().hasHeightForWidth())
        self.slider_zoom_datastream.setSizePolicy(sizePolicy6)
        self.slider_zoom_datastream.setValue(50)
        self.slider_zoom_datastream.setOrientation(Qt.Vertical)

        self.verticalLayout_10.addWidget(self.slider_zoom_datastream)


        self.horizontalLayout_33.addWidget(self.frame_datastream_plot_settings)


        self.verticalLayout_28.addWidget(self.frame_datastream_contents)


        self.verticalLayout_7.addWidget(self.frame_datastream)

        self.frame_score = QFrame(self.widget_plots)
        self.frame_score.setObjectName(u"frame_score")
        self.frame_score.setMinimumSize(QSize(0, 0))
        self.frame_score.setMaximumSize(QSize(16777215, 16777215))
        self.frame_score.setStyleSheet(u"background-color: rgb(41, 45, 56);\n"
"border-radius: 5px;\n"
"color: rgb(210, 210, 210);\n"
"")
        self.frame_score.setFrameShape(QFrame.NoFrame)
        self.frame_score.setFrameShadow(QFrame.Raised)
        self.verticalLayout_26 = QVBoxLayout(self.frame_score)
        self.verticalLayout_26.setSpacing(0)
        self.verticalLayout_26.setObjectName(u"verticalLayout_26")
        self.verticalLayout_26.setContentsMargins(0, 0, 0, 0)
        self.frame_score_title = QFrame(self.frame_score)
        self.frame_score_title.setObjectName(u"frame_score_title")
        self.frame_score_title.setMaximumSize(QSize(16777215, 35))
        self.frame_score_title.setCursor(QCursor(Qt.ArrowCursor))
        self.frame_score_title.setStyleSheet(u"QFrame {\n"
"	border: transparent;\n"
"	background-color: rgb(39, 44, 54);\n"
"}")
        self.frame_score_title.setFrameShape(QFrame.StyledPanel)
        self.frame_score_title.setFrameShadow(QFrame.Raised)
        self.horizontalLayout_25 = QHBoxLayout(self.frame_score_title)
        self.horizontalLayout_25.setObjectName(u"horizontalLayout_25")
        self.horizontalLayout_25.setContentsMargins(9, 9, 9, 9)
        self.label_score_title = QPushButton(self.frame_score_title)
        self.label_score_title.setObjectName(u"label_score_title")
        sizePolicy4.setHeightForWidth(self.label_score_title.sizePolicy().hasHeightForWidth())
        self.label_score_title.setSizePolicy(sizePolicy4)
        self.label_score_title.setMaximumSize(QSize(16777215, 16777215))
        self.label_score_title.setFont(font3)
        self.label_score_title.setStyleSheet(u"QPushButton{\n"
"	text-align: left\n"
"}")

        self.horizontalLayout_25.addWidget(self.label_score_title)


        self.verticalLayout_26.addWidget(self.frame_score_title)

        self.frame_score_plot = QFrame(self.frame_score)
        self.frame_score_plot.setObjectName(u"frame_score_plot")
        self.frame_score_plot.setMinimumSize(QSize(0, 0))
        self.frame_score_plot.setCursor(QCursor(Qt.ArrowCursor))
        self.frame_score_plot.setStyleSheet(u"")
        self.frame_score_plot.setFrameShadow(QFrame.Raised)
        self.horizontalLayout_26 = QHBoxLayout(self.frame_score_plot)
        self.horizontalLayout_26.setSpacing(24)
        self.horizontalLayout_26.setObjectName(u"horizontalLayout_26")
        self.horizontalLayout_26.setContentsMargins(9, 9, 49, 9)

        self.verticalLayout_26.addWidget(self.frame_score_plot)


        self.verticalLayout_7.addWidget(self.frame_score)

        self.frame_focus_window = QFrame(self.widget_plots)
        self.frame_focus_window.setObjectName(u"frame_focus_window")
        self.frame_focus_window.setMinimumSize(QSize(0, 0))
        self.frame_focus_window.setMaximumSize(QSize(16777215, 16777215))
        self.frame_focus_window.setStyleSheet(u"background-color: rgb(41, 45, 56);\n"
"border-radius: 5px;\n"
"color: rgb(210, 210, 210);\n"
"")
        self.frame_focus_window.setFrameShape(QFrame.NoFrame)
        self.frame_focus_window.setFrameShadow(QFrame.Raised)
        self.verticalLayout_24 = QVBoxLayout(self.frame_focus_window)
        self.verticalLayout_24.setSpacing(0)
        self.verticalLayout_24.setObjectName(u"verticalLayout_24")
        self.verticalLayout_24.setContentsMargins(0, 0, 0, 0)
        self.frame_focus_window_title = QFrame(self.frame_focus_window)
        self.frame_focus_window_title.setObjectName(u"frame_focus_window_title")
        self.frame_focus_window_title.setMaximumSize(QSize(16777215, 35))
        self.frame_focus_window_title.setCursor(QCursor(Qt.ArrowCursor))
        self.frame_focus_window_title.setStyleSheet(u"QFrame {\n"
"	border: transparent;\n"
"	background-color: rgb(39, 44, 54);\n"
"}")
        self.frame_focus_window_title.setFrameShape(QFrame.StyledPanel)
        self.frame_focus_window_title.setFrameShadow(QFrame.Raised)
        self.horizontalLayout_23 = QHBoxLayout(self.frame_focus_window_title)
        self.horizontalLayout_23.setObjectName(u"horizontalLayout_23")
        self.horizontalLayout_23.setContentsMargins(9, 9, 9, 9)
        self.label_focus_window_title = QPushButton(self.frame_focus_window_title)
        self.label_focus_window_title.setObjectName(u"label_focus_window_title")
        sizePolicy4.setHeightForWidth(self.label_focus_window_title.sizePolicy().hasHeightForWidth())
        self.label_focus_window_title.setSizePolicy(sizePolicy4)
        self.label_focus_window_title.setMaximumSize(QSize(16777215, 16777215))
        self.label_focus_window_title.setFont(font3)
        self.label_focus_window_title.setStyleSheet(u"QPushButton{\n"
"	text-align: left\n"
"}")

        self.horizontalLayout_23.addWidget(self.label_focus_window_title)


        self.verticalLayout_24.addWidget(self.frame_focus_window_title)

        self.frame_focus_window_contents = QFrame(self.frame_focus_window)
        self.frame_focus_window_contents.setObjectName(u"frame_focus_window_contents")
        self.frame_focus_window_contents.setMinimumSize(QSize(0, 0))
        self.frame_focus_window_contents.setCursor(QCursor(Qt.ArrowCursor))
        self.frame_focus_window_contents.setStyleSheet(u"QFrame {\n"
"	 border: transparent;\n"
"}")
        self.frame_focus_window_contents.setFrameShadow(QFrame.Raised)
        self.horizontalLayout_24 = QHBoxLayout(self.frame_focus_window_contents)
        self.horizontalLayout_24.setSpacing(0)
        self.horizontalLayout_24.setObjectName(u"horizontalLayout_24")
        self.horizontalLayout_24.setContentsMargins(0, 0, 0, 0)
        self.frame_focus_window_plot_contents = QFrame(self.frame_focus_window_contents)
        self.frame_focus_window_plot_contents.setObjectName(u"frame_focus_window_plot_contents")
        self.frame_focus_window_plot_contents.setMinimumSize(QSize(0, 0))
        self.frame_focus_window_plot_contents.setMaximumSize(QSize(16777215, 16777215))
        self.frame_focus_window_plot_contents.setCursor(QCursor(Qt.ArrowCursor))
        self.frame_focus_window_plot_contents.setStyleSheet(u"")
        self.frame_focus_window_plot_contents.setFrameShadow(QFrame.Raised)
        self.horizontalLayout_37 = QHBoxLayout(self.frame_focus_window_plot_contents)
        self.horizontalLayout_37.setSpacing(6)
        self.horizontalLayout_37.setObjectName(u"horizontalLayout_37")
        self.horizontalLayout_37.setContentsMargins(0, 0, 0, 0)
        self.frame_focus_window_plot = QFrame(self.frame_focus_window_plot_contents)
        self.frame_focus_window_plot.setObjectName(u"frame_focus_window_plot")
        sizePolicy4.setHeightForWidth(self.frame_focus_window_plot.sizePolicy().hasHeightForWidth())
        self.frame_focus_window_plot.setSizePolicy(sizePolicy4)
        self.frame_focus_window_plot.setMinimumSize(QSize(0, 0))
        self.frame_focus_window_plot.setMaximumSize(QSize(16777215, 16777215))
        self.frame_focus_window_plot.setCursor(QCursor(Qt.ArrowCursor))
        self.frame_focus_window_plot.setStyleSheet(u"")
        self.frame_focus_window_plot.setFrameShadow(QFrame.Raised)
        self.verticalLayout_8 = QVBoxLayout(self.frame_focus_window_plot)
        self.verticalLayout_8.setObjectName(u"verticalLayout_8")

        self.horizontalLayout_37.addWidget(self.frame_focus_window_plot)

        self.frame_focus_window_size = QFrame(self.frame_focus_window_plot_contents)
        self.frame_focus_window_size.setObjectName(u"frame_focus_window_size")
        self.frame_focus_window_size.setMinimumSize(QSize(40, 0))
        self.frame_focus_window_size.setMaximumSize(QSize(40, 16777215))
        self.frame_focus_window_size.setCursor(QCursor(Qt.ArrowCursor))
        self.frame_focus_window_size.setStyleSheet(u"")
        self.frame_focus_window_size.setFrameShadow(QFrame.Raised)
        self.verticalLayout_12 = QVBoxLayout(self.frame_focus_window_size)
        self.verticalLayout_12.setSpacing(6)
        self.verticalLayout_12.setObjectName(u"verticalLayout_12")
        self.verticalLayout_12.setContentsMargins(9, 9, 9, 9)
        self.label_2 = QLabel(self.frame_focus_window_size)
        self.label_2.setObjectName(u"label_2")
        sizePolicy5.setHeightForWidth(self.label_2.sizePolicy().hasHeightForWidth())
        self.label_2.setSizePolicy(sizePolicy5)
        self.label_2.setPixmap(QPixmap(u":/icons/icons/outline_expand_white_18.png"))

        self.verticalLayout_12.addWidget(self.label_2)

        self.slider_focus_window_size = QSlider(self.frame_focus_window_size)
        self.slider_focus_window_size.setObjectName(u"slider_focus_window_size")
        sizePolicy6.setHeightForWidth(self.slider_focus_window_size.sizePolicy().hasHeightForWidth())
        self.slider_focus_window_size.setSizePolicy(sizePolicy6)
        self.slider_focus_window_size.setValue(50)
        self.slider_focus_window_size.setOrientation(Qt.Vertical)

        self.verticalLayout_12.addWidget(self.slider_focus_window_size)


        self.horizontalLayout_37.addWidget(self.frame_focus_window_size)


        self.horizontalLayout_24.addWidget(self.frame_focus_window_plot_contents)


        self.verticalLayout_24.addWidget(self.frame_focus_window_contents)


        self.verticalLayout_7.addWidget(self.frame_focus_window)

        self.scrollArea_plots.setWidget(self.widget_plots)
        self.splitter.addWidget(self.scrollArea_plots)

        self.verticalLayout_2.addWidget(self.splitter)


        self.verticalLayout_9.addWidget(self.frame_device_config)


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

        QMetaObject.connectSlotsByName(MainWindow)
    # setupUi

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QCoreApplication.translate("MainWindow", u"ST DTDL GUI", None))
#if QT_CONFIG(tooltip)
        self.menu_btn_info.setToolTip(QCoreApplication.translate("MainWindow", u"Show Application Log File", None))
#endif // QT_CONFIG(tooltip)
        self.menu_btn_info.setText("")
        self.settings_title_label.setText(QCoreApplication.translate("MainWindow", u"Settings", None))
        self.groupBox_acquisition_selection.setTitle(QCoreApplication.translate("MainWindow", u"Acquisitions Folder", None))
        self.acq_folder_textEdit.setPlaceholderText(QCoreApplication.translate("MainWindow", u"select a HSD acquisition folder", None))
        self.acq_folder_button.setText("")
        self.groupBox_components.setTitle(QCoreApplication.translate("MainWindow", u"Components", None))
        self.message_label.setText(QCoreApplication.translate("MainWindow", u"Please select one of the following sensors:", None))
        self.run_segmentation_button.setText(QCoreApplication.translate("MainWindow", u"Run Segmentation", None))
#if QT_CONFIG(tooltip)
        self.tag_acquisition_button.setToolTip(QCoreApplication.translate("MainWindow", u"All breakpoints must be confirmed", None))
#endif // QT_CONFIG(tooltip)
        self.tag_acquisition_button.setText(QCoreApplication.translate("MainWindow", u"Tag your acquisition", None))
        self.tag_acquisition_label.setText(QCoreApplication.translate("MainWindow", u"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><meta charset=\"utf-8\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"hr { height: 1px; border-width: 0; }\n"
"li.unchecked::marker { content: \"\\2610\"; }\n"
"li.checked::marker { content: \"\\2612\"; }\n"
"</style></head><body style=\" font-family:'Segoe UI'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:14pt; font-weight:700;\">Acquisition tagged<br />successfully!<br /><img src=\":/icons/icons/outline_done_outline_white_18dp.png\" /></span></p></body></html>", None))
        self.label_datastream_title.setText(QCoreApplication.translate("MainWindow", u"Datastream", None))
#if QT_CONFIG(tooltip)
        self.label_3.setToolTip(QCoreApplication.translate("MainWindow", u"Scroll Datastream", None))
#endif // QT_CONFIG(tooltip)
        self.label_3.setText("")
#if QT_CONFIG(tooltip)
        self.label.setToolTip(QCoreApplication.translate("MainWindow", u"Zoom Datastream", None))
#endif // QT_CONFIG(tooltip)
        self.label.setText("")
        self.label_score_title.setText(QCoreApplication.translate("MainWindow", u"Score", None))
        self.label_focus_window_title.setText(QCoreApplication.translate("MainWindow", u"Focus Window", None))
#if QT_CONFIG(tooltip)
        self.label_2.setToolTip(QCoreApplication.translate("MainWindow", u"Resize Focus Window", None))
#endif // QT_CONFIG(tooltip)
        self.label_2.setText("")
        self.label_credits.setText(QCoreApplication.translate("MainWindow", u"Assisted Segmentation GUI", None))
        self.label_version.setText(QCoreApplication.translate("MainWindow", u"v0.0.1", None))
    # retranslateUi

