# Create .deb without using dpkg tools.
#
# Author: Tim Wegener <twegener@madabar.com>
#
# Use 'include deb_hand.mak' after defining the user variables in a local
# makefile.
#
# The 'data' rule must be customised in the local make file.
# This rule should make a 'data' directory containing the full file
# layout of the installed package.
#
# This makefile will create a debian-binary file a control directory and a
# a build directory in the current directory.
# Do 'make clobber' to remove these generated files.
#
# Destination:
# PACKAGE_DIR - directory where package (and support files) will be built
#               defaults to the current directory
#
# Sources:
# SOURCE_DIR - directory containing files to be packaged
# ICON_SOURCE - 26x26 icon file for maemo
# ${DESCRIPTION} - description with summary on first line
# preinst, postinst, prerm, postrm - optional control shell scripts

# These fields are used to build the control file:
# PACKAGE = 
# VERSION = 
# ARCH = 
# SECTION = 
# PRIORITY = 
# MAINTAINER = 
# DEPENDS = 
#
# SOURCE_DIR =
# ICON_SOURCE = 
# (ICON_SOURCE is optional)

# *** NO USER CHANGES REQUIRED BEYOND THIS POINT ***

PACKAGE_DIR ?= ${DISTRIBUTION_FOLDER}
CONTROL_EXTRAS ?= ${wildcard preinst postinst prerm postrm}

${PACKAGE_DIR}/control: ${PACKAGE_DIR}/data ${CONTROL_EXTRAS} ${PACKAGE}.txt \
			${ICON_SOURCE}
	rm -rf $@
	mkdir $@
ifneq (${CONTROL_EXTRAS},)
	cp ${CONTROL_EXTRAS} $@
endif
#       Make control file.
	echo "Package: ${PACKAGE}" > $@/control
	echo "Version: ${VERSION}" >> $@/control
	echo "Section: ${SECTION}" >> $@/control
	echo "Priority: ${PRIORITY}" >> $@/control
	echo "Architecture: ${ARCH}" >> $@/control
	echo "Depends: ${DEPENDS}" >> $@/control
	echo "Installed-Size: ${shell du -s ${PACKAGE_DIR}/data|cut -f1}" \
		>> $@/control
	echo "Maintainer: ${MAINTAINER}" >> $@/control
	echo -n "Description:" >> $@/control
	cat ${DESCRIPTION} | gawk '{print " "$$0;}' >> $@/control
ifneq (${ICON_SOURCE},)
	echo "Maemo-Icon-26:" >> $@/control
	uuencode -m ${ICON_SOURCE} ${ICON_SOURCE} | sed 1d | sed '$d' | gawk '{print " "$$0;}' >> $@/control
	echo >> $@/control
endif
#       Make md5sums.
	cd ${PACKAGE_DIR}/data && find . -type f -exec md5sum {} \; \
		| sed -e 's| \./||' \
		> $@/md5sums

${PACKAGE_DIR}/debian-binary:
	echo "2.0" > $@

${PACKAGE_DIR}/build: ${PACKAGE_DIR}/debian-binary ${PACKAGE_DIR}/control \
			${PACKAGE_DIR}/data
	rm -rf $@
	mkdir $@
	cp ${PACKAGE_DIR}/debian-binary $@/
	cd ${PACKAGE_DIR}/control && tar czvf $@/control.tar.gz ./*
	cd ${PACKAGE_DIR}/data && tar czvf $@/data.tar.gz ./*

# Convert GNU ar to BSD ar that debian requires.
# Note: Order of files within ar archive is important!
${PACKAGE_DIR}/${PACKAGE}_${VERSION}_${ARCH}.deb: ${PACKAGE_DIR}/build
	ar -rc $@tmp $</debian-binary $</control.tar.gz $</data.tar.gz
	mv $@tmp $@

.PHONY: data
data: ${PACKAGE_DIR}/data

.PHONY: control
control: ${PACKAGE_DIR}/control

.PHONY: build
build: ${PACKAGE_DIR}/build

.PHONY: deb
deb: ${PACKAGE_DIR}/${PACKAGE}_${VERSION}_${ARCH}.deb


clobber::
	rm -rf ${PACKAGE_DIR}/debian_binary ${PACKAGE_DIR}/control \
		${PACKAGE_DIR}/data ${PACKAGE_DIR}/build

