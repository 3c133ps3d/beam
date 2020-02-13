// Copyright 2018 The Beam Team
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#define USE_BASIC_CONFIG

#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wunused-function"
#else
#	pragma warning (push, 0) // suppress warnings from secp256k1
#	pragma warning (disable: 4706) // assignment within conditional expression
#endif

#include "secp256k1-zkp/src/basic-config.h"
#include "secp256k1-zkp/include/secp256k1.h"
#include "secp256k1-zkp/src/scalar.h"
#include "secp256k1-zkp/src/group.h"
#include "secp256k1-zkp/src/hash.h"

#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
#	pragma GCC diagnostic pop
#else
#	pragma warning (default: 4706)
#	pragma warning (pop)
#endif

namespace ECC_Min
{
	inline const uint32_t nBits = 256;

	struct MultiMac
	{
		typedef uint8_t Index;


		struct Prepared
		{
			static const int nBits = 4;
			static const int nMaxOdd = (1 << nBits) - 1;

			static const int nCount = (nMaxOdd >> 1) + 1;
			secp256k1_ge_storage m_pPt[nCount]; // odd powers
		};

		struct WNafCursor
		{
			uint16_t m_iBit;
			uint16_t m_iOdd;
			bool m_Negate;

			bool MoveNext(const secp256k1_scalar&, uint16_t nWndBits);
			void MoveNext2(const secp256k1_scalar&, uint16_t nWndBits);
			static uint8_t get_Bit(const secp256k1_scalar&, uint16_t iBit);
		};

		Index m_Prepared;

		MultiMac() { Reset(); }

		void Reset();
		void Calculate(secp256k1_gej&, const Prepared*, const secp256k1_scalar*, WNafCursor*);


	private:

		struct Normalizer;
	};

	template <unsigned int nMaxCount>
	struct MultiMac_WithBufs
		:public MultiMac
	{
		Prepared m_pPrepared[nMaxCount];
		WNafCursor m_pWnaf[nMaxCount];
		secp256k1_scalar m_pK[nMaxCount];

		void Add(const secp256k1_scalar& k)
		{
			static_assert(nMaxCount <= Index(-1));
			assert(m_Prepared < nMaxCount);
			m_pK[m_Prepared++] = k;
		}

		void Calculate(secp256k1_gej& res)
		{
			assert(m_Prepared <= nMaxCount);
			MultiMac::Calculate(res, m_pPrepared, m_pK, m_pWnaf);
		}
	};
} // namespace ECC_Min
