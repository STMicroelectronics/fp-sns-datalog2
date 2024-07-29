
# ******************************************************************************
# * @attention
# *
# * Copyright (c) 2022 STMicroelectronics.
# * All rights reserved.
# *
# * This software is licensed under terms that can be found in the LICENSE file
# * in the root directory of this software component.
# * If no LICENSE file comes with this software, it is provided AS-IS.
# *
# *
# ******************************************************************************
#

from abc import abstractmethod
import os

from PySide6.QtCore import Qt, Slot, QTimer
from PySide6.QtGui import QPainter, QFont, QScreen
from PySide6.QtWidgets import QWidget, QFrame, QVBoxLayout, QPushButton, QSizePolicy
from PySide6.QtUiTools import QUiLoader
from PySide6.QtWidgets import QApplication
from PySide6.QtDesigner import QPyDesignerCustomWidgetCollection

import pyqtgraph as pg

import st_dtdl_gui

class PlotLabel(QWidget):
    def __init__(self, p_label, parent =None):
        super().__init__(parent)
        self.p_label = p_label
    
    def paintEvent(self, event):
        painter = QPainter(self)
        
        painter.setPen(Qt.white)
        painter.translate(painter.viewport().width() - 4, painter.viewport().height() - 4)
        painter.rotate(-90)
        bold = QFont()
        bold.setBold(True)
        painter.setFont(bold)
        painter.drawText(0, 0, self.p_label)
        painter.end()
        
class CustomPGPlotWidget(pg.PlotWidget):
    def __init__(self, parent=None, background='default', plotItem=None, **kargs):
        super().__init__(parent, background, plotItem, **kargs)
        self.parent = parent
        
    def wheelEvent(self, ev):
        if self.parent.is_docked:
            modifiers = QApplication.keyboardModifiers()
            if modifiers == Qt.ControlModifier:
                return super().wheelEvent(ev)
            else:
                return self.parent.wheelEvent(ev)
        else:
            return super().wheelEvent(ev)

