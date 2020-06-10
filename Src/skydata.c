/* skydata
 * Description: 
 *
 * Author: Bryce Himebaugh 
 * Contact: bhimebau@indiana.edu
 * Date: 01.17.2020
 * Copyright (C) 2020
 *
 */

#include "skydata.h"

//darkest value = 24.1
//brightest value = 19.4

//currently 47 entries


//float skydata[((DARKEST_VALUE_ALLOWED-BRIGHTEST_VALUE_ALLOWED)*10)+1] = {
                                  
float skydata[(((int) DARKEST_VALUE_ALLOWED- (int) BRIGHTEST_VALUE_ALLOWED)*10)+1] = {
  1805, // 19.4
  1715, // 19.5
  1640, // 19.6
  1600, // 19.7
  1550, // 19.8
  1520, // 19.9
  1500, // 20.0
  1460, // 20.1
  1425, // 20.2
  1390, // 20.3
  1345, // 20.4
  1300, // 20.5
  1255, // 20.6
  1195, // 20.7
  1150, // 20.8
  1105, // 20.9
  1060, // 21.0
  1025, // 21.1
  990,  // 21.2
  955,  // 21.3
  925,  // 21.4
  880,  // 21.5
  850,  // 21.6
  825,  // 21.7
  800,  // 21.8
  775,  // 21.9
  745,  // 22.0
  725,  // 22.1
  700,  // 22.2
  675,  // 22.3
  655,  // 22.4
  630,  // 22.5
  610,  // 22.6
  588,  // 22.7
  578,  // 22.8
  565,  // 22.9
  550,  // 23.0
  535,  // 23.1 *Does not appear to be very accurate below this point
  523,  // 23.2
  510,  // 23.3
  490,  // 23.4
  460,  // 23.5
  450,  // 23.6
  440,  // 23.7
  430,  // 23.8
  420,  // 23.9
  410,  // 24.0
  400,  // 24.1
};
  


