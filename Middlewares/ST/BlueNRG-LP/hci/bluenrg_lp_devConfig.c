
#include "bluenrg_utils.h"

/************** Do not change this define section ************/

#define CONFIG_HW_LS_RO           0xFF
#define CONFIG_HW_LS_XTAL         0x00

#define CONFIG_HW_SMPS_10uH              0xFF
#define CONFIG_HW_SMPS_2_2uH             0x00
#define CONFIG_HW_SMPS_NONE              0x01
#define CONFIG_HW_SMPS_1_5uH             0x02

#define USER_MODE               0xFF
#define LS_CRYSTAL_MEASURE      0x01
#define HS_STARTUP_TIME_MEASURE 0x02
#define TX_RX_MODE              0x03

#define MASTER_SCA_500ppm       0 /*!< 251 ppm to 500 ppm */
#define MASTER_SCA_250ppm       1 /*!< 151 ppm to 250 ppm */
#define MASTER_SCA_150ppm       2 /*!< 101 ppm to 150 ppm */
#define MASTER_SCA_100ppm       3 /*!< 76 ppm to 100 ppm */
#define MASTER_SCA_75ppm        4 /*!< 51 ppm to 75 ppm */
#define MASTER_SCA_50ppm        5 /*!< 31 ppm to 50 ppm */
#define MASTER_SCA_30ppm        6 /*!< 21 ppm to 30 ppm */
#define MASTER_SCA_20ppm        7 /*!< 0 ppm to 20 ppm */

/************************************************************/


/************** Definitions that can be changed. ************/

#define CONFIG_HW_HSE_TUNE      63

#define LS_SOURCE               CONFIG_HW_LS_RO
#define SMPS_INDUCTOR           CONFIG_HW_SMPS_10uH

#define SLAVE_SCA_PPM           500     /* Max 500 */
#define HS_STARTUP_TIME_US      800
#define MAX_CONN_EVT_LEN        4000

#define DAY                     02
#define MONTH                   05
#define YEAR                    85
/************************************************************/

#ifndef CONFIG_HW_HSE_TUNE
#define CONFIG_HW_HSE_TUNE   63
#endif

#ifndef LS_SOURCE
#define LS_SOURCE       CONFIG_HW_LS_RO
#endif

#ifndef SMPS_INDUCTOR
#define SMPS_INDUCTOR   CONFIG_HW_SMPS_10uH
#endif

#ifndef TEST_MODE
#define TEST_MODE       USER_MODE
#endif



const devConfig_t deviceConfig = {
  CONFIG_HW_HSE_TUNE,
  LS_SOURCE,
  SMPS_INDUCTOR,
  0xFF,
  htob(HS_STARTUP_TIME_US, 2),
  htob(SLAVE_SCA_PPM, 0),
  0xFF,
  htob(FROM_US_TO_SYS_TIME((MAX_CONN_EVT_LEN*1000)), 4),
  TEST_MODE
};

