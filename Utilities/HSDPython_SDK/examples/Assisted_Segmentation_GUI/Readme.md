# GUI Application for Assisted Segmentation

This document provides a brief guide on how to run and use the GUI application for assisted segmentation, as implemented in `hsdatalog_data_segmentation_GUI.py`.
This is a prototype GUI to show the performance of the Assisted Segmentation algorithm. 


## Running the Application

1. **Prepare the Data**: The application expects a CSV file named `data/A5Y7FI_data.csv` with columns `AccX_LSB`, `AccY_LSB`, and `AccZ_LSB`. Ensure this file is present and correctly formatted. Please modify the code in `demo_gui.py` to load other types of files.

2. **Launch the Application**: Navigate to the directory containing `demo_gui.py` and run the following command in your terminal:

`python demo_gui.py`


## Using the Application

- **Data Visualization**: The main window displays the time series data, algorithmically suggested breakpoints, and any user-defined breakpoints.

- **Zoom and Scroll**: Use the zoom slider to adjust the visible range of the time series data. Scroll to navigate through the data. The Focus Window highlighted in yellow in the top plot can be dragged and dropped.

- **Viewing Breakpoints**: The application automatically displays candidate breakpoints and ground truth breakpoints on the plots. Candidate breakpoints are shown with dashed lines, while ground truth breakpoints are solid.

- **Adding/Removing Breakpoints**: Click on the plot to add a new ground truth breakpoint at the clicked location. Right-click on a candidate breakpoint to remove it. When a breakpoint is added or removed, the preceding candidate breakpoints are automatically confirmed.

- **Viewing Scores**: The score plot shows the segmentation scores across the data. Use this to identify potential breakpoints.

## Exiting the Application

To exit the application, simply close the main window or terminate the process in your terminal.

## Notes

- The application is a demonstration of interactive data segmentation and analysis. It is not optimized for large datasets or production use.
- For further customization or to use with different datasets, modifications to `demo_gui.py` and associated modules may be required.
