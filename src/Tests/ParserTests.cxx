#include <cassert>
#include <sstream>

#include "FitLogger.h"
#include "GeneralUtils.h"
#include "InputUtils.h"
#include "NuisConfig.h"
#include "StatUtils.h"

int main(int argc, char const *argv[]) {
  SETVERBOSITY(SAM);
  QLOG(FIT, "*            Running InputUtils Tests");
  QLOG(FIT, "***************************************************");

  Config::SetPar("NEUT_DIR", "/var/test/NEUT");
  Config::SetPar("NUWRO_DIR", "/var/test/NUWRO");
  Config::SetPar("GIBUU_DIR", "/var/test/NIBUU");
  Config::SetPar("GENIE_DIR", "/var/test/GENIE");

  std::string NEUTInp = "NEUT:@NEUT_DIR/file.root";
  InputUtils::InputType NEUTInpt =
      InputUtils::ParseInputType(GeneralUtils::ParseToStr(NEUTInp, ":")[0]);
  std::string ExpandNEUT = InputUtils::ExpandInputDirectories(
      GeneralUtils::ParseToStr(NEUTInp, ":")[1]);
  bool IsJointNEUT_not =
      InputUtils::IsJointInput(GeneralUtils::ParseToStr(NEUTInp, ":")[1]);

  std::string NEUTJointInp = "NEUT:(@NEUT_DIR/file1.root,@NEUT_DIR/file2.root)";
  bool IsJointNEUT =
      InputUtils::IsJointInput(GeneralUtils::ParseToStr(NEUTJointInp, ":")[1]);
  std::string ExpandNEUTJoint = InputUtils::ExpandInputDirectories(
      GeneralUtils::ParseToStr(NEUTJointInp, ":")[1]);

  std::string NEUTJointInp_MissFSlash =
      "NEUT:(@NEUT_DIR/file1.root,@NEUT_DIR/file2.root)";
  std::string ExpandNEUTJoint_MissFSlash = InputUtils::ExpandInputDirectories(
      GeneralUtils::ParseToStr(NEUTJointInp_MissFSlash, ":")[1]);

  Config::SetPar("NEUT_DIR", "/var/test/NEUT/");

  std::string NEUTJointInp_DoubleFSlash =
      "NEUT:(@NEUT_DIR/file1.root,@NEUT_DIR/file2.root)";
  std::string ExpandNEUTJoint_DoubleFSlash = InputUtils::ExpandInputDirectories(
      GeneralUtils::ParseToStr(NEUTJointInp_DoubleFSlash, ":")[1]);

  QLOG(FIT, "    *        Test input type parse");
  QLOG(FIT, "        *        Test parse 'NEUT'");
  if (!(NEUTInpt == InputUtils::kNEUT_Input)) {
    QERROR(FTL, GeneralUtils::ParseToStr(NEUTInp, ":")[0] << " parsed as type: "
                                                          << NEUTInpt);
  }
  assert(NEUTInpt == InputUtils::kNEUT_Input);
  QLOG(FIT, "    *        Test IsJoint check");
  QLOG(FIT, "        *        Test IsJoint on non joint");
  if (IsJointNEUT_not) {
    QERROR(FTL,
           GeneralUtils::ParseToStr(NEUTInp, ":")[1] << " parsed as Joint.");
  }
  assert(!IsJointNEUT_not);
  QLOG(FIT, "        *        Test IsJoint on joint");
  if (!IsJointNEUT) {
    QERROR(FTL, GeneralUtils::ParseToStr(NEUTJointInp, ":")[1]
                    << " parsed as not Joint.");
  }
  assert(IsJointNEUT);
  QLOG(FIT, "    *        Test directory expansion");
  if ("/var/test/NEUT/file.root" != ExpandNEUT) {
    QERROR(FTL, GeneralUtils::ParseToStr(NEUTInp, ":")[1] << " expanded to: "
                                                          << ExpandNEUT);
  }
  assert("/var/test/NEUT/file.root" == ExpandNEUT);
  QLOG(FIT, "        *        Test joint directory expansion");
  if ("(/var/test/NEUT/file1.root,/var/test/NEUT/file2.root)" !=
      ExpandNEUTJoint) {
    QERROR(FTL, GeneralUtils::ParseToStr(NEUTJointInp, ":")[1]
                    << " expanded to: " << ExpandNEUTJoint);
  }
  assert("(/var/test/NEUT/file1.root,/var/test/NEUT/file2.root)" ==
         ExpandNEUTJoint);
  QLOG(FIT, "        *        Test joint directory expansion missing slash");
  if ("(/var/test/NEUT/file1.root,/var/test/NEUT/file2.root)" !=
      ExpandNEUTJoint_MissFSlash) {
    QERROR(FTL, GeneralUtils::ParseToStr(NEUTJointInp_MissFSlash, ":")[1]
                    << " expanded to: " << ExpandNEUTJoint_MissFSlash);
  }
  assert("(/var/test/NEUT/file1.root,/var/test/NEUT/file2.root)" ==
         ExpandNEUTJoint_MissFSlash);
  QLOG(FIT, "        *        Test joint directory expansion double slash");
  if ("(/var/test/NEUT/file1.root,/var/test/NEUT/file2.root)" !=
      ExpandNEUTJoint_DoubleFSlash) {
    QERROR(FTL, GeneralUtils::ParseToStr(NEUTJointInp_DoubleFSlash, ":")[1]
                    << " expanded to: " << ExpandNEUTJoint_DoubleFSlash);
  }
  assert("(/var/test/NEUT/file1.root,/var/test/NEUT/file2.root)" ==
         ExpandNEUTJoint_DoubleFSlash);

  QLOG(FIT, "*            Passed InputUtils Tests");
  QLOG(FIT, "***************************************************");
}
