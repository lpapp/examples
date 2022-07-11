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
    "OpenSubdiv/3.4.3@thirdparty/development",
    "OpenEXR/2.4.2@thirdparty/development",
    "tbb/2019_U6@thirdparty/development",
    "USD/22.05",
    "QtSingleApplication/2.6@backup/CL571730"
  ]
