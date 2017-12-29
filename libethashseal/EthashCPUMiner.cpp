/*
	This file is part of cpp-ethereum.

	cpp-ethereum is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	cpp-ethereum is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with cpp-ethereum.  If not, see <http://www.gnu.org/licenses/>.
*/
/** @file EthashCPUMiner.cpp
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 *
 * Determines the PoW algorithm.
 */

#include "EthashCPUMiner.h"
#include <thread>
#include <chrono>
#include <boost/algorithm/string.hpp>
#include <random>

using namespace std;
using namespace dev;
using namespace eth;

unsigned EthashCPUMiner::s_numInstances = 0;


EthashCPUMiner::EthashCPUMiner(GenericMiner<EthashProofOfWork>::ConstructionInfo const& _ci):
	GenericMiner<EthashProofOfWork>(_ci), Worker("miner" + toString(index()))
{
}

EthashCPUMiner::~EthashCPUMiner()
{
	terminate();
}

void EthashCPUMiner::kickOff()
{
	stopWorking();
	startWorking();
}

void EthashCPUMiner::pause()
{
	stopWorking();
}

void EthashCPUMiner::workLoop()
{
	ctrace << "EthashCPUMiner start mining";
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	auto tid = std::this_thread::get_id();
	static std::mt19937_64 s_eng((utcTime() + std::hash<decltype(tid)>()(tid)));

	uint64_t tryNonce = s_eng();

	WorkPackage w = work();


	h256 boundary = w.boundary;
	unsigned hashCount = 1;
	for (; !shouldStop(); tryNonce++, hashCount++)
	{
		u256 value = 0;
		// 计算结果小于等于target的时候退出，报告找到的这个解
		if (value <= boundary)//&& submitProof())
			break;
		if (!(hashCount % 100))
			accumulateHashes(100);
	}
}

std::string EthashCPUMiner::platformInfo()
{
	string baseline = toString(std::thread::hardware_concurrency()) + "-thread CPU";
	return baseline;
}
