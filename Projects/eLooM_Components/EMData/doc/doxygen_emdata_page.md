\page emdata_main eLooM data format (EMData)

\tableofcontents

\section emdata_introduction Introduction
**EMData** is an eLooM-based application-level component that provides a common data definition (::EMData_t) and
other interfaces and classes useful for applications that manipulate data like AI or data logging. The following sections provide
a brief description of each class and interface exported by the component.

\subsection emdata_emdata_intro The EMData_t class
The ::EMData_t class has been inspired by the most popular data scientist toolkit. It provide an implementation of a multi-dimensional homogeneous data type.
An header encapsulate all the information about the the data, that are stored in a separate memory buffer called payload.

\anchor emdata_fig1 \image html 42_emd_header_payload.png "Fig.1 - example: header and payload"

\subsection emdata_event_intro Data event source and event listener
The component provides an implementation of the ::IEvent, ::IEventListener and ::IEventSrc interfaces specialized for the ::EMData_t:
- ::DataEvent_t
- ::IDataEventListener_t
- ::DataEventSrc_t

\subsection emdata_cb_intro Circular Buffer
A generic implementation of a circular buffer is provided by the ::CircularBuffer class. A circular buffer uses a block of memory
allocated by the application to stores a set of application defined item called ::CBItem. The application can get a free item from the head - CB_GetFreeItemFromHead() - to produce data and / or a ready item from the tail - CB_GetReadyItemFromTail() - to consume the data. A circular buffer
can be used by multiple tasks.

\subsection emdata_source_bservable_intro Source observable
::ISourceObservable is an interface that describe a generic source of data stream.