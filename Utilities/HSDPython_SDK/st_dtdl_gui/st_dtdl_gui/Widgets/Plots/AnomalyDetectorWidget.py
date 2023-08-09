 
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


from PySide6.QtWidgets import QLabel

from st_dtdl_gui.Widgets.Plots.ClassifierOutputWidget import ClassifierOutputWidget

class AnomalyDetectorWidget(ClassifierOutputWidget):
    def __init__(self, controller, comp_name, comp_display_name, anomaly_classes, ai_tool = None, with_signal=False, with_confidence=False, p_id=0, parent=None, left_label=None):
        """AI is creating summary for __init__
        Args:
            controller ([type]): [description]
            comp_name ([type]): [description]
            comp_display_name ([type]): [description]
            out_classes (dict): a dict (class name, image_path)
            p_id (int, optional): [description]. Defaults to 0.
            parent ([type], optional): [description]. Defaults to None.
        """
        super().__init__(controller, comp_name, comp_display_name, anomaly_classes, ai_tool, with_signal, with_confidence, p_id, parent, left_label)
        self.timer_interval = 0.2

        self.ai_tool_category_label.setText("Anomaly Detection")
        
    def update_plot(self):
        if len(self._data[0]) > 0: 
            # Extract all data from the queue (pop)    
            one_reduced_t_interval = [self._data[0].popleft() for _i in range(len(self._data[0]))]
            ort = one_reduced_t_interval[0][0]
            if self.with_confidence:
                confidence = one_reduced_t_interval[0][1]
                self.class_confidence_value.setText(str(round((confidence * 100.0), 2)) + " %")
            class_id = int(ort)
            class_names = list(self.output_class_widget.keys())
            class_name = class_names[class_id]
            for cn in class_names:
                if cn == class_name:
                    self.output_class_widget[cn].out_class_image.setPixmap(self.output_class_pixmaps[cn][0])
                    self.output_class_widget[cn].setEnabled(True)
                    self.output_class_widget[cn].findChild(QLabel,"out_class_name").setStyleSheet("color: #a4c238; font-size: 30px;")
                else:
                    self.output_class_widget[cn].out_class_image.setPixmap(self.output_class_pixmaps[cn][1])
                    self.output_class_widget[cn].setEnabled(False)
                    self.output_class_widget[cn].findChild(QLabel,"out_class_name").setStyleSheet("color: #383D48; font-size: 20px;")
        self.app_qt.processEvents()
