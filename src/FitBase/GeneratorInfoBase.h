// Box object that allows user to save extra generator level info.
class GeneratorInfoBase {
public:
  inline GeneratorInfoBase(){};
  inline virtual ~GeneratorInfoBase(){};
  inline virtual void AddBranchesToTree(TTree* tn){};
  inline virtual void SetBranchesFromTree(TTree* tn){};
};