// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright (c) 2008-2023 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

#include "foundation/PxPreprocessor.h"

#include "foundation/PxAllocator.h"
#include "foundation/PxAtomic.h"
#include "foundation/PxIntrinsics.h"
#include "DySolverBody.h"
#include "DySolverConstraint1D.h"
#include "DySolverContact.h"
#include "DyThresholdTable.h"
#include "DySolverControl.h"
#include "DyArticulationPImpl.h"
#include "foundation/PxThread.h"
#include "DySolverConstraintDesc.h"
#include "DySolverContext.h"
#include "DyFeatherstoneArticulation.h"

namespace physx
{
namespace Dy
{
void solve1DBlock					(DY_PGS_SOLVE_METHOD_PARAMS);
void solveContactBlock				(DY_PGS_SOLVE_METHOD_PARAMS);
void solveExtContactBlock			(DY_PGS_SOLVE_METHOD_PARAMS);
void solveExt1DBlock				(DY_PGS_SOLVE_METHOD_PARAMS);
void solveContact_BStaticBlock		(DY_PGS_SOLVE_METHOD_PARAMS);
void solveContactPreBlock			(DY_PGS_SOLVE_METHOD_PARAMS);
void solveContactPreBlock_Static	(DY_PGS_SOLVE_METHOD_PARAMS);
void solve1D4_Block					(DY_PGS_SOLVE_METHOD_PARAMS);

void solve1DConcludeBlock				(DY_PGS_SOLVE_METHOD_PARAMS);
void solveContactConcludeBlock			(DY_PGS_SOLVE_METHOD_PARAMS);
void solveExtContactConcludeBlock		(DY_PGS_SOLVE_METHOD_PARAMS);
void solveExt1DConcludeBlock			(DY_PGS_SOLVE_METHOD_PARAMS);
void solveContact_BStaticConcludeBlock	(DY_PGS_SOLVE_METHOD_PARAMS);
void solveContactPreBlock_Conclude		(DY_PGS_SOLVE_METHOD_PARAMS);
void solveContactPreBlock_ConcludeStatic(DY_PGS_SOLVE_METHOD_PARAMS);
void solve1D4Block_Conclude				(DY_PGS_SOLVE_METHOD_PARAMS);

void solve1DBlockWriteBack				(DY_PGS_SOLVE_METHOD_PARAMS);
void solveContactBlockWriteBack			(DY_PGS_SOLVE_METHOD_PARAMS);
void solveExtContactBlockWriteBack		(DY_PGS_SOLVE_METHOD_PARAMS);
void solveExt1DBlockWriteBack			(DY_PGS_SOLVE_METHOD_PARAMS);
void solveContact_BStaticBlockWriteBack	(DY_PGS_SOLVE_METHOD_PARAMS);
void solveContactPreBlock_WriteBack		(DY_PGS_SOLVE_METHOD_PARAMS);
void solveContactPreBlock_WriteBackStatic(DY_PGS_SOLVE_METHOD_PARAMS);
void solve1D4Block_WriteBack			(DY_PGS_SOLVE_METHOD_PARAMS);

// PT: not sure what happened, these ones were declared but not actually used
//void writeBack1DBlock				(DY_PGS_SOLVE_METHOD_PARAMS);
//void contactBlockWriteBack		(DY_PGS_SOLVE_METHOD_PARAMS);
//void extContactBlockWriteBack		(DY_PGS_SOLVE_METHOD_PARAMS);
//void ext1DBlockWriteBack			(DY_PGS_SOLVE_METHOD_PARAMS);
//void contactPreBlock_WriteBack	(DY_PGS_SOLVE_METHOD_PARAMS);
//void writeBack1D4Block			(DY_PGS_SOLVE_METHOD_PARAMS);

// could move this to PxPreprocessor.h but 
// no implementation available for MSVC
#if PX_GCC_FAMILY
#define PX_UNUSED_ATTRIBUTE __attribute__((unused))
#else
#define PX_UNUSED_ATTRIBUTE 
#endif

static SolveBlockMethod gVTableSolveBlock[] PX_UNUSED_ATTRIBUTE = 
{
	0,
	solveContactBlock,				// DY_SC_TYPE_RB_CONTACT
	solve1DBlock,					// DY_SC_TYPE_RB_1D
	solveExtContactBlock,			// DY_SC_TYPE_EXT_CONTACT
	solveExt1DBlock,				// DY_SC_TYPE_EXT_1D
	solveContact_BStaticBlock,		// DY_SC_TYPE_STATIC_CONTACT
	solveContactBlock,				// DY_SC_TYPE_NOFRICTION_RB_CONTACT
	solveContactPreBlock,			// DY_SC_TYPE_BLOCK_RB_CONTACT
	solveContactPreBlock_Static,	// DY_SC_TYPE_BLOCK_STATIC_RB_CONTACT
	solve1D4_Block,					// DY_SC_TYPE_BLOCK_1D,
};

static SolveWriteBackBlockMethod gVTableSolveWriteBackBlock[] PX_UNUSED_ATTRIBUTE = 
{
	0,
	solveContactBlockWriteBack,				// DY_SC_TYPE_RB_CONTACT
	solve1DBlockWriteBack,					// DY_SC_TYPE_RB_1D
	solveExtContactBlockWriteBack,			// DY_SC_TYPE_EXT_CONTACT
	solveExt1DBlockWriteBack,				// DY_SC_TYPE_EXT_1D
	solveContact_BStaticBlockWriteBack,		// DY_SC_TYPE_STATIC_CONTACT
	solveContactBlockWriteBack,				// DY_SC_TYPE_NOFRICTION_RB_CONTACT
	solveContactPreBlock_WriteBack,			// DY_SC_TYPE_BLOCK_RB_CONTACT
	solveContactPreBlock_WriteBackStatic,	// DY_SC_TYPE_BLOCK_STATIC_RB_CONTACT
	solve1D4Block_WriteBack,				// DY_SC_TYPE_BLOCK_1D,
};

static SolveBlockMethod gVTableSolveConcludeBlock[] PX_UNUSED_ATTRIBUTE = 
{
	0,
	solveContactConcludeBlock,				// DY_SC_TYPE_RB_CONTACT
	solve1DConcludeBlock,					// DY_SC_TYPE_RB_1D
	solveExtContactConcludeBlock,			// DY_SC_TYPE_EXT_CONTACT
	solveExt1DConcludeBlock,				// DY_SC_TYPE_EXT_1D
	solveContact_BStaticConcludeBlock,		// DY_SC_TYPE_STATIC_CONTACT
	solveContactConcludeBlock,				// DY_SC_TYPE_NOFRICTION_RB_CONTACT
	solveContactPreBlock_Conclude,			// DY_SC_TYPE_BLOCK_RB_CONTACT
	solveContactPreBlock_ConcludeStatic,	// DY_SC_TYPE_BLOCK_STATIC_RB_CONTACT
	solve1D4Block_Conclude,					// DY_SC_TYPE_BLOCK_1D,
};

SolveBlockMethod* getSolveBlockTable()
{
	return gVTableSolveBlock;
}

SolveBlockMethod* getSolverConcludeBlockTable()
{
	return gVTableSolveConcludeBlock;
}

SolveWriteBackBlockMethod* getSolveWritebackBlockTable()
{
	return gVTableSolveWriteBackBlock;
}

SolverCoreGeneral* SolverCoreGeneral::create(bool fricEveryIteration)
{
	SolverCoreGeneral* scg = reinterpret_cast<SolverCoreGeneral*>(
		PX_ALLOC(sizeof(SolverCoreGeneral), "SolverCoreGeneral"));

	if (scg)
	{
		PX_PLACEMENT_NEW(scg, SolverCoreGeneral);
		scg->frictionEveryIteration = fricEveryIteration;
	}

	return scg;
}

void SolverCoreGeneral::destroyV()
{
	this->~SolverCoreGeneral();
	PX_FREE_THIS;
}

void SolverCoreGeneral::solveV_Blocks(SolverIslandParams& params) const
{
	const PxI32 TempThresholdStreamSize = 32;
	ThresholdStreamElement tempThresholdStream[TempThresholdStreamSize];

	SolverContext cache;
	cache.solverBodyArray			= params.bodyDataList;
	cache.mThresholdStream			= tempThresholdStream;
	cache.mThresholdStreamLength	= TempThresholdStreamSize;
	cache.mThresholdStreamIndex		= 0;
	cache.writeBackIteration		= false;
	cache.Z							= params.Z;
	cache.deltaV					= params.deltaV;

	const PxI32 batchCount = PxI32(params.numConstraintHeaders);

	PxSolverBody* PX_RESTRICT bodyListStart = params.bodyListStart;
	const PxU32 bodyListSize = params.bodyListSize;

	Cm::SpatialVector* PX_RESTRICT motionVelocityArray = params.motionVelocityArray;

	const PxU32 velocityIterations = params.velocityIterations;
	const PxU32 positionIterations = params.positionIterations;

	const PxU32 numConstraintHeaders = params.numConstraintHeaders;
	const PxU32 articulationListSize = params.articulationListSize;

	ArticulationSolverDesc* PX_RESTRICT articulationListStart = params.articulationListStart;

	PX_ASSERT(positionIterations >= 1);

	if(numConstraintHeaders == 0)
	{
		for (PxU32 baIdx = 0; baIdx < bodyListSize; baIdx++)
		{
			Cm::SpatialVector& motionVel = motionVelocityArray[baIdx];
			const PxSolverBody& atom = bodyListStart[baIdx];

			motionVel.linear = atom.linearVelocity;
			motionVel.angular = atom.angularState;
		}

		//Even thought there are no external constraints, there may still be internal constraints in the articulations...
		for(PxU32 i = 0; i < positionIterations; ++i)
			for (PxU32 j = 0; j < articulationListSize; ++j)
				articulationListStart[j].articulation->solveInternalConstraints(params.dt, params.invDt, cache.Z, cache.deltaV, false, false, 0.f, 0.8f);

		for (PxU32 i = 0; i < articulationListSize; i++)
			ArticulationPImpl::saveVelocity(articulationListStart[i].articulation, cache.deltaV);

		for (PxU32 i = 0; i < velocityIterations; ++i)
			for (PxU32 j = 0; j < articulationListSize; ++j)
				articulationListStart[j].articulation->solveInternalConstraints(params.dt, params.invDt, cache.Z, cache.deltaV, true, false, 0.f, 0.8f);

		for (PxU32 j = 0; j < articulationListSize; ++j)
			articulationListStart[j].articulation->writebackInternalConstraints(false);

		return;
	}

	BatchIterator contactIterator(params.constraintBatchHeaders, params.numConstraintHeaders);

	PxSolverConstraintDesc* PX_RESTRICT constraintList = params.constraintList;

	//0-(n-1) iterations
	PxI32 normalIter = 0;

	for (PxU32 iteration = positionIterations; iteration > 0; iteration--)	//decreasing positive numbers == position iters
	{
		cache.doFriction = this->frictionEveryIteration ? true : iteration <= 3;

		SolveBlockParallel(constraintList, batchCount, normalIter * batchCount, batchCount, 
			cache, contactIterator, iteration == 1 ? gVTableSolveConcludeBlock : gVTableSolveBlock, normalIter);

		for (PxU32 i = 0; i < articulationListSize; ++i)
			articulationListStart[i].articulation->solveInternalConstraints(params.dt, params.invDt, cache.Z, cache.deltaV, false, false, 0.f, 0.8f);

		++normalIter;
	}

	for (PxU32 baIdx = 0; baIdx < bodyListSize; baIdx++)
	{
		const PxSolverBody& atom = bodyListStart[baIdx];
		Cm::SpatialVector& motionVel = motionVelocityArray[baIdx];
		motionVel.linear = atom.linearVelocity;
		motionVel.angular = atom.angularState;
	}
	
	for (PxU32 i = 0; i < articulationListSize; i++)
		ArticulationPImpl::saveVelocity(articulationListStart[i].articulation, cache.deltaV);

	const PxI32 velItersMinOne = (PxI32(velocityIterations)) - 1;

	PxI32 iteration = 0;

	for(; iteration < velItersMinOne; ++iteration)
	{
		SolveBlockParallel(constraintList, batchCount, normalIter * batchCount, batchCount, 
			cache, contactIterator, gVTableSolveBlock, normalIter);

		for (PxU32 i = 0; i < articulationListSize; ++i)
			articulationListStart[i].articulation->solveInternalConstraints(params.dt, params.invDt, cache.Z, cache.deltaV, true, false, 0.f, 0.8f);
		++normalIter;
	}

	PxI32* outThresholdPairs = params.outThresholdPairs;
	ThresholdStreamElement* PX_RESTRICT thresholdStream = params.thresholdStream;
	PxU32 thresholdStreamLength = params.thresholdStreamLength;

	cache.writeBackIteration = true;
	cache.mSharedThresholdStream = thresholdStream;
	cache.mSharedThresholdStreamLength = thresholdStreamLength;
	cache.mSharedOutThresholdPairs = outThresholdPairs;
	//PGS solver always runs at least one velocity iteration (otherwise writeback won't happen)
	{
		SolveBlockParallel(constraintList, batchCount, normalIter * batchCount, batchCount, 
			cache, contactIterator, gVTableSolveWriteBackBlock, normalIter);

		for (PxU32 i = 0; i < articulationListSize; ++i)
		{
			articulationListStart[i].articulation->solveInternalConstraints(params.dt, params.invDt, cache.Z, cache.deltaV, true, false, 0.f, 0.8f);
			articulationListStart[i].articulation->writebackInternalConstraints(false);
		}

		++normalIter;
	}

	//Write back remaining threshold streams
	if(cache.mThresholdStreamIndex > 0)
	{
		//Write back to global buffer
		PxI32 threshIndex = PxAtomicAdd(outThresholdPairs, PxI32(cache.mThresholdStreamIndex)) - PxI32(cache.mThresholdStreamIndex);
		for(PxU32 b = 0; b < cache.mThresholdStreamIndex; ++b)
		{
			thresholdStream[b + threshIndex] = cache.mThresholdStream[b];
		}
		cache.mThresholdStreamIndex = 0;
	}
}

void SolverCoreGeneral::solveVParallelAndWriteBack
(SolverIslandParams& params, Cm::SpatialVectorF* Z, Cm::SpatialVectorF* deltaV) const
{
#if PX_PROFILE_SOLVE_STALLS
	PxU64 startTime = readTimer();

	PxU64 stallCount = 0;
#endif

	SolverContext cache;
	cache.solverBodyArray = params.bodyDataList;
	const PxU32 batchSize = params.batchSize;

	const PxI32 UnrollCount = PxI32(batchSize);
	const PxI32 ArticCount = 2;
	const PxI32 SaveUnrollCount = 32;

	const PxI32 TempThresholdStreamSize = 32;
	ThresholdStreamElement tempThresholdStream[TempThresholdStreamSize];

	const PxI32 bodyListSize = PxI32(params.bodyListSize);
	const PxI32 articulationListSize = PxI32(params.articulationListSize);

	const PxI32 batchCount = PxI32(params.numConstraintHeaders);
	cache.mThresholdStream = tempThresholdStream;
	cache.mThresholdStreamLength = TempThresholdStreamSize;
	cache.mThresholdStreamIndex = 0;
	cache.writeBackIteration = false;
	cache.Z = Z;
	cache.deltaV = deltaV;

	const PxReal dt = params.dt;
	const PxReal invDt = params.invDt;

	const PxI32 positionIterations = PxI32(params.positionIterations);
	const PxI32 velocityIterations = PxI32(params.velocityIterations);

	PxI32* constraintIndex = &params.constraintIndex; // counter for distributing constraints to tasks, incremented before they're solved
	PxI32* constraintIndexCompleted = &params.constraintIndexCompleted; // counter for completed constraints, incremented after they're solved

	PxI32* articIndex = &params.articSolveIndex;
	PxI32* articIndexCompleted = &params.articSolveIndexCompleted;

	PxSolverConstraintDesc* PX_RESTRICT constraintList = params.constraintList;

	ArticulationSolverDesc* PX_RESTRICT articulationListStart = params.articulationListStart;

	const PxU32 nbPartitions = params.nbPartitions;	

	PxU32* headersPerPartition = params.headersPerPartition;

	PX_UNUSED(velocityIterations);

	PX_ASSERT(velocityIterations >= 1);
	PX_ASSERT(positionIterations >= 1);

	PxI32 endIndexCount = UnrollCount;
	PxI32 index = PxAtomicAdd(constraintIndex, UnrollCount) - UnrollCount;

	PxI32 articSolveStart = 0;
	PxI32 articSolveEnd = 0;
	PxI32 maxArticIndex = 0;
	PxI32 articIndexCounter = 0;
	
	BatchIterator contactIter(params.constraintBatchHeaders, params.numConstraintHeaders);

	PxI32 maxNormalIndex = 0;
	PxI32 normalIteration = 0;
	PxU32 a = 0;
	PxI32 targetConstraintIndex = 0;
	PxI32 targetArticIndex = 0;
	
	for(PxU32 i = 0; i < 2; ++i)
	{
		SolveBlockMethod* solveTable = i == 0 ? gVTableSolveBlock : gVTableSolveConcludeBlock;
		for(; a < positionIterations - 1 + i; ++a)
		{
			WAIT_FOR_PROGRESS(articIndexCompleted, targetArticIndex); // wait for arti solve of previous iteration

			cache.doFriction = this->frictionEveryIteration ? true : (positionIterations - a) <= 3;
			for(PxU32 b = 0; b < nbPartitions; ++b)
			{
				WAIT_FOR_PROGRESS(constraintIndexCompleted, targetConstraintIndex); // wait for rigid solve of previous partition

				maxNormalIndex += headersPerPartition[b];
				
				PxI32 nbSolved = 0;
				while(index < maxNormalIndex)
				{
					const PxI32 remainder = PxMin(maxNormalIndex - index, endIndexCount);
					SolveBlockParallel(constraintList, remainder, index, batchCount, cache, contactIter, solveTable, 
						normalIteration);
					index += remainder;
					endIndexCount -= remainder;
					nbSolved += remainder;
					if(endIndexCount == 0)
					{
						endIndexCount = UnrollCount;
						index = PxAtomicAdd(constraintIndex, UnrollCount) - UnrollCount;
					}
				}
				if(nbSolved)
				{
					PxMemoryBarrier();
					PxAtomicAdd(constraintIndexCompleted, nbSolved);
				}
				targetConstraintIndex += headersPerPartition[b]; //Increment target constraint index by batch count
			}

			WAIT_FOR_PROGRESS(constraintIndexCompleted, targetConstraintIndex); // wait for all rigid partitions to be done

			maxArticIndex += articulationListSize;
			targetArticIndex += articulationListSize;

			while (articSolveStart < maxArticIndex)
			{
				const PxI32 endIdx = PxMin(articSolveEnd, maxArticIndex);

				PxI32 nbSolved = 0;
				while (articSolveStart < endIdx)
				{
					articulationListStart[articSolveStart - articIndexCounter].articulation->solveInternalConstraints(dt, invDt, cache.Z, cache.deltaV, false, false, 0.f, 0.8f);
					articSolveStart++;
					nbSolved++;
				}

				if (nbSolved)
				{
					PxMemoryBarrier();
					PxAtomicAdd(articIndexCompleted, nbSolved);
				}

				const PxI32 remaining = articSolveEnd - articSolveStart;

				if (remaining == 0)
				{
					articSolveStart = PxAtomicAdd(articIndex, ArticCount) - ArticCount;
					articSolveEnd = articSolveStart + ArticCount;
				}
			}

			articIndexCounter += articulationListSize;

			++normalIteration;
		}
	}

	PxI32* bodyListIndex = &params.bodyListIndex;
	PxI32* bodyListIndexCompleted = &params.bodyListIndexCompleted;

	PxSolverBody* PX_RESTRICT bodyListStart = params.bodyListStart;
	Cm::SpatialVector* PX_RESTRICT motionVelocityArray = params.motionVelocityArray;

	//Save velocity - articulated
	PxI32 endIndexCount2 = SaveUnrollCount;
	PxI32 index2 = PxAtomicAdd(bodyListIndex, SaveUnrollCount) - SaveUnrollCount;
	{
		WAIT_FOR_PROGRESS(articIndexCompleted, targetArticIndex); // wait for all articulation solves before saving velocity
		WAIT_FOR_PROGRESS(constraintIndexCompleted, targetConstraintIndex); // wait for all rigid partition solves before saving velocity
		PxI32 nbConcluded = 0;
		while(index2 < articulationListSize)
		{
			const PxI32 remainder = PxMin(SaveUnrollCount, (articulationListSize - index2));
			endIndexCount2 -= remainder;
			for(PxI32 b = 0; b < remainder; ++b, ++index2)
			{
				ArticulationPImpl::saveVelocity(articulationListStart[index2].articulation, cache.deltaV);
			}
			if(endIndexCount2 == 0)
			{
				index2 = PxAtomicAdd(bodyListIndex, SaveUnrollCount) - SaveUnrollCount;
				endIndexCount2 = SaveUnrollCount;
			}
			nbConcluded += remainder;
		}

		index2 -= articulationListSize;

		//save velocity
		
		while(index2 < bodyListSize)
		{
			const PxI32 remainder = PxMin(endIndexCount2, (bodyListSize - index2));
			endIndexCount2 -= remainder;
			for(PxI32 b = 0; b < remainder; ++b, ++index2)
			{
				PxPrefetchLine(&bodyListStart[index2 + 8]);
				PxPrefetchLine(&motionVelocityArray[index2 + 8]);
				PxSolverBody& body = bodyListStart[index2];
				Cm::SpatialVector& motionVel = motionVelocityArray[index2];
				motionVel.linear = body.linearVelocity;
				motionVel.angular = body.angularState;
				PX_ASSERT(motionVel.linear.isFinite());
				PX_ASSERT(motionVel.angular.isFinite());
			}

			nbConcluded += remainder;
			
			//Branch not required because this is the last time we use this atomic variable
			//if(index2 < articulationListSizePlusbodyListSize)
			{
				index2 = PxAtomicAdd(bodyListIndex, SaveUnrollCount) - SaveUnrollCount - articulationListSize;
				endIndexCount2 = SaveUnrollCount;
			}
		}

		if(nbConcluded)
		{
			PxMemoryBarrier();
			PxAtomicAdd(bodyListIndexCompleted, nbConcluded);
		}
	}

	WAIT_FOR_PROGRESS(bodyListIndexCompleted, (bodyListSize + articulationListSize)); // wait for all velocity saves to be done

	a = 1;
	for(; a < params.velocityIterations; ++a)
	{
		WAIT_FOR_PROGRESS(articIndexCompleted, targetArticIndex); // wait for arti solve of previous iteration
		for(PxU32 b = 0; b < nbPartitions; ++b)
		{
			WAIT_FOR_PROGRESS(constraintIndexCompleted, targetConstraintIndex); // wait for rigid solve of previous partition

			maxNormalIndex += headersPerPartition[b];
			
			PxI32 nbSolved = 0;
			while(index < maxNormalIndex)
			{
				const PxI32 remainder = PxMin(maxNormalIndex - index, endIndexCount);
				SolveBlockParallel(constraintList, remainder, index, batchCount, cache, contactIter, gVTableSolveBlock, 
					normalIteration);
				index += remainder;
				endIndexCount -= remainder;
				nbSolved += remainder;
				if(endIndexCount == 0)
				{
					endIndexCount = UnrollCount;
					index = PxAtomicAdd(constraintIndex, UnrollCount) - UnrollCount;
				}
			}
			if(nbSolved)
			{
				PxMemoryBarrier();
				PxAtomicAdd(constraintIndexCompleted, nbSolved);
			}
			targetConstraintIndex += headersPerPartition[b]; //Increment target constraint index by batch count
		}

		WAIT_FOR_PROGRESS(constraintIndexCompleted, targetConstraintIndex); // wait for all rigid partitions to be done

		maxArticIndex += articulationListSize;
		targetArticIndex += articulationListSize;

		while (articSolveStart < maxArticIndex)
		{
			const PxI32 endIdx = PxMin(articSolveEnd, maxArticIndex);

			PxI32 nbSolved = 0;
			while (articSolveStart < endIdx)
			{
				articulationListStart[articSolveStart - articIndexCounter].articulation->solveInternalConstraints(dt, invDt, cache.Z, cache.deltaV, true, false, 0.f, 0.8f);
				articSolveStart++;
				nbSolved++;
			}

			if (nbSolved)
			{
				PxMemoryBarrier();
				PxAtomicAdd(articIndexCompleted, nbSolved);
			}

			const PxI32 remaining = articSolveEnd - articSolveStart;

			if (remaining == 0)
			{
				articSolveStart = PxAtomicAdd(articIndex, ArticCount) - ArticCount;
				articSolveEnd = articSolveStart + ArticCount;
			}
		}
		++normalIteration;
		articIndexCounter += articulationListSize;
	}

	ThresholdStreamElement* PX_RESTRICT thresholdStream = params.thresholdStream;
	PxU32 thresholdStreamLength = params.thresholdStreamLength;
	PxI32* outThresholdPairs = params.outThresholdPairs;

	cache.mSharedOutThresholdPairs = outThresholdPairs;
	cache.mSharedThresholdStream = thresholdStream;
	cache.mSharedThresholdStreamLength = thresholdStreamLength;

	//Last iteration - do writeback as well!
	cache.writeBackIteration = true;
	{
		WAIT_FOR_PROGRESS(articIndexCompleted, targetArticIndex); // wait for arti velocity iterations to be done
		for(PxU32 b = 0; b < nbPartitions; ++b)
		{
			WAIT_FOR_PROGRESS(constraintIndexCompleted, targetConstraintIndex); // wait for rigid partition velocity iterations to be done resp. previous partition writeback iteration

			maxNormalIndex += headersPerPartition[b];
			
			PxI32 nbSolved = 0;
			while(index < maxNormalIndex)
			{
				const PxI32 remainder = PxMin(maxNormalIndex - index, endIndexCount);

				SolveBlockParallel(constraintList, remainder, index, batchCount, cache, contactIter, gVTableSolveWriteBackBlock, 
					normalIteration);

				index += remainder;
				endIndexCount -= remainder;
				nbSolved += remainder;
				if(endIndexCount == 0)
				{
					endIndexCount = UnrollCount;
					index = PxAtomicAdd(constraintIndex, UnrollCount) - UnrollCount;
				}
			}
			if(nbSolved)
			{
				PxMemoryBarrier();
				PxAtomicAdd(constraintIndexCompleted, nbSolved);
			}
			targetConstraintIndex += headersPerPartition[b]; //Increment target constraint index by batch count
		}
		{
			WAIT_FOR_PROGRESS(constraintIndexCompleted, targetConstraintIndex); // wait for rigid partitions writeback iterations to be done

			maxArticIndex += articulationListSize;
			targetArticIndex += articulationListSize;

			while (articSolveStart < maxArticIndex)
			{
				const PxI32 endIdx = PxMin(articSolveEnd, maxArticIndex);

				PxI32 nbSolved = 0;
				while (articSolveStart < endIdx)
				{
					articulationListStart[articSolveStart - articIndexCounter].articulation->solveInternalConstraints(dt, invDt, cache.Z, cache.deltaV, false, false, 0.f, 0.8f);
					articulationListStart[articSolveStart - articIndexCounter].articulation->writebackInternalConstraints(false);
					articSolveStart++;
					nbSolved++;
				}

				if (nbSolved)
				{
					PxMemoryBarrier();
					PxAtomicAdd(articIndexCompleted, nbSolved);
				}

				PxI32 remaining = articSolveEnd - articSolveStart;

				if (remaining == 0)
				{
					articSolveStart = PxAtomicAdd(articIndex, ArticCount) - ArticCount;
					articSolveEnd = articSolveStart + ArticCount;
				}
			}

			articIndexCounter += articulationListSize; // not strictly necessary but better safe than sorry
			WAIT_FOR_PROGRESS(articIndexCompleted, targetArticIndex); // wait for arti solve+writeback to be done
		}

		// At this point we've awaited the completion all rigid partitions and all articulations
		// No more syncing on the outside of this function is required.

		if(cache.mThresholdStreamIndex > 0)
		{
			//Write back to global buffer
			PxI32 threshIndex = PxAtomicAdd(outThresholdPairs, PxI32(cache.mThresholdStreamIndex)) - PxI32(cache.mThresholdStreamIndex);
			for(PxU32 b = 0; b < cache.mThresholdStreamIndex; ++b)
			{
				thresholdStream[b + threshIndex] = cache.mThresholdStream[b];
			}
			cache.mThresholdStreamIndex = 0;
		}

		++normalIteration;
	}

#if PX_PROFILE_SOLVE_STALLS

	PxU64 endTime = readTimer();
	PxReal totalTime = (PxReal)(endTime - startTime);
	PxReal stallTime = (PxReal)stallCount;
	PxReal stallRatio = stallTime/totalTime;
	if(0)//stallRatio > 0.2f)
	{
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency( &frequency );
		printf("Warning -- percentage time stalled = %f; stalled for %f seconds; total Time took %f seconds\n", 
			stallRatio * 100.f, stallTime/(PxReal)frequency.QuadPart, totalTime/(PxReal)frequency.QuadPart);
	}
#endif
}

void SolverCoreGeneral::writeBackV
(const PxSolverConstraintDesc* PX_RESTRICT constraintList, const PxU32 /*constraintListSize*/, PxConstraintBatchHeader* batchHeaders, const PxU32 numBatches,
 ThresholdStreamElement* PX_RESTRICT thresholdStream, const PxU32 thresholdStreamLength, PxU32& outThresholdPairs,
 PxSolverBodyData* atomListData, WriteBackBlockMethod writeBackTable[]) const
{
	SolverContext cache;
	cache.solverBodyArray			= atomListData;
	cache.mThresholdStream			= thresholdStream;
	cache.mThresholdStreamLength	= thresholdStreamLength;
	cache.mThresholdStreamIndex		= 0;

	PxI32 outThreshIndex = 0;
	for(PxU32 j = 0; j < numBatches; ++j)
	{
		PxU8 type = *constraintList[batchHeaders[j].startIndex].constraint;
		writeBackTable[type](constraintList + batchHeaders[j].startIndex,
			batchHeaders[j].stride, cache);
	}

	outThresholdPairs = PxU32(outThreshIndex);
}

}
}


//#endif
