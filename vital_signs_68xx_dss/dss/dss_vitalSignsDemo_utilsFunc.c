/**
 *   @file  post_processing.c
 *
 *   @brief
 *      Routines for post processing and Miscellaneous Functions for the Vital Signs Demo
 *
 *  \par

 *  NOTE:
 * Copyright (c) 2018 Texas Instruments Incorporated
 *
 * All rights reserved not granted herein.
 * Limited License.
 *
 * Texas Instruments Incorporated grants a world-wide, royalty-free, non-exclusive license
 * under copyrights and patents it now or hereafter owns or controls to make, have made, use,
 * import, offer to sell and sell ("Utilize") this software subject to the terms herein.
 *
 * With respect to the foregoing patent license, such license is granted  solely to the extent
 * that any such patent is necessary to Utilize the software alone.  The patent license shall
 * not apply to any combinations which include this software, other than combinations with
 * devices manufactured by or for TI (“TI Devices”). No hardware patent is licensed hereunder.
 *
 * Redistributions must preserve existing copyright notices and reproduce this license
 * (including the above copyright notice and the disclaimer and (if applicable) source code
 * license limitations below) in the documentation and/or other materials provided with the
 * distribution.
 *
 * Redistribution and use in binary form, without modification, are permitted provided that
 * the following conditions are met:
 *
 * No reverse engineering, decompilation, or disassembly of this software is permitted with
 * respect to any software provided in binary form. Any redistribution and use are licensed
 * by TI for use only with TI Devices. Nothing shall obligate TI to provide you with source
 * code for the software licensed and provided to you in object code.
 *
 * If software source code is provided to you, modification and redistribution of the source
 * code are permitted provided that the following conditions are met:
 *
 * Any redistribution and use of the source code, including any resulting derivative works,
 * are licensed by TI for use only with TI Devices.
 * Any redistribution and use of any object code compiled from the source code and any
 * resulting derivative works, are licensed by TI for use only with TI Devices.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of its suppliers may be
 * used to endorse or promote products derived from this software without specific prior
 * written permission.
 *
 * DISCLAIMER.
 *
 * THIS SOFTWARE IS PROVIDED BY TI AND TI’S LICENSORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL TI AND TI’S
 * LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* Standard Include Files. */
#include "dss_vitalSignsDemo_utilsFunc.h"

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <math.h>


/* BIOS/XDC Include Files. */
#include <xdc/std.h>
#include <xdc/cfg/global.h>

/** @fn  float filter_IIR_BiquadCascade()
*   @brief Filters the data 
*
*/
#define PI 3.1415926535897


float  filter_IIR_BiquadCascade(float DataIn, float *pFilterCoefs, float *pScaleVals, float *pDelay, uint16_t numStages)
{
	float a1,a2;
	float b0,b1,b2;
	float y, input, output;
	float scaleVal;
	uint16_t indexStage;
	uint16_t numCoefsStage;
	uint16_t indexTemp;
	
	numCoefsStage = 6;
	input = DataIn;
	for (indexStage = 0; indexStage < numStages; indexStage++)
	{
		indexTemp = numCoefsStage*indexStage;
		b0 = pFilterCoefs[ indexTemp ];
		b1 = pFilterCoefs[ indexTemp + 1];
		b2 = pFilterCoefs[ indexTemp + 2];
		a1 = pFilterCoefs[ indexTemp + 4];
		a2 = pFilterCoefs[ indexTemp + 5];
		scaleVal = pScaleVals[indexStage];

		pDelay[indexTemp] = scaleVal*input - a1*pDelay[indexTemp + 1] - a2*pDelay[indexTemp + 2];//el +1 hya el out prev wel +2 el prev prev
        y =  b0*pDelay[indexTemp] + b1*pDelay[indexTemp + 1] + b2*pDelay[indexTemp + 2];

        pDelay[indexTemp + 2] = pDelay[indexTemp + 1];
        pDelay[indexTemp + 1] = pDelay[indexTemp];

	    input = y;
    }
	output = y;

	return output;
}


