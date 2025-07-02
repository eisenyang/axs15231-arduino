idf_component_register(
    SRC_DIRS 
        "${COMPONENT_SRCS}"
        "."
        "include"
    INCLUDE_DIRS
        "${COMPONENT_ADD_INCLUDEDIRS}"
        "."
        "include"
    PRIV_INCLUDE_DIRS "${COMPONENT_PRIV_INCLUDEDIRS}")
# spiffs_create_partition_image(spiffs ../spiffs FLASH_IN_PROJECT)
