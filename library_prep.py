import os, sys

base_path = os.path.realpath('.')

srcs_strs = ("LiquidCrystal_I2C/LiquidCrystal_I2C.cpp",
             "OneWire/OneWire.cpp",
             "Arduino-Temperature-Control-Library/DallasTemperature.cpp")
incld_strs = ("LiquidCrystal_I2C",
              "OneWire",
              "Arduino-Temperature-Control-Library")

with open(os.path.join(base_path, "PIT", "components", "arduino", "CMakeLists.txt"), 'r') as f:
    config = f.read()

for s in srcs_strs:
    s_loc = config.find("LIBRARY_SRCS")
    e_loc = config.find(')', s_loc, -1)

    if config.find(s, s_loc, e_loc) == -1:
        config = config[:e_loc] + "libraries/" + s + '\n' + config[e_loc:]

for s in incld_strs:
    s_loc = config.find("COMPONENT_ADD_INCLUDEDIRS")
    e_loc = config.find(')', s_loc, -1)

    if config.find(s, s_loc, e_loc) == -1:
        config = config[:e_loc] + "libraries/" + s + '\n' + config[e_loc:]

with open(os.path.join(base_path, "PIT", "components", "arduino", "CMakeLists.txt"), 'w') as f:
    f.write(config)