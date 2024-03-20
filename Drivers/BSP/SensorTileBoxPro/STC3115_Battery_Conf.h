/**
  ******************************************************************************
  * @file    STC3115_Battery_Conf.h
  * @author  System Research & Applications Team - Agrate/Catania Lab.
  * @version V1.1.0
  * @date    20-July-2023
  * @brief   Application/Battery description and charge curve characterization
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
*/

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __STC3115_BATTERY_CONF_H
#define __STC3115_BATTERY_CONF_H


/* ******************************************************************************** */
/*   INTERNAL PARAMETERS FOR BAK 103450AR2-1S-3M  BATTERY                           */
/* -------------------------------------------------------------------------------- */

/*Battery parameters define  ------------------------------------------------------ */
#define CAPACITY 480 /* battery nominal capacity in mAh */
#define RINT     160   /* Internal battery impedance in mOhms,0 if unknown */

//#define OCV_OFFSET_TAB	{-39, -44, -37, -46, -43, -43, -42, -28, -9, -1, -6, -55, -76, -97, -85, 0 } /* OLD OCVTAB	*/
#define OCV_OFFSET_TAB  {0, -40, -100, -58, -31, -30, -9, -19, -36, -40, -4, -36, -40, -23, -41, -20}
//#define OCV_OFFSET_TAB  {-20, -41, -23, -40, -36, -4, -40, -36, -19, -9, -30, -31, -58, -100, -40, 0}
 
/*Application parameters define  -------------------------------------------------- */
#define VMODE     MIXED_MODE /* running mode constant, VM_MODE or MIXED_MODE */
#define ALM_EN    0          /* Alarm enable constant, set at 1 to enable	*/
#define ALM_SOC   10         /* SOC alarm in % */
#define ALM_VBAT  2850       /* Voltage alarm in mV	*/
#define RSENSE    50         /* sense resistor in mOhms */

#define APP_EOC_CURRENT    14   /* end charge current in mA     */
#define APP_CUTOFF_VOLTAGE 3000 /* application cut-off voltage in mV  */

/* ******************************************************************************** */

#endif /* __STC3115_BATTERY_CONF_H */
