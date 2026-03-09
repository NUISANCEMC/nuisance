// MicroBooNEBlockHandler.cxx

#include "MicroBooNEBlockHandler.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <sstream>

#include "TAxis.h"

// =============================================================================
// Constructor
// =============================================================================

MicroBooNEBlockHandler::MicroBooNEBlockHandler(const std::string& filename)
{
  std::vector<Block> blocks = parseFile(filename);
  if (blocks.empty()) {
    std::cerr << "[MicroBooNEBlockHandler] No blocks parsed from \""
              << filename << "\".\n";
    return;
  }
  buildHistograms(blocks);
}

// =============================================================================
// Diagnostics
// =============================================================================

void MicroBooNEBlockHandler::printSummary(size_t nShow) const
{
  // --- histograms ---
  std::cout << "\nCreated " << hists.size() << " histogram(s).\n\n";
  for (size_t i = 0; i < hists.size(); ++i) {
    const auto& h = hists[i];
    std::cout << "[" << std::setw(3) << i << "]  "
              << std::setw(42) << std::left << h->GetTitle()
              << "  nbins=" << h->GetNbinsX()
              << "  xaxis=\"" << h->GetXaxis()->GetTitle() << "\"\n";
  }

  // --- binMap sample ---
  std::cout << "\nbinMap has " << binMap.size() << " entries.\n";
  std::cout << "Sample (globalBin -> {histIdx, rootBin, width}):\n";

  std::vector<int> keys;
  keys.reserve(binMap.size());
  for (auto& kv : binMap) keys.push_back(kv.first);
  std::sort(keys.begin(), keys.end());

  auto printBinEntry = [&](int k) {
    const auto& e = binMap.at(k);
    std::cout << "  globalBin " << std::setw(4) << k
              << "  ->  histIdx=" << std::setw(3) << e.histIdx
              << "  rootBin="    << std::setw(3) << e.rootBin
              << "  width="
              << std::setw(10) << std::setprecision(5) << e.width
              << "  (\"" << hists[e.histIdx]->GetTitle() << "\")\n";
  };

  size_t n = std::min(nShow, keys.size());
  for (size_t i = 0; i < n; ++i) printBinEntry(keys[i]);
  if (keys.size() > 2 * n) std::cout << "  ...\n";
  size_t start = keys.size() > n ? keys.size() - n : n;
  for (size_t i = start; i < keys.size(); ++i) printBinEntry(keys[i]);

  // --- blockBins summary ---
  std::cout << "\nblockBins has " << blockBins.size() << " block(s).\n";
  std::vector<int> blockKeys;
  blockKeys.reserve(blockBins.size());
  for (auto& kv : blockBins) blockKeys.push_back(kv.first);
  std::sort(blockKeys.begin(), blockKeys.end());
  for (int bk : blockKeys) {
    const auto& v = blockBins.at(bk);
    std::cout << "  block " << std::setw(2) << bk
              << "  ->  " << v.size() << " global bins"
              << "  [" << v.front() << " .. " << v.back() << "]\n";
  }

  // --- blockHists summary ---
  std::cout << "\nblockHists has " << blockHists.size() << " block(s).\n";
  std::vector<int> histBlockKeys;
  histBlockKeys.reserve(blockHists.size());
  for (auto& kv : blockHists) histBlockKeys.push_back(kv.first);
  std::sort(histBlockKeys.begin(), histBlockKeys.end());
  for (int bk : histBlockKeys) {
    const auto& v = blockHists.at(bk);
    std::cout << "  block " << std::setw(2) << bk
              << "  ->  " << v.size() << " hist(s)"
              << "  [" << v.front() << " .. " << v.back() << "]\n";
  }
  std::cout << "\n";
}

// =============================================================================
// Parsing helpers
// =============================================================================

std::string MicroBooNEBlockHandler::nextQuoted(const std::string& line, size_t& pos)
{
  size_t open = line.find('"', pos);
  if (open == std::string::npos) return "";
  size_t close = line.find('"', open + 1);
  if (close == std::string::npos) return "";
  pos = close + 1;
  return line.substr(open + 1, close - open - 1);
}

double MicroBooNEBlockHandler::parseEdge(const std::string& tok)
{
  if (tok == "inf")  return  std::numeric_limits<double>::infinity();
  if (tok == "-inf") return -std::numeric_limits<double>::infinity();
  return std::stod(tok);
}

