/*********************************************************************************
//                      Venus Game Engine
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Venus Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//      Copyright(c) 2003-2014 Venus.PK, All Right Reversed.
//	VProfiler.h
//	Created: 2014-2-13   10:44
//
**********************************************************************************/
#ifndef __VPROFILER_H__
#define __VPROFILER_H__

#define VENABLE_STAT 0


#if VENABLE_STAT
	
template<class T, UINT HistorySize = 64> 
class VStatHistory
{
	T History[HistorySize];
	UINT HistoryCount;
	float Total;
	float Max;
public:
	VStatHistory(void) :HistoryCount(0),Total(0.0f),Max(0.0f)
	{
		for (UINT Index = 0; Index < HistorySize; Index++)
		{
			History[Index] = (T)0;
		}
	}

	inline float GetAverage(void) const
	{
		return Total / (float)HistorySize;
	}
	inline float GetMax(void) const
	{
		return Max;
	}

	/**
	 * Returns the most recent value
	 */
	inline TYPE GetMostRecent(void) const
	{
		DWORD Index = (HistoryCount - 1) % STAT_HISTORY_SIZE;
		return History[ Index ];
	}

	/**
	 * Adds a new value to the history
	 */
	inline void AddToHistory(const T& Value)
	{
		// Find where we are going to update
		UINT Index = (HistoryCount++) % STAT_HISTORY_SIZE;
		// Reinit max if we've wrapped around our circular buffer
		if (Index == 0)
		{
			Max = InitialStatValue<TYPE>();
		}
		// Subtract the previous value (note that this will be zero while the
		// count hasn't wrapped)
		Total = Total - (DOUBLE)History[Index] + (DOUBLE)Value;
		// Now that the previous value no longer contributes set the new one
		History[Index] = Value;
		// Update the max
		if ((DOUBLE)Value > Max)
		{
			Max = Value;
		}
	}
};
#endif 


#define VPROF(x)  //x

class VProfiler
{
public:
	VProfiler(void);
	~VProfiler(void);
};


#endif 

