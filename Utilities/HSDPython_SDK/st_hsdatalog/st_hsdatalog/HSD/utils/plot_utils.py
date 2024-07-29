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

from collections import OrderedDict
import matplotlib.pyplot as plt
import numpy as np

class PlotUtils:
    
    # A list of colors to be used for line plotting, for example in a graph.
    lines_colors = ['#e6007e', '#a4c238', '#3cb4e6', '#ef4f4f', '#46b28e', '#e8ce0e', '#60b562', '#f99e20', '#41b3ba']

    @staticmethod
    def draw_line(plt, ss_data_frame, idx, color, label, picker = False):
        if picker:
            line, = plt.plot(ss_data_frame['Time'], ss_data_frame.iloc[:, idx + 1], color=color, label=label, picker=5)
        else:
            line, = plt.plot(ss_data_frame['Time'], ss_data_frame.iloc[:, idx + 1], color=color, label=label)
        return line

    @staticmethod
    def draw_tag_lines(plt, ss_data_frame, label, alpha=0.9):
        true_tag_idxs = ss_data_frame[label].loc[lambda x: x== True].index
        tag_groups = np.split(true_tag_idxs, np.where(np.diff(true_tag_idxs) != 1)[0]+1)
        for i in range(len(tag_groups)):
            start_tag_time = ss_data_frame.at[tag_groups[i][0],'Time']
            end_tag_time = ss_data_frame.at[tag_groups[i][-1],'Time']
            plt.axvspan(start_tag_time, end_tag_time, facecolor='1', alpha=alpha)
            plt.axvline(x=start_tag_time, color='g', label= "Start " + label)
            plt.axvline(x=end_tag_time, color='r', label= "End " + label)
    
    @staticmethod
    def set_plot_time_label(axs, fig, dim):
        if dim > 1:
            for ax in axs.flat:
                ax.set(xlabel = 'Time (s)')
            for ax in fig.get_axes():
                ax.label_outer()
        else:
            axs[0].set(xlabel = 'Time (s)')
    
    @staticmethod
    def set_legend(ax):
        old_handles, old_labels = ax.get_legend_handles_labels()
        ax.legend = plt.legend(handles=old_handles, labels=list(OrderedDict.fromkeys(old_labels)), loc='upper left', ncol=1)

    @staticmethod
    def draw_regions(plt, ss_data_frame, label, color, edgecolor, alpha, hatch):
        true_flag_idxs = ss_data_frame[label].loc[lambda x: x== 1.0].index
        if len(true_flag_idxs)>0:
            flag_groups = np.split(true_flag_idxs, np.where(np.diff(true_flag_idxs) != 1)[0]+1)
            for i in range(len(flag_groups)):
                start_flag_time = ss_data_frame.at[flag_groups[i][0],'Time']
                end_flag_time = ss_data_frame.at[flag_groups[i][-1],'Time']
                plt.axvspan(start_flag_time, end_flag_time, facecolor=color, edgecolor=edgecolor, alpha=alpha, hatch=hatch)

    @staticmethod
    def darken_color(color_hex, percent):
        r = int(color_hex[1:3], 16)
        g = int(color_hex[3:5], 16)
        b = int(color_hex[5:7], 16)

        r_dark = int(r * (100 - percent) / 100)
        g_dark = int(g * (100 - percent) / 100)
        b_dark = int(b * (100 - percent) / 100)

        color_dark = "#{:02x}{:02x}{:02x}".format(r_dark, g_dark, b_dark)

        return color_dark