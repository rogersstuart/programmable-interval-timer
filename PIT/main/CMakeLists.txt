# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )
file(GLOB_RECURSE files "./*.cpp")
set(COMPONENT_SRCS ${files})
set(COMPONENT_ADD_INCLUDEDIRS . ./Display ./UserInterface)

register_component()
