#!/bin/bash
# Written by Uwe Hermann <uwe@hermann-uwe.de>, released as public domain.
# Modified by Piotr Esden-Tempski <piotr@esden.net>, released as public domain.
# Modified by Benjamin Lesage <benjamin.lesage@irisa.fr>, released as public domain.

# Original repository: https://github.com/esden/summon-arm-toolchain

# 
# Requirements (example is for Debian, replace package names as needed):
#
# apt-get install flex bison  libncurses5-dev \
# autoconf texinfo build-essential
#
# Or on Ubuntu Maverick give `apt-get build-dep gcc-4.5` a try.
#

# Stop if any command fails
set -e

##############################################################################
# Default settings section
# You probably want to customize those
##############################################################################
#TARGET=mips		                 # Or: TARGET=arm-elf
TARGET=mips
PREFIX=$(pwd)/install	         # Install location of your final toolchain
DEPS_PREFIX=$(pwd)/lib_install # Dependencies temporary destination directory

# Set to `sudo` if you need superuser privileges while installing
SUDO=
# Set to 1 to be quieter while running
QUIET=0


##############################################################################
# Parsing command line parameters
##############################################################################
while [ $# -gt 0 ]; do
  case $1 in
    TARGET=*)
    TARGET=$(echo $1 | sed 's,^TARGET=,,')
    ;;
    PREFIX=*)
    PREFIX=$(echo $1 | sed 's,^PREFIX=,,')
    ;;
    SUDO=*)
    SUDO=$(echo $1 | sed 's,^SUDO=,,')
    ;;
    QUIET=*)
    QUIET=$(echo $1 | sed 's,^QUIET=,,')
    ;;
    *)
    echo "Unknown parameter: $1"
    exit 1
    ;;
  esac
  shift # shifting parameter list to access the next one
done

echo "Settings used for this build are:"
echo "TARGET=$TARGET"
echo "PREFIX=$PREFIX"
echo "SUDO=$SUDO"
echo "QUIET=$QUIET"
echo "Command line parameters PARAM=VALUE can be used to override default settings."

##############################################################################
# Version and download url settings section
##############################################################################
GCCVERSION=4.5.1
GCC=gcc-${GCCVERSION}
GCCURL=http://ftp.gnu.org/gnu/gcc/${GCC}/${GCC}.tar.gz

BINUTILS=binutils-2.20.1
NEWLIB=newlib-1.18.0
GDB=gdb-7.4.1

LIBELF=libelf-0.8.13

##############################################################################
# Flags section
##############################################################################

CPUS=$(getconf _NPROCESSORS_ONLN)
PARALLEL=-j$((CPUS + 1))
echo "${CPUS} cpu's detected running make with '${PARALLEL}' flag"

GDBFLAGS=
BINUTILFLAGS=

GCCFLAGS=
# -i added lbesnard otherwise compiler not installed for arm-elf
MAKEFLAGS="${PARALLEL} -i "
TARFLAGS=v

if [ ${QUIET} != 0 ]; then
    TARFLAGS=
    MAKEFLAGS="${MAKEFLAGS} -s"
fi

SUMMON_DIR=$(pwd)
SOURCES=${SUMMON_DIR}/sources
STAMPS=${SUMMON_DIR}/stamps

export PATH="${PREFIX}/bin:${PATH}"
export LD_LIBRARY_PATH="${DEPS_PREFIX}/lib:${PREFIX}/lib:${LD_LIBRARY_PATH}"
export LD_RUN_PATH="${DEPS_PREFIX}/lib:${PREFIX}/lib:${LD_RUN_PATH}"

export LD_FLAGS="${LD_FLAGS} -static"
export LD_FLAGS="${LD_FLAGS} -L${DEPS_PREFIX} -L${PREFIX}/lib"


##############################################################################
# Tool section
##############################################################################
TAR=tar

##############################################################################
# OS and Tooldetection section
# Detects which tools and flags to use
##############################################################################

case "$(uname)" in
	Linux)
	echo "Found Linux OS."
	;;
	Darwin)
	echo "Found Darwin OS."
	#exit 0
	;;
	*)
	echo "Found unknown OS. Aborting!"
	exit 1
	;;
esac

##############################################################################
# Building section
# You probably don't have to touch anything after this
##############################################################################

# Fetch a versioned file from a URL
function fetch {
    if [ ! -e ${STAMPS}/$1.fetch ]; then
        log "Downloading $1 sources..."
        wget -c  $2
        touch ${STAMPS}/$1.fetch
    fi
}

# Log a message out to the console
function log {
    echo "******************************************************************"
    echo "* $*"
    echo "******************************************************************"
}

