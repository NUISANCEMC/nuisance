// MicroBooNEBlockHandler.h
//
// Parses a binning scheme definition file and exposes the result as four
// public data members.
//
//   hists      -- std::vector< std::shared_ptr<TH1D> >
//                 One histogram per 1D block; one per var0 slice for 2D blocks.
//                 Bins with ±inf edges are omitted from the x-axis.
//                 Titles and axis labels use ROOT TLatex conventions.
//
//   binMap     -- std::unordered_map< int, BinEntry >
//                 Maps every GlobalBinIndex to the histogram and bin that holds
//                 it, plus the product of the bin widths.
//                 BinEntry { int histIdx,   // zero-based index into hists
//                            int rootBin,   // one-based ROOT bin number
//                            double width } // product of var widths (finite dims)
//                 Infinite-edge bins are omitted.
//
//   blockBins  -- std::unordered_map< int, std::vector<int> >
//                 Maps block index -> sorted list of ALL GlobalBinIndices in
//                 that block (including infinite-edge bins).
//
//   blockHists -- std::unordered_map< int, std::vector<int> >
//                 Maps block index -> sorted list of zero-based indices into
//                 hists for all slice histograms belonging to that block.

#ifndef MICROBOONE_BLOCKHANDLER_H_SEEN
#define MICROBOONE_BLOCKHANDLER_H_SEEN

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "TH1D.h"

// =============================================================================
// Internal data structures
// =============================================================================

struct BinEdges1D { double lo, hi; };

struct BinDef {
  int        globalIdx;
  BinEdges1D var0;  // always present
  BinEdges1D var1;  // only for 2D blocks; default-initialised otherwise
};

struct Block {
  int blockIdx = 0;
  int binCount = 0;
  int varCount = 0;
  std::vector<std::string> varName; // raw LaTeX
  std::vector<std::string> varUnit; // raw LaTeX
  std::vector<BinDef>      bins;
};

// =============================================================================
// MicroBooNEBlockHandler
// =============================================================================

class MicroBooNEBlockHandler {
public:
  // ---------------------------------------------------------------------------
  // Public types
  // ---------------------------------------------------------------------------

  struct BinEntry {
    int    histIdx; // zero-based index into hists
    int    rootBin; // one-based ROOT bin number within that TH1D
    double width;   // product of finite bin widths from the input table
  };

  using BinMap     = std::unordered_map< int, BinEntry >;
  using BlockBins  = std::unordered_map< int, std::vector<int> >;
  using BlockHists = std::unordered_map< int, std::vector<int> >;

  // ---------------------------------------------------------------------------
  // Public data members
  // ---------------------------------------------------------------------------

  std::vector< std::shared_ptr<TH1D> > hists;
  BinMap     binMap;
  BlockBins  blockBins;
  BlockHists blockHists;

  // ---------------------------------------------------------------------------
  // Constructor
  // ---------------------------------------------------------------------------

  explicit MicroBooNEBlockHandler(const std::string& filename);

  // ---------------------------------------------------------------------------
  // Diagnostics
  // ---------------------------------------------------------------------------

  void printSummary(size_t nShow = 10) const;

private:
  // ---------------------------------------------------------------------------
  // Parsing helpers
  // ---------------------------------------------------------------------------

  static std::string nextQuoted(const std::string& line, size_t& pos);
  static double parseEdge(const std::string& tok);
  static std::vector<Block> parseFile(const std::string& filename);

  // ---------------------------------------------------------------------------
  // Label helpers
  // ---------------------------------------------------------------------------

  static std::string latexToRoot(const std::string& s);
  static std::string axisLabel(const std::string& latexName,
                                const std::string& latexUnits);
  static std::string edgeStr(double v);
  static std::string rangeStr(double lo, double hi,
                               const std::string& rootName,
                               const std::string& rootUnits);

  // ---------------------------------------------------------------------------
  // Axis-edge builder
  // ---------------------------------------------------------------------------

  static std::vector<double> makeEdges(const std::vector<BinEdges1D>& pairs);

  // ---------------------------------------------------------------------------
  // Core builder -- populates hists, binMap, blockBins, blockHists
  // ---------------------------------------------------------------------------

  void buildHistograms(const std::vector<Block>& blocks);
};

#endif // MICROBOONEBLOCKHANDLER_H_SEEN