float unwrap(float phase, float phasePrev, float *diffPhaseCorrectionCum,bool rangeBinChanges)
{
	float modFactorF;
	float diffPhase;
	float diffPhaseMod;
	float diffPhaseCorrection;
	float phaseOut;

	// incremental phase variation
	diffPhase = phase - phasePrev;

	if (diffPhase > PI)
		modFactorF = 1;
	else if (diffPhase < - PI)
	    modFactorF = -1;
	else
		modFactorF = 0;

	diffPhaseMod = diffPhase - modFactorF*2*PI;

	// preserve variation sign for +pi vs. -pi
	if ((diffPhaseMod == -PI) && (diffPhase > 0))//zeyda bel logic
		diffPhaseMod = PI;

	// incremental phase correction
	diffPhaseCorrection = diffPhaseMod - diffPhase;

	// Ignore correction when incremental variation is smaller than cutoff
	if (((diffPhaseCorrection < PI) && (diffPhaseCorrection>0)) ||
			((diffPhaseCorrection > -PI) && (diffPhaseCorrection<0)) )
		diffPhaseCorrection = 0;

	if (rangeBinChanges)
	    diffPhaseCorrection=phasePrev-phase;
	// Find cumulative sum of deltas
	*diffPhaseCorrectionCum = *diffPhaseCorrectionCum + diffPhaseCorrection;
	phaseOut = phase + *diffPhaseCorrectionCum;
    return phaseOut;
 }



uint16_t  computeMaxIndex(float *pDataIn, uint16_t startIndex, uint16_t endIndex)
{
	float MaxVal = pDataIn[startIndex];
	uint16_t 	MaxValIndex = startIndex;
	uint16_t temp;
		for (temp = startIndex+1; temp < endIndex; temp++)
		{
			if (pDataIn[temp] > MaxVal)
			{
				MaxVal = pDataIn[temp];
				MaxValIndex = temp;
			}
		}
		return MaxValIndex;
}


static uint16_t  find_Peaks_uint32(uint32_t *pDataIn, uint16_t *pPeakIndex, float *pPeakValues, uint16_t startIndex, uint16_t endIndex)
{
        uint16_t temp;
        uint16_t numPeaks;
        numPeaks = 0;
        for (temp = startIndex+1; temp < endIndex-1; temp++)
        {
            if (pDataIn[temp] > pDataIn[temp-1]  & pDataIn[temp] > pDataIn[temp+1] )
            {
                pPeakIndex[numPeaks]  = temp;
                pPeakValues[numPeaks] = pDataIn[temp];
                numPeaks = numPeaks+1;
            }
        }
        return numPeaks;
}

static uint16_t find_Peaks_int32(int32_t *pDataIn, uint16_t *pPeakLocs, float *pPeakValues, uint16_t startIndex, uint16_t endIndex)
{
        uint16_t temp;
        uint16_t numPeaks;
        numPeaks = 0;
        bool t=0;
        for (temp = startIndex+1; temp < endIndex-1; temp++)
        {
            if(t){
                if(pDataIn[temp] > pDataIn[temp-1]  & pDataIn[temp] > pDataIn[temp+1] & pDataIn[temp] > 0.05)// second we validated it with its next maxima > 0.1
                {
                    pPeakLocs[numPeaks] = temp;
                    pPeakValues[numPeaks] = (float) pDataIn[temp];
                    numPeaks = numPeaks+1;
                    t=0;
                }
            }
            if (pDataIn[temp] < pDataIn[temp-1] & pDataIn[temp] < pDataIn[temp+1] & pDataIn[temp]< -0.1) // first we find a minima < -0.1
            {
               t=1;
            }
        }
		return numPeaks;
}


static uint16_t find_Peaks_float(float *pDataIn, uint16_t *pPeakLocs,float *pPeakValues, uint16_t startIndex, uint16_t endIndex)
{
        uint16_t temp;
        uint16_t numPeaks;
        numPeaks = 0;
        for (temp = startIndex+1; temp < endIndex-1; temp++)
        {
        if (pDataIn[temp] > pDataIn[temp-1]  & pDataIn[temp] > pDataIn[temp+1] )
                {
                    pPeakLocs[numPeaks] = temp;
                    pPeakValues[numPeaks] = (float) pDataIn[temp];
                    numPeaks = numPeaks+1;
                }
        }
        return numPeaks;
}

