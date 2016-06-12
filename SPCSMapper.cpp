#ifndef _SPCSMAPPER_CPP_
#define _SPCSMAPPER_CPP_
// $Id: SPCSMapper.cpp,v 1.1.1.1 2002-02-21 00:19:00 bmaddox Exp $
// Brian Maddox - USGS MCMC SES - bmaddox@usgs.gov
// Last modified by $Author: bmaddox $ on $Date: 2002-02-21 00:19:00 $

/***************************************************************************
 *                                                                         *
 *   This library is free software and is distributed under the MIT open   *
 *   source license.  For more information, consult the file COPYING.      *
 *                                                                         *
 ***************************************************************************/

#include "SPCSMapper.h"

using namespace USGSImageLib;

// **************************************************************************
SPCSMapper::SPCSMapper()
  : initialized(false)
{
}


// **************************************************************************
SPCSMapper::~SPCSMapper()
{
}


// **************************************************************************
// This is probably a real bad way of doing this, and yeah, it's kinda cheesy,
// but, well, it works pretty darn well
void SPCSMapper::initNAD27Mappings(void)
  throw()
{
  // Make sure the array is zero'd out before we begin
  for (int loopcount = 0; loopcount < 6000; loopcount++)
    mappings[loopcount] = 0;

  // Alabama 
  mappings[101] = 26729;  mappings[102] = 26730;
  // Alaska
  mappings[5001] = 26731; mappings[5002] = 26732; mappings[5003] = 26733;
  mappings[5004] = 26734; mappings[5005] = 26735; mappings[5006] = 26736;
  mappings[5007] = 26737; mappings[5008] = 26738; mappings[5009] = 26739;
  mappings[5010] = 26740;
  // Arizona
  mappings[201] = 26748; mappings[202] = 26749; mappings[203] = 26750;
  // Arkansas
  mappings[301] = 26751; mappings[302] = 26752;
  // California
  mappings[401] = 26741; mappings[402] = 26742; mappings[403] = 26743;
  mappings[404] = 26744; mappings[405] = 26745; mappings[406] = 26746;
  mappings[407] = 26747;
  // Colorado
  mappings[501] = 26753; mappings[502] = 26754; mappings[503] = 26755;
  // Connecticut
  mappings[600] = 26756;
  // Delaware
  mappings[700] = 26757;
  // Dist of Columbia 
  //  mappings[1900] = 26785;
  // Florida
  mappings[901] = 26758; mappings[902] = 26759; mappings[903] = 26760;
  // Georgia
  mappings[1001] = 26766; mappings[1002] = 26767;
  // Hawaii
  mappings[5101] = 26761; mappings[5102] = 26762; mappings[5103] = 26763;
  mappings[5104] = 26764; mappings[5105] = 26765;
  // Idaho
  mappings[1101] = 26768; mappings[1102] = 26769; mappings[1103] = 26770;
  // Illinois
  mappings[1201] = 26771; mappings[1202] = 26772;
  // Indiana
  mappings[1301] = 26773; mappings[1302] = 26774;
  // Iowa
  mappings[1401] = 26775; mappings[1402] = 26776;
  // Kansas
  mappings[1501] = 26777; mappings[1502] = 26778;
  // Kentucky
  mappings[1601] = 26779; mappings[1602] = 26780;
  // Louisiana
  mappings[1701] = 26781; mappings[1702] = 26782; //mappings[1703] =
  // Maine
  mappings[1801] = 26783; mappings[1802] = 26784;
  // Maryland
  mappings[1900] = 26785;
  // Massachusetts
  mappings[2001] = 26786; mappings[2002] = 26787;
  // Michigan
  mappings[2101] = 26801; mappings[2102] = 26802; mappings[2103] = 26803;
  mappings[2111] = 26788; mappings[2112] = 26789; mappings[2113] = 26790;
  // Minnesota
  mappings[2201] = 26791; mappings[2202] = 26792; mappings[2203] = 26793;
  // Mississippi
  mappings[2301] = 26794; mappings[2302] = 26795;
  // Missouri
  mappings[2401] = 26796; mappings[2402] = 26797; mappings[2403] = 26798;
  // Montana
  mappings[2501] = 32001; mappings[2502] = 32002; mappings[2503] = 32003;
  // Nebraska
  mappings[2601] = 32005; mappings[2602] = 32006;
  // Nevada
  mappings[2701] = 32007; mappings[2702] = 32008; mappings[2703] = 32009;
  // New Hampshire
  mappings[2800] = 32010;
  // New Jersey
  mappings[2900] = 32011;
  // New Mexico
  mappings[3001] = 32012; mappings[3002] = 32013; mappings[3003] = 32014;
  // New York
  mappings[3101] = 32015; mappings[3102] = 32016; mappings[3103] = 32017;
  mappings[3104] = 32018;
  // North Carolina
  mappings[3200] = 32019;
  // North Dakota
  mappings[3301] = 32020; mappings[3302] = 32021;
  // Ohio
  mappings[3401] = 32022; mappings[3402] = 32023;
  // Oklahoma
  mappings[3501] = 32024; mappings[3502] = 32025;
  // Oregon
  mappings[3601] = 32026; mappings[3602] = 32027;
  // Pennsylvania
  mappings[3701] = 32028; mappings[3702] = 32029;
  // Rhode Island
  mappings[3800] = 32030;
  // South Carolina
  mappings[3901] = 32031; mappings[3902] = 32033;
  // South Dakota
  mappings[4001] = 32034; mappings[4002] = 32035;
  // Tennessee
  mappings[4100] = 32036;
  // Texas
  mappings[4201] = 32037; mappings[4202] = 32038; mappings[4203] = 32039;
  mappings[4204] = 32040; mappings[4205] = 32041;
  // Utah
  mappings[4301] = 32042; mappings[4302] = 32043; mappings[4303] = 32044;
  // Vermont
  mappings[4400] = 32045;
  // Virginia
  mappings[4501] = 32046; mappings[4502] = 32047;
  // Washington
  mappings[4601] = 32048; mappings[4602] = 32049;
  // West Virginia
  mappings[4701] = 32050; mappings[4702] = 32051;
  // Wisconsin
  mappings[4801] = 32052; mappings[4802] = 32053; mappings[4803] = 32054;
  // Wyoming
  mappings[4901] = 32055; mappings[4902] = 32056; mappings[4903] = 32057;
  mappings[4904] = 32058;
  // Puerto Rico
  mappings[5201] = 32059;
  // St Croix
  mappings[5202] = 32060;

  initialized = true;
}

