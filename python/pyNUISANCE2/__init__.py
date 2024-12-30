# Helper function for jupyter loading
def source_bash_setup(script):
    import subprocess
    import os
    import sys

    # Run the setup script and capture the output
    completed_process = subprocess.run(
        ['bash', '-c', f'source {script} && env'],
        stdout=subprocess.PIPE,
        text=True
    )
    # Parse the environment variables and update the notebook environment
    for line in completed_process.stdout.splitlines():
        key, _, value = line.partition("=")
        os.environ[key] = value
        if key == "PYTHONPATH":
            for ndir in value.split(":"):
                sys.path.insert(0, ndir)


import ROOT
import cppyy
import os
import glob

prefix=os.environ["NUISANCE"]

cppyy.add_library_path(prefix + "/lib/")
cppyy.add_include_path(prefix + "/include/")

for lib in glob.glob(prefix + "/lib/*.so"):
    try:
      cppyy.load_library(lib)
    except:
      print("Failed to dynamic load ", lib)

for incfile in glob.glob(prefix + "/include/*.h"):
    try:
        cppyy.include(incfile)
    except:
        print("Failed to dynamic load ", incfile)

# Attempt import
try:
  from cppyy.gbl import SampleUtils
  sample = SampleUtils.CreateSample
except:
  raise RunTimeError("Failed to load sample utils in pyNUISANCE!")

try:
  from cppyy.gbl import InputUtils
  inputhandler = InputUtils.CreateInputHandler
except:
  pass

def input_type(typename):
  return InputUtils.ParseInputType(typename)

nullptr = 0

# Allows:
#anl = sample("ANL_CC1pi0_XSec_1DEnu_nu", "NEUT:test.root", "FIX", "", nullptr)
#events = anl.GetInput()
#print(events.GetNEvents())

def generate(generator, target, experiment, probe, mode, nevents, out="output-events.root"):
   import subprocess
   command = f"nuis-gen {generator} -t {target} -E {experiment} -P {probe} -M {mode} -n {nevents} -o {out}"
   subprocess.call(command.split())

