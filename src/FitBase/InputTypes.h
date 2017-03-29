//! Global Enum to define generator type being read with FitEvent
// Have to define kNORM as if its a generator for the time being.
enum generator_event_type {
  kUNKNOWN = 999,
  kNEUT = 0,
  kNIWG = 1,
  kNUWRO = 2,
  kT2K = 3,
  kCUSTOM = 4,
  kGENIE = 5,
  kEVTSPLINE = 6,
  kNUANCE = 7,
  kGiBUU = 8,
  kNORM = 9,
  kMODENORM = 10,
  kEMPTY = 11,
  kINPUTFITEVENT = 12,
  kNEWSPLINE = 13,
  kLIKEWEIGHT = 14,
  kSPLINEPARAMETER = 15,
};

inline std::ostream& operator<<(std::ostream& os,
                                generator_event_type const& gs) {
  switch (gs) {
    case kUNKNOWN: {
      return os << "kUNKNOWN";
    }
    case kNEUT: {
      return os << "kNEUT";
    }
    case kNIWG: {
      return os << "kNIWG";
    }
    case kNUWRO: {
      return os << "kNUWRO";
    }
    case kT2K: {
      return os << "kT2K";
    }
    case kCUSTOM: {
      return os << "kCUSTOM";
    }
    case kGENIE: {
      return os << "kGENIE";
    }
    case kEVTSPLINE: {
      return os << "kEVTSPLINE";
    }
    case kNUANCE: {
      return os << "kNUANCE";
    }
    case kGiBUU: {
      return os << "kGiBUU";
    }
    case kNORM: {
      return os << "kNORM";
    }
    case kMODENORM: {
      return os << "kMODENORM";
    }
    default: { return os << "kUNKNOWN"; }
  }
}