class PlotWidget(QWidget):    
    def __init__(self, controller, comp_name, comp_display_name, p_id = 0, parent=None, left_label = None):
        super().__init__(parent)
        self.parent = parent
        self.controller = controller
        self.controller.sig_logging.connect(self.s_is_logging)
        self.controller.sig_detecting.connect(self.s_is_detecting)
        
        self.is_docked = True
        self.app_qt = self.controller.qt_app

        self.p_id = p_id
        self.comp_name = comp_name
        self.comp_display_name = comp_display_name
        self.left_label = left_label
        
        self.timer_interval = 0.2
        self.plot_len = 3000
        
        self.stop_stream = False
        
        QPyDesignerCustomWidgetCollection.registerCustomWidget(PlotWidget, module="PlotWidget")
        loader = QUiLoader()
        self.plot_widget = loader.load(os.path.join(os.path.dirname(st_dtdl_gui.__file__),"UI","plot_widget.ui"), parent)
        self.title_frame = self.plot_widget.frame_plot.findChild(QFrame,"frame_title")
        self.contents_frame = self.plot_widget.frame_plot.findChild(QFrame,"frame_contents")
        self.pushButton_pop_out = self.title_frame.findChild(QPushButton, "pushButton_pop_out")
        self.pushButton_pop_out.clicked.connect(self.clicked_pop_out_button)
        self.pushButton_plot_settings = self.title_frame.findChild(QPushButton, "pushButton_plot_settings")
        
        #Hide Output format description file loading frame
        self.load_output_fmt_frame = self.plot_widget.frame_plot.findChild(QFrame, "frame_load_out_fmt")
        self.is_out_fmt_displayed = False
        self.pushButton_plot_settings.setVisible(False)
        self.load_output_fmt_frame.setVisible(False)

        #Hide Time/Freq settings frame
        self.time_freq_setting_frame = self.plot_widget.frame_plot.findChild(QFrame, "frame_time_freq_settings")
        self.is_time_freq_settings_displayed = False
        self.pushButton_plot_settings.setVisible(False)
        self.time_freq_setting_frame.setVisible(False)
            
        #Hide FFT settings frame
        self.fft_settings_frame = self.plot_widget.frame_plot.findChild(QFrame, "frame_fft_settings")
        self.fft_settings_frame.setVisible(False)
        
        #Hide Wav conversion/playing frame
        self.frame_wav_control = self.plot_widget.frame_plot.findChild(QFrame, "frame_wav_control")
        self.is_wav_settings_displayed = False
        self.pushButton_plot_settings.setVisible(False)
        self.frame_wav_control.setVisible(False)
            
        #Main layout
        main_layout = QVBoxLayout()
        self.setLayout(main_layout)
        main_layout.addWidget(self.plot_widget)
        
        title_label = PlotLabel("{}".format(self.comp_display_name))
        title_label.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        self.title_frame.layout().addWidget(title_label)
        
        self.graph_widget = CustomPGPlotWidget(parent = self)
        self.graph_widget.getPlotItem().layout.setContentsMargins(10, 0, 3, 0)
        self.graph_widget.getPlotItem().setMenuEnabled(False) #Disable right click menu in plots
        # self.graph_widget.setMinimumSize(QSize(300, 150))
        self.graph_widget.setMinimumHeight(150)
        
        if self.left_label != None:
            styles = {'color':'#d2d2d2', 'font-size':'12px'}
            self.graph_widget.setLabel('left', self.left_label, **styles)
            self.graph_widget.getAxis("left").setWidth(60)
        
        self.graph_widget.setBackground('#1b1d23')
        # self.graph_widget.setAntialiasing(True)
        self.graph_widget.showGrid(x=True, y=True)
                
        #crosshair label in legend
        self.legend = self.graph_widget.addLegend()
        style = pg.PlotDataItem(pen='w')
        self.legend.anchor((0,0), (0,0))
        self.legend.addItem(style, 'coords')
        self.legend.items[0][0].deleteLater()
        
        #crosshair in signalgraph
        crosshair_pen=pg.mkPen(color='#484A4F', style=Qt.DashLine)
        vLine = pg.InfiniteLine(angle=90, movable=False, pen=crosshair_pen)
        hLine = pg.InfiniteLine(angle=0, movable=False, pen=crosshair_pen)
        self.graph_widget.addItem(vLine, ignoreBounds=True)
        self.graph_widget.addItem(hLine, ignoreBounds=True)
        
        # VB
        vb = self.graph_widget.plotItem.vb

        def mouseMoved(evt):
            pos = evt
            if self.graph_widget.sceneBoundingRect().contains(pos):
                mousePoint = vb.mapSceneToView(pos)
                if self.legend.getLabel(style) is not None:
                    self.legend.getLabel(style).setText("""<span style='font-size: 9pt; 
                                                            color: #20b2aa;
                                                            font-weight: bold'>
                                                            x=
                                                            <span style='color: white;
                                                            font-weight: normal'>
                                                            %0.1f,
                                                            <span style='color: #20b2aa;
                                                            font-weight: bold;'>
                                                            y=
                                                            <span style='color: white;
                                                            font-weight: normal;'>
                                                            %0.1f
                                                    </span>""" % (mousePoint.x(), mousePoint.y()))
                vLine.setPos(mousePoint.x())
                hLine.setPos(mousePoint.y())
        
        proxy = pg.SignalProxy(self.graph_widget.scene().sigMouseMoved, rateLimit=60, slot=mouseMoved)
        self.graph_widget.scene().sigMouseMoved.connect(mouseMoved)
        
        self.contents_frame.layout().addWidget(self.graph_widget)

        self.timer_interval_ms = self.timer_interval*1000
        self.timer = QTimer() #to create a thread that calls a function at intervals
        self.timer.setTimerType(Qt.PreciseTimer)
        self.timer.timeout.connect(self.update_plot)#the update function keeps getting called at intervals
    
    @Slot()
    def clicked_pop_out_button(self):
        if self.is_docked:
            self.pop_out_widget()
            self.is_docked = False
        else:
            self.pop_in_widget()
            self.is_docked = True
            
    @Slot(bool, int)
    @abstractmethod
    def s_is_logging(self, status: bool, interface: int):
        pass
        
    @Slot(bool, int)
    @abstractmethod
    def s_is_detecting(self, status: bool):
        pass

    def reset(self):
        pass
    
    @abstractmethod
    def update_plot(self):
        pass

    @abstractmethod
    def add_data(self, data):
        pass

    def closeEvent(self, event):
        self.pop_in_widget()
        self.is_docked = True

    def pop_out_widget(self):
        self.setWindowFlags(Qt.Dialog | Qt.WindowMaximizeButtonHint | Qt.WindowMinimizeButtonHint)
        center = QScreen.availableGeometry(QApplication.primaryScreen()).center()
        geo = self.frameGeometry()
        geo.moveCenter(center)
        self.move(geo.topLeft())
        self.show()

    def pop_in_widget(self):
        self.setWindowFlags(Qt.Widget)
        self.parent.layout().insertWidget(self.p_id, self)

# class PlotWidget_test_v1(PlotWidget): #NOTE Old. To be refactorized in order to support HSDv1 plots
#     def __init__(self, controller, comp_name, s_id, ss_id, odr, time_window, spts, sample_size, data_format, n_curves=1, parent=None, p_id = 0):
#         super().__init__(controller, comp_name, odr, time_window, n_curves, parent, p_id)
#         self.s_id = s_id
#         self.ss_id = ss_id
#         self.sample_size = sample_size
#         self.spts = spts
#         self.data_format = data_format