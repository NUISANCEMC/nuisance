#include <cassert>
#include <sstream>

#include "FitLogger.h"
#include "FitParameters.h"
#include "InputUtils.h"

int main(int argc, char const *argv[]) {
  LOG_VERB(SAM);
  LOG(FIT) << "*            Running InputUtils Tests" << std::endl;
  LOG(FIT) << "***************************************************"
           << std::endl;

  FitPar::Config().ForceParam("NEUT_DIR=/var/test/NEUT");
  FitPar::Config().ForceParam("NUWRO_DIR=/var/test/NUWRO");
  FitPar::Config().ForceParam("GENIE_DIR=/var/test/GENIE");
  FitPar::Config().ForceParam("GIBUU_DIR=/var/test/GIBUU");

  std::string NEUTInp = "NEUT:@NEUT_DIR/file.root";
  InputUtils::InputType NEUTInpt =
      InputUtils::ParseInputType(GeneralUtils::ParseToStr(NEUTInp, ":")[0]);
  std::string ExpandNEUT = InputUtils::ExpandInputDirectories(
      GeneralUtils::ParseToStr(NEUTInp, ":")[1]);
  bool IsJointNEUT_not =
      InputUtils::IsJointInput(GeneralUtils::ParseToStr(NEUTInp, ":")[1]);

  std::string NEUTJointInp = "NEUT:(@NEUT_DIR/file1.root,@NEUT_DIR/file2.root)";
  bool IsJointNEUT = InputUtils::IsJointInput(
      GeneralUtils::ParseToStr(NEUTJointInp, ":")[1]);
  std::string ExpandNEUTJoint = InputUtils::ExpandInputDirectories(
      GeneralUtils::ParseToStr(NEUTJointInp, ":")[1]);

  std::string NEUTJointInp_MissFSlash =
      "NEUT:(@NEUT_DIR/file1.root,@NEUT_DIR/file2.root)";
  std::string ExpandNEUTJoint_MissFSlash = InputUtils::ExpandInputDirectories(
      GeneralUtils::ParseToStr(NEUTJointInp_MissFSlash, ":")[1]);

  FitPar::Config().ForceParam("NEUT_DIR=/var/test/NEUT/");
  std::string NEUTJointInp_DoubleFSlash =
      "NEUT:(@NEUT_DIR/file1.root,@NEUT_DIR/file2.root)";
  std::string ExpandNEUTJoint_DoubleFSlash = InputUtils::ExpandInputDirectories(
      GeneralUtils::ParseToStr(NEUTJointInp_DoubleFSlash, ":")[1]);

  LOG(FIT) << "    *        Test input type parse" << std::endl;
  LOG(FIT) << "        *        Test parse 'NEUT'" << std::endl;
  if (!(NEUTInpt == InputUtils::kNEUT_Input)) {
    ERR(FTL) << GeneralUtils::ParseToStr(NEUTInp, ":")[0]
             << " parsed as type: " << NEUTInpt << std::endl;
  }
  assert(NEUTInpt == InputUtils::kNEUT_Input);
  LOG(FIT) << "    *        Test IsJoint check" << std::endl;
  LOG(FIT) << "        *        Test IsJoint on non joint" << std::endl;
  if (IsJointNEUT_not) {
    ERR(FTL) << GeneralUtils::ParseToStr(NEUTInp, ":")[1] << " parsed as Joint."
             << std::endl;
  }
  assert(!IsJointNEUT_not);
  LOG(FIT) << "        *        Test IsJoint on joint" << std::endl;
  if (!IsJointNEUT) {
    ERR(FTL) << GeneralUtils::ParseToStr(NEUTJointInp, ":")[1]
             << " parsed as not Joint." << std::endl;
  }
  assert(IsJointNEUT);
  LOG(FIT) << "    *        Test directory expansion" << std::endl;
  if ("/var/test/NEUT/file.root" != ExpandNEUT) {
    ERR(FTL) << GeneralUtils::ParseToStr(NEUTInp, ":")[1]
             << " expanded to: " << ExpandNEUT << std::endl;
  }
  assert("/var/test/NEUT/file.root" == ExpandNEUT);
  LOG(FIT) << "        *        Test joint directory expansion" << std::endl;
  if ("(/var/test/NEUT/file1.root,/var/test/NEUT/file2.root)" !=
      ExpandNEUTJoint) {
    ERR(FTL) << GeneralUtils::ParseToStr(NEUTJointInp, ":")[1]
             << " expanded to: " << ExpandNEUTJoint << std::endl;
  }
  assert("(/var/test/NEUT/file1.root,/var/test/NEUT/file2.root)" ==
         ExpandNEUTJoint);
  LOG(FIT) << "        *        Test joint directory expansion missing slash"
           << std::endl;
  if ("(/var/test/NEUT/file1.root,/var/test/NEUT/file2.root)" !=
      ExpandNEUTJoint_MissFSlash) {
    ERR(FTL) << GeneralUtils::ParseToStr(NEUTJointInp_MissFSlash, ":")[1]
             << " expanded to: " << ExpandNEUTJoint_MissFSlash << std::endl;
  }
  assert("(/var/test/NEUT/file1.root,/var/test/NEUT/file2.root)" ==
         ExpandNEUTJoint_MissFSlash);
  LOG(FIT) << "        *        Test joint directory expansion double slash"
           << std::endl;
  if ("(/var/test/NEUT/file1.root,/var/test/NEUT/file2.root)" !=
      ExpandNEUTJoint_DoubleFSlash) {
    ERR(FTL) << GeneralUtils::ParseToStr(NEUTJointInp_DoubleFSlash, ":")[1]
             << " expanded to: " << ExpandNEUTJoint_DoubleFSlash << std::endl;
  }
  assert("(/var/test/NEUT/file1.root,/var/test/NEUT/file2.root)" ==
         ExpandNEUTJoint_DoubleFSlash);

  LOG(FIT) << "*            Passed InputUtils Tests" << std::endl;
  LOG(FIT) << "***************************************************"
           << std::endl;
}
