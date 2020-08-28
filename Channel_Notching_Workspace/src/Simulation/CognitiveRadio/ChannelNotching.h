/************************************************************************************
 * Copyright (C) 2015                                                               *
 * TETCOS, Bangalore. India                                                         *
 *                                                                                  *
 * Tetcos owns the intellectual property rights in the Product and its content.     *
 * The copying, redistribution, reselling or publication of any or all of the       *
 * Product or its content without express prior written consent of Tetcos is        *
 * prohibited. Ownership and / or any other right relating to the software and all *
 * intellectual property rights therein shall remain at all times with Tetcos.      *
 *                                                                                  *
 * Author:    Shubham Gupta                                                         *
 *                                                                                  *
 * ---------------------------------------------------------------------------------*/
#ifndef _CHANNEL_NOTCHING_H_
#define _CHANNEL_NOTCHING_H_

 //subchannel structure
typedef struct stru_subchannel
{
	unsigned int nChannelNumber;
	char subchannelbitmap[60];
}SUBCHANNEL, *PSUBCHANNEL;

//functions needed for channel Notching.
int init_subchannel(int count, int number);
void init_scansubchannel(BS_MAC* pstruBSMAC, BS_PHY* pstruBSPhy);
int scansubchannel(PSUBCHANNEL subchannel, double dLowerChannelFrequency,
	double dUpperChannelFrequency,
	double dIncumbentStartFrequency,
	double dIncumbentEndFrequency);
int updatesymbolparameter(struct stru_802_22_Channel* pstruChannel, SYMBOL_PARAMETER* pstruSymbolParameter);
PSUBCHANNEL getsubchannel(int number);
double getLowerFrequency(unsigned int number, struct stru_802_22_Channel* pstruChannelList);
double getUpperFrequency(unsigned int number, struct stru_802_22_Channel* pstruChannelList);

#endif
