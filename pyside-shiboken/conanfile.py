from conans import ConanFile

class TestbedConan(ConanFile):
  name = "Testbed"
  description = "Testbed"
  license = "None"
  url = "None"
  settings = "os", "arch", "compiler", "build_type"
  generators = [
    "cmake_paths",
  ]

  requires = [
    "Qt/5.15.2",
    # "PySide2/5.15.2.1@thirdparty/development",
    PySide2/5.12.6@thirdparty/development",
    "Python/3.7.7@thirdparty/development"
  ]