std::vector<Block> MicroBooNEBlockHandler::parseFile(const std::string& filename)
{
  std::ifstream ifs(filename);
  if (!ifs.is_open()) {
    std::cerr << "[MicroBooNEBlockHandler] Cannot open file: " << filename << "\n";
    return {};
  }

  std::vector<Block> blocks;
  Block current;
  bool inBlock  = false;
  int  binsRead = 0;

  std::string line;
  while (std::getline(ifs, line)) {

    // Trim trailing whitespace / CR
    while (!line.empty() &&
           (line.back() == ' ' || line.back() == '\t' ||
            line.back() == '\r'))
      line.pop_back();

    if (line.empty()) {
      if (inBlock) { blocks.push_back(current); inBlock = false; }
      continue;
    }

    std::istringstream ss(line);

    if (!inBlock) {
      // Header line
      current  = Block{};
      binsRead = 0;
      ss >> current.blockIdx >> current.binCount >> current.varCount;

      size_t qpos = (size_t)ss.tellg();
      if ((int)ss.tellg() < 0) qpos = line.size();
      for (int v = 0; v < current.varCount; ++v) {
        current.varName.push_back(nextQuoted(line, qpos));
        current.varUnit.push_back(nextQuoted(line, qpos));
      }
      inBlock = true;
    }
    else {
      // Bin definition line
      BinDef bd{};
      std::string g, lo0s, hi0s;
      ss >> g >> lo0s >> hi0s;
      bd.globalIdx = std::stoi(g);
      bd.var0 = { parseEdge(lo0s), parseEdge(hi0s) };

      if (current.varCount == 2) {
        std::string lo1s, hi1s;
        ss >> lo1s >> hi1s;
        bd.var1 = { parseEdge(lo1s), parseEdge(hi1s) };
      }

      current.bins.push_back(bd);
      if (++binsRead == current.binCount) {
        blocks.push_back(current);
        inBlock = false;
      }
    }
  }
  if (inBlock) blocks.push_back(current); // no trailing blank line
  return blocks;
}

// =============================================================================
// Label helpers
// =============================================================================

std::string MicroBooNEBlockHandler::latexToRoot(const std::string& s)
{
  std::string out;
  out.reserve(s.size());
  for (char c : s) {
    if      (c == '$')  continue;
    else if (c == '\\') out += '#';
    else                out += c;
  }
  return out;
}

std::string MicroBooNEBlockHandler::axisLabel(const std::string& latexName,
                                     const std::string& latexUnits)
{
  std::string name  = latexToRoot(latexName);
  std::string units = latexToRoot(latexUnits);
  return units.empty() ? name : name + " (" + units + ")";
}

std::string MicroBooNEBlockHandler::edgeStr(double v)
{
  if (std::isinf(v)) return v > 0 ? "#infty" : "-#infty";
  std::ostringstream oss;
  oss << std::setprecision(4) << std::noshowpoint << v;
  return oss.str();
}

std::string MicroBooNEBlockHandler::rangeStr(double lo, double hi,
                                    const std::string& rootName,
                                    const std::string& rootUnits)
{
  std::ostringstream oss;
  oss << rootName << " #in [" << edgeStr(lo) << ", " << edgeStr(hi) << ")";
  if (!rootUnits.empty()) oss << " " << rootUnits;
  return oss.str();
}

// =============================================================================
// Axis-edge builder
// =============================================================================

std::vector<double> MicroBooNEBlockHandler::makeEdges(const std::vector<BinEdges1D>& pairs)
{
  std::set<double> s;
  for (auto& p : pairs) {
    if (std::isinf(p.lo) || std::isinf(p.hi)) continue;
    s.insert(p.lo);
    s.insert(p.hi);
  }
  return { s.begin(), s.end() };
}

// =============================================================================
// Core builder -- populates hists, binMap, blockBins, blockHists
// =============================================================================

