// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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
#ifndef OPENMPWRAPPER_H
#define OPENMPWRAPPER_H


// Have to define dummy opemmp variables incase its not enabled
#ifdef __USE_OPENMP__

#include <omp.h>

#else

typedef int omp_int_t;
inline omp_int_t omp_get_thread_num()  { return 0; }
inline omp_int_t omp_get_max_threads() { return 1; }

#endif

#endif