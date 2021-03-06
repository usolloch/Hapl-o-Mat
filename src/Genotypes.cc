/*
 * Hapl-o-Mat: A software for haplotype inference
 *
 * Copyright (C) 2016, DKMS gGmbH 
 *
 * Dr. Jürgen Sauter
 * Kressbach 1
 * 72072 Tuebingen, Germany
 *
 * T +49 7071 943-2060
 * F +49 7071 943-2090
 * sauter(at)dkms.de
 *
 * This file is part of Hapl-o-Mat
 *
 * Hapl-o-Mat is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * Hapl-o-Mat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Hapl-o-Mat; see the file COPYING.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <iostream>

#include "Exceptions.h"
#include "Genotypes.h"
#include "Typedefs.h"
#include "Utility.h"

std::shared_ptr<Locus> GLSGenotype::resolve(const bool doAmbiguityFilter, const bool expandAmbiguityLines) const{

  std::shared_ptr<Locus> pLocus;

  if(singleLocusGenotype.find("|") != std::string::npos){
    strVec_t genotypes = split(singleLocusGenotype, '|');
    
    strArrVec_t in_phasedLocus;
    for(auto genotype : genotypes){
      strVec_t alleles = split(genotype, '+');
      std::array<std::string, 2> splittedGenotype;
      for(size_t pos = 0; pos < alleles.size(); pos++)
	splittedGenotype.at(pos) = alleles.at(pos);
      in_phasedLocus.push_back(splittedGenotype);
    }
    pLocus = std::make_shared<PhasedLocus> (in_phasedLocus, wantedResolution);
  }
  else if (singleLocusGenotype.find("/") != std::string::npos){
    strVec_t separatePlus;
    separatePlus = split(singleLocusGenotype, '+');
    strVec_t lhs = split(separatePlus.at(0), '/');
    strVec_t rhs = split(separatePlus.at(1), '/');
    strVecArr_t in_unphasedLocus;
    in_unphasedLocus.at(0) = lhs;
    in_unphasedLocus.at(1) = rhs;
    pLocus = std::make_shared<UnphasedLocus> (in_unphasedLocus, wantedResolution, doAmbiguityFilter, expandAmbiguityLines);
  }
  else{
    strArrVec_t in_phasedLocus;
    strVec_t alleles = split(singleLocusGenotype, '+');    
    std::array<std::string, 2> splittedGenotype;
    for(size_t pos = 0; pos < alleles.size(); pos++)
      splittedGenotype.at(pos) = alleles.at(pos);
    in_phasedLocus.push_back(splittedGenotype);
    pLocus = std::make_shared<PhasedLocus> (in_phasedLocus, wantedResolution);
  }

  pLocus->resolve();

  return pLocus;
}

void GLSGenotype::orderSingleLocusGenotype(){

  if(singleLocusGenotype.find("|") != std::string::npos){
    strVec_t genotypes = split(singleLocusGenotype, '|');
    for(auto genotype = genotypes.begin();
	genotype != genotypes.end();
	genotype ++)
      {
	strVec_t alleles = split(*genotype, '+');
	std::sort(alleles.begin(), alleles.end());
	*genotype = "";
	for(auto allele : alleles){
	  *genotype += allele + "+";
	}
	genotype->pop_back();
      }
    
    std::sort(genotypes.begin(), genotypes.end());
    singleLocusGenotype = "";
    for(auto genotype : genotypes)
      {
	singleLocusGenotype += genotype + "|";
      }
    singleLocusGenotype.pop_back();
  }
    
  else if(singleLocusGenotype.find("/") != std::string::npos){  
    strVec_t allelesAtLocusPositions = split(singleLocusGenotype, '+');
    for(auto allelesAtLocusPosition = allelesAtLocusPositions.begin();
	allelesAtLocusPosition != allelesAtLocusPositions.end();
	allelesAtLocusPosition ++)
      {
	strVec_t alleles = split(*allelesAtLocusPosition, '/');
	std::sort(alleles.begin(), alleles.end());
	*allelesAtLocusPosition = "";
	for(auto allele : alleles)
	  {
	    *allelesAtLocusPosition += allele + "/";
	  }
	allelesAtLocusPosition->pop_back();
      }
    
    std::sort(allelesAtLocusPositions.begin(), allelesAtLocusPositions.end());
    singleLocusGenotype = "";
    for(auto allelesAtLocusPosition : allelesAtLocusPositions)
      {
	singleLocusGenotype += allelesAtLocusPosition + "+";
      }
    singleLocusGenotype.pop_back();
  }

  else{
    strVec_t alleleAtLocusPositions = split(singleLocusGenotype, '+');
    std::sort(alleleAtLocusPositions.begin(), alleleAtLocusPositions.end());

    singleLocusGenotype = "";
    for(auto alleleAtLocusPosition : alleleAtLocusPositions)
      {
	singleLocusGenotype += alleleAtLocusPosition + "+";
      }    
    singleLocusGenotype.pop_back();
  }
}

void MACGenotype::buildSingleLocusGenotype(){

  std::sort(initialAllelesAtLocusPositions.begin(), initialAllelesAtLocusPositions.end());
  singleLocusGenotype = "";
  for(auto allele : initialAllelesAtLocusPositions){
    singleLocusGenotype += allele;
    singleLocusGenotype += "+";
  }
  singleLocusGenotype.pop_back();
}

void MACGenotype::resolveNMDPCode(const std::string code, strVec_t & newCodes) const{

  std::string nmdpCode = findNMDPCode(code);
  auto itFileNMDPCodes = fileNMDPCodes().getList().find(nmdpCode);
  if(itFileNMDPCodes != fileNMDPCodes().getList().cend())
    {
      std::string newCode = code;
      size_t positionAsterik = code.find('*') + 1;
      size_t positionNMDPCodeInCode = code.find(nmdpCode, positionAsterik);
      newCode.erase(positionNMDPCodeInCode);
      if(itFileNMDPCodes->second.find(':') != std::string::npos)
	{
	  std::size_t posLastColon = newCode.find_last_of(':');
	  newCode.erase(posLastColon);
	  posLastColon = newCode.find_last_of(':');
	  if(posLastColon == std::string::npos)
	    posLastColon = newCode.find_last_of('*');
	  newCode.erase(posLastColon+1);
	}

      strVec_t splittedCode = split(itFileNMDPCodes->second, '/');
      for(auto itSplittedCode : splittedCode)
	{
	  std::string newCode2 = newCode;
	  newCode2.append(itSplittedCode);
	  newCodes.push_back(newCode2);
	}//for splittedCode
    }     
  else
    {
      throw(MultipleAlleleCodeException(nmdpCode));
    }
}

std::shared_ptr<Locus> MACGenotype::resolve(const bool doAmbiguityFilter, const bool expandAmbiguityLines) const{

  std::shared_ptr<Locus> pLocus;
  strVecArr_t allelesAtLocusPositions;
  size_t locusPosition = 0;
  for(auto allele : initialAllelesAtLocusPositions){
    strVec_t alleles;
    if(checkNMDPCode(allele)){
      resolveNMDPCode(allele, alleles);
    }
    else{
      alleles.push_back(allele);
    }
    allelesAtLocusPositions.at(locusPosition) = alleles;
    locusPosition ++;
  }
  
  if(allelesAtLocusPositions.at(0).size() == 1 and allelesAtLocusPositions.at(1).size() == 1)
    {
      pLocus = std::make_shared<PhasedLocus>(allelesAtLocusPositions, wantedResolution);
    }
  else
    {
      pLocus = std::make_shared<UnphasedLocus> (allelesAtLocusPositions, wantedResolution, doAmbiguityFilter, expandAmbiguityLines);
    }
  
  pLocus->resolve();

  return pLocus;
}
