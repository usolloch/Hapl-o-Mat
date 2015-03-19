#include "Phenotype.h"
#include "Haplotype.h"
#include <algorithm>
#include <iostream>

double Phenotype::computeSummedFrequencyDiplotypes () const{

  double summedFrequencyDiplotypes = 0.;
  auto itDiploEnd = c_diplotypeListEnd();
  for(auto itDiplo = c_diplotypeListBegin();
      itDiplo != itDiploEnd;
      itDiplo ++)
    {
      summedFrequencyDiplotypes += itDiplo->frequency;
    }

  return summedFrequencyDiplotypes;
}

void Phenotype::expectation(const HaplotypeList & haplotypeList){

  auto itDiploEnd = diplotypeList.end();
  for(auto itDiplo = diplotypeList.begin();
      itDiplo != itDiploEnd;
      itDiplo ++)
    {
      if(itDiplo->id1 == itDiplo->id2){
        itDiplo->frequency = haplotypeList.getFrequency(itDiplo->id1);
	itDiplo->frequency *= itDiplo->frequency;
      }
      else{
        itDiplo->frequency = haplotypeList.getFrequency(itDiplo->id1);
        itDiplo->frequency *= haplotypeList.getFrequency(itDiplo->id2);
        itDiplo->frequency *= 2.;
      }
    }
}

double Phenotype::derivative(const HaplotypeList & haplotypeList,
			     const size_t haplotype_k,
			     const size_t lastHaplotype) const{
  
  double result = 0.;
  auto itDiploEnd = diplotypeList.end();
  for(auto itDiplo = diplotypeList.begin();
      itDiplo != itDiploEnd;
      itDiplo ++)
    {
      double sum = 0;
      sum += derivativeHaplotypeFrequency(itDiplo->id1, haplotype_k, lastHaplotype) * haplotypeList.getFrequency(itDiplo->id2);
      sum += derivativeHaplotypeFrequency(itDiplo->id2, haplotype_k, lastHaplotype) * haplotypeList.getFrequency(itDiplo->id1);

      if(itDiplo->id1 != itDiplo->id2){
	sum *= 2.;
      }
      result += sum;      

    }//diplotypes
  
  return result;
}

int Phenotype::derivativeHaplotypeFrequency(const size_t haplotype,
					       const size_t haplotype_k,
					       const size_t lastHaplotype) const{
  
  int result = 0;
  if(haplotype != lastHaplotype){
    if(haplotype == haplotype_k){
      result = 1;
    }
  }
  else{
    if(haplotype_k != lastHaplotype){
      result = -1;
    }
  }
  
  return result;
}

double Phenotype::secondDerivative(const HaplotypeList & haplotypeList,
				   const size_t haplotype_k,
				   const size_t haplotype_l,
				   const size_t lastHaplotype) const{

  double result = 0.;
  auto itDiploEnd = diplotypeList.end();
  for(auto itDiplo = diplotypeList.begin();
      itDiplo != itDiploEnd;
      itDiplo ++)
    {
      double sum = 0;
      sum += derivativeHaplotypeFrequency(itDiplo->id1, haplotype_k, lastHaplotype) 
	* derivativeHaplotypeFrequency(itDiplo->id2, haplotype_l, lastHaplotype);
      sum += derivativeHaplotypeFrequency(itDiplo->id1, haplotype_l, lastHaplotype)
	* derivativeHaplotypeFrequency(itDiplo->id2, haplotype_k, lastHaplotype);

      if(itDiplo->id1 != itDiplo->id2){
	sum *= 2.;
      }
      result += sum;      
    }

  return result;
}

size_t PhenotypeList::computeSizeInBytes(){

  size_t sizeInBytes = 0;
  for(auto pheno : hashList){
    sizeInBytes += sizeof(pheno.first);
    sizeInBytes += sizeof(pheno.second);
    for(auto diplo = pheno.second.c_diplotypeListBegin();
	diplo != pheno.second.c_diplotypeListEnd();
	diplo ++){
      sizeInBytes += sizeof(*diplo);
    }
  }
  
  sizeInBytes += sizeof(hashList);
  
  return sizeInBytes;
}

void PhenotypeList::expectationStep(const HaplotypeList & haplotypeList){

  for(auto phenotype = hashList.begin();
      phenotype != hashList.end();
      phenotype ++){
    phenotype->second.expectation(haplotypeList);
  }
}
