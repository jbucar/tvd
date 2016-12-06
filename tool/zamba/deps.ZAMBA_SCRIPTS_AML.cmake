# Install AML scripts files
INSTALL( DIRECTORY scripts/impl/aml/
        DESTINATION bin
	USE_SOURCE_PERMISSIONS
        FILES_MATCHING PATTERN "*"
)