uint16_t find_Peaks(void *pDataIn, enum dataType type, uint16_t *pPeakIndex, float *pPeakValues, uint16_t startIndex, uint16_t endIndex)
{

    int32_t  *pDataIn_int32;
    uint32_t *pDataIn_uint32;
    float    *pDataIn_float;

    uint16_t numPeaks;
     if (type == int32_type)
     {
         pDataIn_int32 = (int32_t*) pDataIn;
         numPeaks = find_Peaks_int32(pDataIn_int32, pPeakIndex, pPeakValues, startIndex, endIndex);
     }

     if (type == uint32_type)
     {
         pDataIn_uint32 = (uint32_t*) pDataIn;
         numPeaks = find_Peaks_uint32(pDataIn_uint32, pPeakIndex, pPeakValues, startIndex, endIndex);
     }

     if (type == float_type)
     {
         pDataIn_float = (float*) pDataIn;
         numPeaks = find_Peaks_float(pDataIn_float, pPeakIndex, pPeakValues, startIndex, endIndex);
     }

     return numPeaks;
}

uint16_t shape_form_recognition(float *pDataIn, uint16_t *pPeakLocs,float *pPeakValues, uint16_t startIndex, uint16_t endIndex, uint16_t winMin, uint16_t winMax)//heart pk method
{
        uint16_t temp,temp_saved,temp_start;
        uint16_t numPeaks;
        numPeaks = 0;
        float min1,min2;
        bool t_minima1=0,t_maxima1=0,t_minima2=0,t_maxima2=0;
        for (temp = startIndex+1; temp < endIndex-1; temp++)
        {
            if(pDataIn[temp] > pDataIn[temp-1]  & pDataIn[temp] > pDataIn[temp+1])//maxima
            {
                if(t_minima1){
                    if(pDataIn[temp] > 0.05){
                        t_maxima1 = 1;
                        t_minima1 = 0;
                    }
                }
                if(t_minima2){
                    t_maxima2 = 1;
                    t_minima2 = 0;
                }
            }
            else if (pDataIn[temp] < pDataIn[temp-1] & pDataIn[temp] < pDataIn[temp+1])//minima
            {
                if(t_maxima2){
                    if(pDataIn[temp] < min2 & pDataIn[temp] < -0.05 & abs(min1-pDataIn[temp]) < 0.15 & temp-temp_start < winMax & temp-temp_start > winMin){
                        t_maxima2 = 0;
                        pPeakLocs[numPeaks] = temp_saved;//temp;
                        pPeakValues[numPeaks] = (float) pDataIn[temp_saved];//temp];
                        numPeaks = numPeaks+1;
                        temp -= 1;
                    }
                    else{
                        t_maxima2 = 0;
                        temp = temp_saved-1;
                    }
                }
                else{
                    if(!t_maxima1){
                        if(pDataIn[temp] < -0.05){
                            t_minima1 = 1;
                            min1 = pDataIn[temp];
                            temp_start = temp;
                        }
                        else{
                            t_minima1 = 0;
                        }
                    }
                    else{
                        temp_saved = temp;
                        if(pDataIn[temp] > min1){
                            t_minima2 = 1;
                            t_maxima1 = 0;
                            min2 = pDataIn[temp];
                        }
                        else{
                            t_maxima1 = 0;
                            temp -= 1;
                        }
                    }
                }
            }
        }
        return numPeaks;
}

uint16_t  filterPeaksWfm(uint16_t *pPeakLocsIn, float *pPeakValues, uint16_t *pPeakLocsOut, uint16_t numPeaksIn, uint16_t winMin, uint16_t winMax)
{
	// Filter out invalid peaks outside [winMin winMax]
    uint16_t tempIndex;
	uint16_t pkDiff;
	uint16_t numPeaksOutValid;
   	numPeaksOutValid = 1;   // The first or the second peak is assumed to be valid

   	if(pPeakValues[0] > pPeakValues[1]/4){
   	    pPeakLocsOut[0] = pPeakLocsIn[0];
   	    tempIndex = 1;
   	}else{
        pPeakLocsOut[0] = pPeakLocsIn[1];
        tempIndex = 2;
    }
	for (; tempIndex < numPeaksIn; tempIndex++)
	{
		pkDiff = pPeakLocsIn[tempIndex] - pPeakLocsOut[numPeaksOutValid - 1 ];
		if ( pkDiff > winMin )
		{
		    pPeakLocsOut[numPeaksOutValid] = pPeakLocsIn[tempIndex];
		    numPeaksOutValid = numPeaksOutValid + 1;
		}
	}
	return numPeaksOutValid;
}