// **************************************************************************
void SPCSMapper::initNAD83Mappings(void)
  throw()
{
  // Make sure the array is zero'd out before we begin
  for (int loopcount = 0; loopcount < 6000; loopcount++)
    mappings[loopcount] = 0;

  // Alabama 
  mappings[101] = 26929;  mappings[102] = 26930;
  // Alaska
  mappings[5001] = 26931; mappings[5002] = 26932; mappings[5003] = 26933;
  mappings[5004] = 26934; mappings[5005] = 26935; mappings[5006] = 26936;
  mappings[5007] = 26937; mappings[5008] = 26938; mappings[5009] = 26939;
  mappings[5010] = 26940;
  // Arizona
  mappings[201] = 26948; mappings[202] = 26949; mappings[203] = 26950;
  // Arkansas
  mappings[301] = 26951; mappings[302] = 26952;
  // California
  mappings[401] = 26941; mappings[402] = 26942; mappings[403] = 26943;
  mappings[404] = 26944; mappings[405] = 26945; mappings[406] = 26946;
  // Colorado
  mappings[501] = 26953; mappings[502] = 26954; mappings[503] = 26955;
  // Connecticut
  mappings[600] = 26956;
  // Delaware
  mappings[700] = 26957;
  // Dist of Columbia 
  //  mappings[1900] = 26785;
  // Florida
  mappings[901] = 26958; mappings[902] = 26959; mappings[903] = 26960;
  // Georgia
  mappings[1001] = 26966; mappings[1002] = 26967;
  // Hawaii
  mappings[5101] = 26961; mappings[5102] = 26962; mappings[5103] = 26963;
  mappings[5104] = 26964; mappings[5105] = 26965;
  // Idaho
  mappings[1101] = 26968; mappings[1102] = 26969; mappings[1103] = 26970;
  // Illinois
  mappings[1201] = 26971; mappings[1202] = 26972;
  // Indiana
  mappings[1301] = 26973; mappings[1302] = 26974;
  // Iowa
  mappings[1401] = 26975; mappings[1402] = 26976;
  // Kansas
  mappings[1501] = 26977; mappings[1502] = 26978;
  // Kentucky
  mappings[1601] = 26979; mappings[1602] = 26980;
  // Louisiana
  mappings[1701] = 26981; mappings[1702] = 26982; //mappings[1703] =
  // Maine
  mappings[1801] = 26983; mappings[1802] = 26984;
  // Maryland
  mappings[1900] = 26985;
  // Massachusetts
  mappings[2001] = 26986; mappings[2002] = 26987;
  // Michigan
  mappings[2111] = 26988; mappings[2112] = 26989; mappings[2113] = 26990;
  // Minnesota
  mappings[2201] = 26991; mappings[2202] = 26992; mappings[2203] = 26993;
  // Mississippi
  mappings[2301] = 26994; mappings[2302] = 26995;
  // Missouri
  mappings[2401] = 26996; mappings[2402] = 26997; mappings[2403] = 26998;
  // Montana
  mappings[2500] = 32100;
  // Nebraska
  mappings[2600] = 32104;
  // Nevada
  mappings[2701] = 32107; mappings[2702] = 32108; mappings[2703] = 32109;
  // New Hampshire
  mappings[2800] = 32110;
  // New Jersey
  mappings[2900] = 32111;
  // New Mexico
  mappings[3001] = 32112; mappings[3002] = 32113; mappings[3003] = 32114;
  // New York
  mappings[3101] = 32115; mappings[3102] = 32116; mappings[3103] = 32117;
  mappings[3104] = 32118;
  // North Carolina
  mappings[3200] = 32119;
  // North Dakota
  mappings[3301] = 32120; mappings[3302] = 32121;
  // Ohio
  mappings[3401] = 32122; mappings[3402] = 32123;
  // Oklahoma
  mappings[3501] = 32124; mappings[3502] = 32125;
  // Oregon
  mappings[3601] = 32126; mappings[3602] = 32127;
  // Pennsylvania
  mappings[3701] = 32128; mappings[3702] = 32129;
  // Rhode Island
  mappings[3800] = 32130;
  // South Carolina
  mappings[3900] = 32133;
  // South Dakota
  mappings[4001] = 32134; mappings[4002] = 32135;
  // Tennessee
  mappings[4100] = 32136;
  // Texas
  mappings[4201] = 32137; mappings[4202] = 32138; mappings[4203] = 32139;
  mappings[4204] = 32140; mappings[4205] = 32141;
  // Utah
  mappings[4301] = 32142; mappings[4302] = 32143; mappings[4303] = 32144;
  // Vermont
  mappings[4400] = 32145;
  // Virginia
  mappings[4501] = 32146; mappings[4502] = 32147;
  // Washington
  mappings[4601] = 32148; mappings[4602] = 32149;
  // West Virginia
  mappings[4701] = 32150; mappings[4702] = 32151;
  // Wisconsin
  mappings[4801] = 32152; mappings[4802] = 32153; mappings[4803] = 32154;
  // Wyoming
  mappings[4901] = 32155; mappings[4902] = 32156; mappings[4903] = 32157;
  mappings[4904] = 32158;
  // Puerto Rico
  mappings[5200] = 32161;
  // St Croix
  //mappings[5202] = 32161;

  initialized = true;
}


// **************************************************************************
bool SPCSMapper::getUSGSCode(int ingeotiffcode, unsigned short& usgscode)
  throw()
{
  if (!initialized)
    return false;

  for (int loopcount = 0; loopcount < 6000; loopcount++)
  {
    if (mappings[loopcount] == ingeotiffcode)
    {
      usgscode = loopcount;
      break;
    }
  }

  return true;
}

// **************************************************************************
bool SPCSMapper::getGeoTIFFCode(int inspcscode, unsigned short& geotiffcode)
  throw()
{
  if (!initialized)
    return false;

  geotiffcode = mappings[inspcscode];

  return true;
}


#endif
