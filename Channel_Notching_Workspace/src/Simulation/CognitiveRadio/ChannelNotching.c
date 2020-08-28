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
#include "main.h"
#include "802_22.h"

#ifdef _CHANNEL_NOTCHING_


 // The code will work only for a single channel, single CPE and for atmost one incumbent. 

PSUBCHANNEL* subchannellist = NULL;

char com[60];// This represents the subchannel bitmap if all subchannel are blocked

// Create subchannels for all channels
int init_subchannel(int count, int number)
{
	if (subchannellist == NULL)
	{
		subchannellist = (PSUBCHANNEL*)calloc(count, sizeof * subchannellist);
		memset(com, '1', 60);
	}
	if (subchannellist[number - 1] == NULL)
	{
		subchannellist[number - 1] = (PSUBCHANNEL)calloc(1, sizeof * subchannellist[number - 1]);
		memset(subchannellist[number - 1]->subchannelbitmap, '0', 60);
	}
	subchannellist[number - 1]->nChannelNumber = number;
	return 0;
}

//Initially scan all channels for incumbents and block busy subchannels.
void init_scansubchannel(BS_MAC* pstruBSMAC, BS_PHY* pstruBSPhy)
{
	struct stru_802_22_Channel* pstruChannel, * pstruPrevChannel = NULL;
	INCUMBENT* pstruIncumbent;
	CHANNEL_STATE nPrevChannelState;
	int nLoop;
	for (nLoop = 0; nLoop < pstruBSMAC->nIncumbentCount; nLoop++)
	{
		pstruIncumbent = pstruBSMAC->pstruIncumbent[nLoop];
		if (pstruIncumbent->nIncumbentStatus == IncumbentStatus_NONOPERATIONAL)
			continue;
		pstruChannel = pstruBSPhy->pstruChannelSet;
		nPrevChannelState = ChannelState_BACKUP;
		while (pstruChannel)
		{
			if (pstruChannel->nChannelState == ChannelState_DISALLOWED)
			{
				pstruChannel = pstruChannel->pstruNextChannel;
				nPrevChannelState = ChannelState_PROTECTED;
				memset(subchannellist[pstruChannel->nChannelNumber - 1]->subchannelbitmap, '1', sizeof subchannellist[pstruChannel->nChannelNumber - 1]->subchannelbitmap);
				continue;
			}
			if (scansubchannel(subchannellist[pstruChannel->nChannelNumber - 1],
				pstruChannel->dLowerFrequency,
				pstruChannel->dUpperFrequency,
				(double)pstruIncumbent->nStartFrequeny,
				(double)pstruIncumbent->nEndFrequency))
			{
				pstruChannel->nChannelState = ChannelState_PROTECTED;
				if (pstruPrevChannel)
					pstruPrevChannel->nChannelState = ChannelState_PROTECTED;
				nPrevChannelState = ChannelState_PROTECTED;
			}
			else
			{
				pstruChannel->nChannelState = ChannelState_BACKUP;
				if (nPrevChannelState == ChannelState_PROTECTED)
					pstruChannel->nChannelState = ChannelState_PROTECTED;
				nPrevChannelState = ChannelState_BACKUP;
			}
			pstruChannel = pstruChannel->pstruNextChannel;
		}
	}
}

//scan the subchannels of the channel with given channel number
int scansubchannel(PSUBCHANNEL subchannel, double dLowerChannelFrequency,
	double dUpperChannelFrequency,
	double dIncumbentStartFrequency,
	double dIncumbentEndFrequency)
{
	double diff = (dUpperChannelFrequency - dLowerChannelFrequency) / 60;
	int i;
	memset(subchannel->subchannelbitmap, '0', 60);
	for (i = 0; i < 60; i++)
	{
		if (fnIsinRange(dLowerChannelFrequency + i * diff,
			dLowerChannelFrequency + (i + 1) * diff,
			dIncumbentStartFrequency,
			dIncumbentEndFrequency))
		{
			subchannel->subchannelbitmap[i] = '1';
			if (i != 59)
				subchannel->subchannelbitmap[i + 1] = '1';
			if (i != 0)
				subchannel->subchannelbitmap[i - 1] = '1';
		}
	}
	if (strcmp(com, subchannel->subchannelbitmap))
		return 0;// indicates some sub channel are free
	return 1;// indicates all sub channels are blocked
}

//Recalculate the bits per symbol, which will effect the maximum possible throughput
int updatesymbolparameter(struct stru_802_22_Channel* pstruChannel, SYMBOL_PARAMETER* pstruSymbolParameter)
{
	int ncount = 0, i;
	for (i = 0; i < 60; i++)
		if (subchannellist[pstruChannel->nChannelNumber - 1]->subchannelbitmap[i] == '0')
			ncount++;
	pstruSymbolParameter->nOFDMSlots = ncount;
	pstruSymbolParameter->nBitsCountInOneSymbol = pstruSymbolParameter->nOFDMSlots * pstruSymbolParameter->nBitsCountInOneSlot;
	return 1;
}

//return subchannel structure of channel with given index.
PSUBCHANNEL getsubchannel(int number)
{
	return subchannellist[number - 1];
}

//return lower frequency of channel with given index.
double getLowerFrequency(unsigned int number, struct stru_802_22_Channel* pstruChannelList)
{
	while (pstruChannelList)
	{
		if (pstruChannelList->nChannelNumber==number)
			return pstruChannelList->dLowerFrequency;
		pstruChannelList = pstruChannelList->pstruNextChannel;
	}
	return 0;
}

//return upper frequency of channel with given index.
double getUpperFrequency(unsigned int number, struct stru_802_22_Channel* pstruChannelList)
{
	while (pstruChannelList)
	{
		if (pstruChannelList->nChannelNumber == number)
			return pstruChannelList->dUpperFrequency;
		pstruChannelList = pstruChannelList->pstruNextChannel;
	}
	return 0;
}

#endif