#!/bin/sh

# Stop if any command fails
#set -e

# CONFIGURATION
# svn username
USERNAME=anonymous
# packaged revision, use 'HEAD' for latest revision.
REVISION=HEAD
# repository root address (and access method)
REPOSITORY_ROOT="https://scm.gforge.inria.fr/svn"
# END CONFIGURATION

function fail()
{
	echo "$*" >&2
	exit 1
}

DESTINATION="salto_svn"
[ ! -e "${DESTINATION}" ] || \
	fail "Destination directory '${DESTINATION}' already exists." >&2 

ARCHIVE="salto.tgz"
[ ! -e "${ARCHIVE}" ] || \
	fail "Destination archive '${ARCHIVE}' already exists." >&2


svn export \
	--username "${USERNAME}" \
	--revision "${REVISION}" \
	--quiet \
	"${REPOSITORY_ROOT}/salto/trunk" \
	"${DESTINATION}"
[ $? -eq 0 ] || \
	fail "svn could not access HEPTANE repository." >&2 

tar -czf "${ARCHIVE}" "${DESTINATION}"
[ $? -eq 0 ] || \
	fail "error during archive '${ARCHIVE}' creation." >&1

rm -rf "${DESTINATION}"
[ $? -eq 0 ] || \
	fail "error during directory '${DESTINATION}' removal." >&1
