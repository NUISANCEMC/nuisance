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

#ifndef NUISCLING_H
#define NUISCLING_H

#include <string>
#include <vector>

class FitEvent;

class nuiscling {
public:
  typedef bool (*filter_ftype)(FitEvent const *);
  typedef double (*project_ftype)(FitEvent const *);
  typedef double (*weight_ftype)(FitEvent const *);

private:
  nuiscling();
  static nuiscling *instance_;

public:
  static nuiscling & Get();

  bool LoadFile(std::string const &);

  filter_ftype GetFilterFunction(std::string const &);
  project_ftype GetProjectionFunction(std::string const &);
  weight_ftype GetWeightFunction(std::string const &);

  bool Filter(FitEvent const *, filter_ftype fn);
  std::vector<double> Project(FitEvent const *, std::vector<project_ftype> fns);
  std::vector<double> Weight(FitEvent const *, std::vector<weight_ftype> fns);

};

#endif