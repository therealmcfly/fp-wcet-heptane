#!/bin/sh
# syntax: heptane-package.sh 
# Stop if any command fails
#set -e

HERE="$PWD"
function fail()
{
  echo "$*" >&2
  cd ${HERE}
  exit 1
}

EXPORTDIR=export
mkdir -p ${EXPORTDIR}; cd ${EXPORTDIR}

DESTINATION="heptane_git"
[ ! -e "${DESTINATION}" ] || fail "Destination directory '${DESTINATION}' already exists." >&2 

ARCHIVE="${DESTINATION}.tgz"
[ ! -e "${ARCHIVE}" ] || fail "Destination archive '${ARCHIVE}' already exists." >&2

echo ">>> Getting the version from the gitlab"
git clone https://gitlab.inria.fr/pacap/heptane.git
[ $? -eq 0 ] || fail "git: could not access HEPTANE repository." >&2 
mv heptane  "${DESTINATION}"
# svn export --quiet svn+ssh://${USERNAME}@scm.gforge.inria.fr/svnroot/heptane/trunk "${DESTINATION}"

echo ">>> Cleaning the target directory"
# cleaning.
# --------
FILES="${DESTINATION}/benchmarks/tasks_of_debie \
 ${DESTINATION}/src/TODO.txt \
${DESTINATION}/benchmarks/papabench \
${DESTINATION}/benchmarks/cover \
${DESTINATION}/benchmarks/edn \
${DESTINATION}/benchmarks/lms \
${DESTINATION}/benchmarks/st \
${DESTINATION}/scripts/heptane-package.sh \
${DESTINATION}/tools \
${DESTINATION}/.git"

for aFile in ${FILES}
do    
  rm -rf "${aFile}"
  [ $? -eq 0 ] || fail "error during directory '${aFile}' removal." >&1
done 

ed -s <<EOF 
r ${DESTINATION}/install.sh
,s/MODE_DEBUG=1/MODE_DEBUG=0/g
w
EOF

echo ">>> Creating the archive: ${DESTINATION}.tgz in ${EXPORTDIR} "
# Creating the archive.
# --------------------
tar -czf "${ARCHIVE}" "${DESTINATION}"
[ $? -eq 0 ] || fail "error during archive '${ARCHIVE}' creation." >&1

rm -rf "${DESTINATION}"
[ $? -eq 0 ] || fail "error during directory '${DESTINATION}' removal." >&1

cd ${HERE}

exit

