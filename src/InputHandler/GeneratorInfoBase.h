// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NUISANCE.
*
*    NUISANCE is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NUISANCE is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NUISANCE.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/
#ifndef GENERATOR_INFO_BASE_SEEN_H
#define GENERATOR_INFO_BASE_SEEN_H
/*!
 *  \addtogroup InputHandler
 *  @{
 */

/// Base Box object that allows user to save extra generator level info
/// by overriding functions. For an example see GENIEGeneratorInfo
class GeneratorInfoBase {
public:
  inline GeneratorInfoBase(){};
  inline virtual ~GeneratorInfoBase(){};
  inline virtual void AddBranchesToTree(TTree* tn){};
  inline virtual void SetBranchesFromTree(TTree* tn){};
  inline virtual void AllocateParticleStack(int stacksize){};
  inline virtual void DeallocateParticleStack(){};
  inline virtual void Reset(){};
};
/*! @} */
#endif