# Unpack an archive
function unpack {
    log Unpacking $*
    # Use 'auto' mode decompression.  Replace with a switch if tar doesn't support -a
    ARCHIVE=$(ls ${SOURCES}/$1.tar.*)
    case ${ARCHIVE} in
	*.bz2)
	    echo "archive type bz2"
	    TYPE=j
	    ;;
	*.gz)
	    echo "archive type gz"
	    TYPE=z
	    ;;
	*)
	    echo "Unknown archive type of $1"
	    echo ${ARCHIVE}
	    exit 1
	    ;;
    esac
    ${TAR} xf${TYPE}${TARFLAGS} ${SOURCES}/$1.tar.*
}

# Install a build
function install {
    log $1
    ${SUDO} make ${MAKEFLAGS} $2 $3 $4 $5 $6 $7 $8
}

mkdir -p ${STAMPS} ${SOURCES}

cd ${SOURCES}

fetch ${BINUTILS} http://ftp.gnu.org/gnu/binutils/${BINUTILS}.tar.bz2
fetch ${GCC} ${GCCURL}
fetch ${NEWLIB} ftp://sources.redhat.com/pub/newlib/${NEWLIB}.tar.gz
fetch ${GDB} http://ftp.gnu.org/gnu/gdb/${GDB}.tar.bz2

fetch ${LIBELF} http://www.mr511.de/software/${LIBELF}.tar.gz


cd ${SUMMON_DIR}

if [ ! -e build/temp ]; then
    mkdir -p build/temp
fi

if [ ! -e ${STAMPS}/${LIBELF}.build ]; then
	cd build
	unpack ${LIBELF}
	cd temp
	log "Configuring ${LIBELF}"
	../${LIBELF}/configure --prefix=${DEPS_PREFIX} --disable-shared --enable-static
	make ${MAKEFLAGS}
	install ${LIBELF} install
	cd ..
	log "Cleaning up ${LIBELF}"
	rm -rf temp/* ${LIBELF}
	cd ..
	touch ${STAMPS}/${LIBELF}.build
fi

if [ ! -e ${STAMPS}/${BINUTILS}.build ]; then
    cd build
    unpack ${BINUTILS}
		cd temp
    log "Configuring ${BINUTILS}"
    ../${BINUTILS}/configure --target=${TARGET} \
                           --prefix=${PREFIX} \
                                             --disable-shared \
                           --enable-interwork \
                           --disable-multilib \
                           --with-gnu-as \
                           --with-gnu-ld \
                           --disable-nls \
                           --disable-werror \
			   ${BINUTILFLAGS}
    log "Building ${BINUTILS}"
    make ${MAKEFLAGS}
    install ${BINUTILS} install
    cd ..
    log "Cleaning up ${BINUTILS}"
    rm -rf temp/* ${BINUTILS}
		cd ..
    touch ${STAMPS}/${BINUTILS}.build
fi

if [ ! -e ${STAMPS}/${GCC}-${NEWLIB}.build ]; then
		cd build
		unpack ${GCC}
		unpack ${NEWLIB}
		log "Adding newlib symlink to gcc"
		ln -fs `pwd`/${NEWLIB}/newlib ${GCC}
		log "Adding libgloss symlink to gcc"
		ln -fs `pwd`/${NEWLIB}/libgloss ${GCC}
		log "Downloading prerequisites"
		cd ${GCC}
		./contrib/download_prerequisites
		cd ..
		cd temp
		log "Configuring ${GCC} and ${NEWLIB}"
		../${GCC}/configure --target=${TARGET} \
												--prefix=${PREFIX} \
--with-mpfr=${DEPS_PREFIX} --with-gmp=${DEPS_PREFIX} --with-mpc=${DEPS_PREFIX} \
												--enable-interwork \
												--disable-multilib \
												--enable-languages="c,c++" \
												--with-newlib \
												--with-gnu-as \
												--with-gnu-ld \
												--disable-nls \
												--disable-shared \
												--disable-threads \
												--with-headers=newlib/libc/include \
												--disable-werror \
												--with-system-zlib \
												--disable-newlib-supplied-syscalls \
												${GCCFLAGS}
		log "Building ${GCC} and ${NEWLIB}"
		make ${MAKEFLAGS}
		install ${GCC} install
		cd ..
		log "Cleaning up ${GCC} and ${NEWLIB}"
		rm -rf temp/* ${GCC} ${NEWLIB}
		cd ..
		touch ${STAMPS}/${GCC}-${NEWLIB}.build
fi


if [ ! -e ${STAMPS}/${GDB}.build ]; then
    cd build
    unpack ${GDB}
		cd temp
    log "Configuring ${GDB}"
    ../${GDB}/configure --target=${TARGET} \
                      --prefix=${PREFIX} \
                      --enable-interwork \
                      --disable-multilib \
                      --disable-werror \
		      ${GDBFLAGS}
    log "Building ${GDB}"
    make ${MAKEFLAGS}
    install ${GDB} install
    cd ..
    log "Cleaning up ${GDB}"
    rm -rf temp/* ${GDB}
    cd ..
    touch ${STAMPS}/${GDB}.build
fi