uint16_t  find_periodicity(uint16_t *pPeakLocsIn, uint16_t numPeaksIn, uint16_t winMin, uint16_t winMax)
{
    uint16_t tempIndex;
    uint16_t last_symbol_ind, period;
    uint16_t decalage = 2,max,recorded_max=winMax-8,n_period,max_n_period=0;
    for(max = winMax;max > 10; --max){
        n_period=0;
        last_symbol_ind = pPeakLocsIn[0];
        for (tempIndex = 1; tempIndex < numPeaksIn; tempIndex++)
        {
            period = pPeakLocsIn[tempIndex] - last_symbol_ind;
            last_symbol_ind = pPeakLocsIn[tempIndex];
            if(max >= period & period > max-decalage)
                ++n_period;
            else if(max >= period/2 & period/2 > max-decalage)
                n_period+=2;
        }
        if(n_period>max_n_period){
            max_n_period = n_period;
            recorded_max = max;
        }
    }
    return recorded_max;
}


float  filter_FIR(float *pDataIn, float *filterCoefs, uint16_t numCoefs)
{
    float sum;
    uint16_t temp;
    sum = 0;
	for (temp = 0; temp < numCoefs; temp++)
	{
		sum += pDataIn[temp]*filterCoefs[temp];
	}
	return sum;
}

float  computeConfidenceMetric(float* pDataSpectrum,
                               uint16_t spectrumIndexStart,
                               uint16_t spectrumIndexEnd,
                               uint16_t peakIndex,
                               uint16_t numIndexAroundPeak)
{
    uint16_t indexTemp;
    int16_t startInd;
    int16_t endInd;

    float sumSignal;
    float sumPeak;
    float confidenceMetric;

    endInd   = peakIndex + numIndexAroundPeak;
    startInd = peakIndex - numIndexAroundPeak;

    if (startInd < 0)
        startInd = 0;
    if(endInd >= (spectrumIndexEnd))
        endInd = spectrumIndexEnd - 1;

    /* Energy of the complete Spectrum */
    sumSignal = 0;
    for (indexTemp = spectrumIndexStart; indexTemp <= spectrumIndexEnd; indexTemp++)
    {
        sumSignal += pDataSpectrum[indexTemp];
    }

    /* Energy of the frequency Bins including (and around) the peak of interest */
    sumPeak   = 0;
    for (indexTemp = startInd; indexTemp <= endInd; indexTemp++)
    {
        sumPeak += pDataSpectrum[indexTemp];
    }

    if (fabs(sumSignal - sumPeak) < 0.0001)     // This condition would arise if the input signal amplitude is very low
    {
        confidenceMetric = 0;
    }
    else
    {
        confidenceMetric = sumPeak/(sumSignal - sumPeak);
    }

    return confidenceMetric;
}




float  filter_RemoveImpulseNoise(float dataPrev2, float dataPrev1 , float dataCurr, float thresh)
{
	float forwardDiff,backwardDiff;
	float x1, x2, y1, y2, x, y;
    float pDataIn[3];

    pDataIn[0] = dataPrev2 ;
	pDataIn[1] = dataPrev1 ;
	pDataIn[2] = dataCurr  ;

	backwardDiff = pDataIn[1] - pDataIn[0];
	forwardDiff  = pDataIn[1] - pDataIn[2];

	x1 = 0;
	x2 = 2;
	y1 = pDataIn[0];
	y2 = pDataIn[2];
	x  = 1;

	/* Interpolate between x1 and x2 if the forwardDiff and backwardDiff exceed the threshold */
	if ( ((forwardDiff > thresh) && (backwardDiff > thresh)) || ((forwardDiff < -thresh) && (backwardDiff < -thresh)))
	y = y1 + ( ((x-x1)*(y2 - y1))/(x2 - x1) );
	else
	y = pDataIn[1];

	return y;
}