void MicroBooNEBlockHandler::buildHistograms(const std::vector<Block>& blocks)
{
  int histCounter = 0;

  for (const auto& blk : blocks) {

    if (blk.varCount == 1) {
      // ------------------------------------------------------------------------
      // 1D block -> single histogram
      // ------------------------------------------------------------------------
      std::vector<BinEdges1D> pairs;
      for (auto& bd : blk.bins) pairs.push_back(bd.var0);

      std::vector<double> edges = makeEdges(pairs);
      if (edges.size() < 2) {
        std::cerr << "[MicroBooNEBlockHandler] Block " << blk.blockIdx
                  << ": not enough finite edges, skipping.\n";
        continue;
      }

      std::string hname  = "h_block" + std::to_string(blk.blockIdx)
                           + "_" + std::to_string(histCounter++);
      std::string htitle = "Block " + std::to_string(blk.blockIdx)
                           + ": " + latexToRoot(blk.varName[0]);

      auto h = std::make_shared<TH1D>(
        hname.c_str(), htitle.c_str(),
        (int)(edges.size() - 1), edges.data());
      h->GetXaxis()->SetTitle(
        axisLabel(blk.varName[0], blk.varUnit[0]).c_str());

      int histIdx = (int)hists.size();
      hists.push_back(h);
      blockHists[blk.blockIdx].push_back(histIdx);

      for (auto& bd : blk.bins) {
        blockBins[blk.blockIdx].push_back(bd.globalIdx); // all bins
        if (std::isinf(bd.var0.lo) || std::isinf(bd.var0.hi)) continue;
        int    rb    = h->GetXaxis()->FindFixBin(
                           0.5 * (bd.var0.lo + bd.var0.hi));
        double width = bd.var0.hi - bd.var0.lo;
        binMap[bd.globalIdx] = { histIdx, rb, width };
      }
    }
    else if (blk.varCount == 2) {
      // ------------------------------------------------------------------------
      // 2D block -> one histogram per unique var0 slice
      // ------------------------------------------------------------------------

      // Collect unique var0 slices in order of first appearance
      std::vector<BinEdges1D>           sliceOrder;
      std::map< std::pair<double,double>,
                std::vector<BinEdges1D> > sliceMap;

      for (auto& bd : blk.bins) {
        auto key = std::make_pair(bd.var0.lo, bd.var0.hi);
        if (sliceMap.find(key) == sliceMap.end())
          sliceOrder.push_back(bd.var0);
        sliceMap[key].push_back(bd.var1);
      }

      std::string rootName0  = latexToRoot(blk.varName[0]);
      std::string rootUnit0  = latexToRoot(blk.varUnit[0]);
      std::string xAxisLabel = axisLabel(blk.varName[1], blk.varUnit[1]);

      // var0-slice key -> histogram index
      std::map< std::pair<double,double>, int > sliceHistIdx;

      for (auto& sliceEdge : sliceOrder) {
        auto key = std::make_pair(sliceEdge.lo, sliceEdge.hi);
        std::vector<double> edges = makeEdges(sliceMap[key]);
        if (edges.size() < 2) {
          std::cerr << "[MicroBooNEBlockHandler] Block " << blk.blockIdx
                    << " slice [" << sliceEdge.lo << ","
                    << sliceEdge.hi << "): not enough finite edges, "
                    << "skipping.\n";
          continue;
        }

        std::string hname = "h_block" + std::to_string(blk.blockIdx)
                            + "_" + std::to_string(histCounter++);
        std::string htitle = "Block " + std::to_string(blk.blockIdx)
                             + ": " + rangeStr(sliceEdge.lo, sliceEdge.hi,
                                               rootName0, rootUnit0);

        auto h = std::make_shared<TH1D>(
          hname.c_str(), htitle.c_str(),
          (int)(edges.size() - 1), edges.data());
        h->GetXaxis()->SetTitle(xAxisLabel.c_str());

        int histIdx = (int)hists.size();
        sliceHistIdx[key] = histIdx;
        hists.push_back(h);
        blockHists[blk.blockIdx].push_back(histIdx);
      }

      // Populate binMap and blockBins.
      // blockBins gets every bin in the block; binMap only gets finite ones.
      for (auto& bd : blk.bins) {
        blockBins[blk.blockIdx].push_back(bd.globalIdx); // all bins
        if (std::isinf(bd.var1.lo) || std::isinf(bd.var1.hi)) continue;
        auto key = std::make_pair(bd.var0.lo, bd.var0.hi);
        auto it  = sliceHistIdx.find(key);
        if (it == sliceHistIdx.end()) continue; // slice skipped

        int         histIdx = it->second;
        const auto& h       = hists[histIdx];
        int rb = h->GetXaxis()->FindFixBin(
                     0.5 * (bd.var1.lo + bd.var1.hi));

        // Width = var0_width * var1_width.
        // If a var0 edge is ±inf (the slice itself is unbounded), treat
        // that dimension's width as 1 so the finite var1 width remains
        // useful for normalisation.
        double w0 = (std::isinf(bd.var0.lo) || std::isinf(bd.var0.hi))
                    ? 1.0 : bd.var0.hi - bd.var0.lo;
        double w1 = bd.var1.hi - bd.var1.lo;

        binMap[bd.globalIdx] = { histIdx, rb, w0 * w1 };
      }
    }

    // Keep blockBins and blockHists entries in ascending order
    auto& vb = blockBins[blk.blockIdx];
    std::sort(vb.begin(), vb.end());
    auto& vh = blockHists[blk.blockIdx];
    std::sort(vh.begin(), vh.end());
  }
}
