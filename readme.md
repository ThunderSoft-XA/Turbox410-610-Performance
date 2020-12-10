# Qualcomm TurboX C610&C410 Performance demo  Developer documentation

## brief introduction

This document mainly introduces the deployment and use of MultRtspDecoderApp environment。

## Environment configuration

##### 1、Installing the ADB tool

##### 2、Configure the compilation environment according to the release note document

##### 3、Write BB file, compile the executable file of MultiDecoder into the system image, and burn the system according to the realease note document

##### 4、Start the MultiDecoder according to the 《TurboxC610-MultiDecoderApp_User Guide》document

## For exsample of BB complie file

inherit autotools pkgconfig

DESCRIPTION = "Recipe to provide thundercomm bins library"
LICENSE = "CLOSED"
SECTION = "MultiDecoderApp"

#DEPENDS = "opencv"

DEPENDS = "libnl"
DEPENDS = "glib-2.0"
DEPENDS = "libutils"
DEPENDS = "libcutils"

RDEPENDS_multi-decoder += "glib-2.0"

DEPENDS = "gstreamer1.0"
DEPENDS = "gstreamer1.0-plugins-base"
DEPENDS = "gstreamer1.0-plugins-qti-oss-mlmeta"
DEPENDS = "gstreamer1.0-plugins-qti-oss-tools"
DEPENDS = "gstreamer1.0-rtsp-server"

SRC_URI = "\
	file://Turbox410-MultiDecoder  \
	"

#INSANE_SKIP_${PN}_append = "already-stripped"
S = "${WORKDIR}"

INSANE_SKIP_${PN} = "ldflags file-rdeps dev-deps"
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
INHIBIT_PACKAGE_STRIP = "1"

do_compile[noexec] = "1"

INSANE_SKIP_${PN}-dev += "dev-elf dev-deps"

do_install () {
  	echo "--- examples doing install ---"
  	echo ${D}
  	echo ${bindir}
  	echo ${D}${bindir}
  	echo ${WORK_DIR}
  	echo "--- examples before ---"
  	echo "--- examples end ---"
	install -d ${D}${libdir}/
	    
	install -d ${D}${bindir}
	install -m 0755 ${S}/MultiDecoderApp ${D}${bindir}

}
FILES_${PN} += "${bindir}/*"
FILES_${PN} += "${libdir}/*"
PARALLEL_MAKEINST = ""