void  heapsort_index(float *pDataIn, int dataLength,  uint16_t *pSortOutIndex)
{
    int l,j,ir,indxt,i;
    float q;

    for (j = 0; j < dataLength; j++)
        pSortOutIndex[j] = j;

    if (dataLength == 1)
        return;

    l  = dataLength >> 1;
    ir = dataLength - 1;

    while(1)
    {
        if (l > 0)
        {
            q = pDataIn[(indxt = pSortOutIndex[--l])];
        }
        else
        {
            q  = pDataIn[(indxt = pSortOutIndex[ir])];
            pSortOutIndex[ir] = pSortOutIndex[0];
            if (--ir == 0)
            {
                pSortOutIndex[0] = indxt;
                return;
            }
        }

        i = l;
        j = (l << 1) + 1;

        while (j <= ir)
        {

            if (j < ir && pDataIn[pSortOutIndex[j]] < pDataIn[pSortOutIndex[j+1]]) j++;

            if (q < pDataIn[pSortOutIndex[j]])
            {
                pSortOutIndex[i] = pSortOutIndex[j];
                j += (i = j) + 1;
            }
            else
                break;
        }

        pSortOutIndex[i] = indxt;
    }
}

uint16_t binary_search(float *pDataIn, uint16_t datalength, float target)
{
    uint8_t first,last,middle;
    first = 0;
    last = datalength - 1;
    middle = (first+last)/2;
    while (first <= last) {
        if (pDataIn[middle] < target)
            first = middle + 1;
        else if (pDataIn[middle] > target)
            last = middle - 1;
        else
        {
            first = middle;
            break;
        }
        middle = (first + last)/2;
    }
    return first;
}


float  computeAGC (float* pDataIn, uint16_t dataInLength, uint16_t lenBlock, float thresh)
{
    uint16_t indexTemp;
    uint16_t indexCurr;
    uint16_t indexInner;

    float sumEnergy;
    float scaleValue;
    float scaleValueSum;   // Higher this value , more the movement
    scaleValueSum = 0;

        for (indexTemp = lenBlock; indexTemp < dataInLength; indexTemp++)
        {
            sumEnergy = 0;
            for (indexInner = 0; indexInner <= lenBlock; indexInner++)
            {
                indexCurr = indexTemp - lenBlock + indexInner ;
                sumEnergy += pDataIn[indexCurr] * pDataIn[indexCurr];
            }

            if (sumEnergy > thresh)
                 {
                 scaleValue = sqrt(thresh/sumEnergy);
                 scaleValueSum += 1;
                     for (indexInner = 0; indexInner <= lenBlock; indexInner++)
                     {
                     indexCurr = indexTemp - lenBlock + indexInner ;
                     pDataIn[indexCurr]= pDataIn[indexCurr] * scaleValue;
                     }
                 }

        }
        return scaleValueSum;
}


void  computeAutoCorrelation (float* pDataIn, uint16_t dataInLength, float* pDataOut,  uint16_t minLag, uint16_t maxLag)
{
    float sum;
    uint16_t indexLag;
    uint16_t index;

    for (indexLag = minLag; indexLag < maxLag; indexLag++)
    {
        sum = 0;
        for (index = 0; index < dataInLength - indexLag; index++)
        {
            sum += pDataIn[index]* pDataIn[index + indexLag];
        }
        pDataOut[indexLag] = sum;
    }

}


void computeEnergyHarmonics (float* pAbsSpectrum, float* pDataOut, uint16_t spectrumStartIndex, uint16_t spectrumEndIndex, uint16_t freqWindowSize)
{
    float sum;

    uint16_t index, indexInnerLoop;

    uint16_t  window1StartIndex;
    uint16_t  window2StartIndex;

    for (index = spectrumStartIndex; index < spectrumEndIndex; index++)
    {
        window1StartIndex = index   - freqWindowSize;
        window2StartIndex = 2*index - freqWindowSize;

        sum = 0;
        for (indexInnerLoop = 0; indexInnerLoop < freqWindowSize; indexInnerLoop++)
        {
            sum += pAbsSpectrum[window1StartIndex + indexInnerLoop] + pAbsSpectrum[window2StartIndex + indexInnerLoop];
        }
        pDataOut[index] = sum;
    }


}




