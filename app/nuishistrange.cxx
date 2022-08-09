#include "TFile.h"
#include "TH1.h"

#include <cstdio>

int main(int argc, char const * argv[]){
	TFile f(argv[1]);
	TH1 *h = nullptr;
	f.GetObject(argv[2],h);
	if(!h){ return 1; }
	printf("%.3f,%.3f", h->GetXaxis()->GetBinLowEdge(1),
	h->GetXaxis()->GetBinUpEdge(h->GetXaxis()->GetNbins()));
	return 0;
}