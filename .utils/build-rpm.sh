#!/bin/bash
set -ex

# Environment
export VERBOSE=1
export CTEST_OUTPUT_ON_FAILURE=1
export BRANCH=$1
export LC_ALL=C

# Platform-specific configuration
source /etc/os-release

CMAKEFLAGS="-DINSTALL_DOC=ON"

if [ "$ID" == "fedora" ]; then
  if [ "$VERSION_ID" -ge 30 ]; then
    PYTHON="python3"
    CMAKEFLAGS="$CMAKEFLAGS -DPYTHON_EXPLICIT_VERSION=3"
  else
    PYTHON="python"
  fi
elif [ "$ID" == "centos" ]; then
  yum install -y epel-release
  PYTHON="python"
fi

# Astrorama repository
cat > /etc/yum.repos.d/astrorama.repo << EOF
[bintray--astrorama-fedora]
name=bintray--astrorama-fedora
baseurl=https://dl.bintray.com/astrorama/travis/master/${ID}/\$releasever/\$basearch
gpgcheck=0
repo_gpgcheck=0
enabled=1
EOF

if [ "${BRANCH}" != "master" ]; then
  cat >> /etc/yum.repos.d/astrorama.repo <<EOF
[bintray--astrorama-fedora-develop]
name=bintray--astrorama-fedora-develop
baseurl=https://dl.bintray.com/astrorama/travis/develop/${ID}/\$releasever/\$basearch
gpgcheck=0
repo_gpgcheck=0
enabled=1
EOF
  CMAKEFLAGS="$CMAKEFLAGS -DCPACK_PACKAGE_RELEASE=dev"
fi

# From the CMakeLists.txt, retrieve the list of dependencies
cmake_deps=$(grep -oP 'elements_project\(\S+\s+\S+ USE \K(\S+ \S+)*(?=\))' /src/CMakeLists.txt)
rpm_dev_deps=$(echo ${cmake_deps} | awk '{for(i=1;i<NF;i+=2){print $i "-devel-" $(i+1)}}')
rpm_doc_deps=$(echo ${cmake_deps} | awk '{for(i=1;i<NF;i+=2){print $i "-doc-" $(i+1)}}')
yum install -y ${rpm_dev_deps} ${rpm_doc_deps}

# Dependencies
yum install -y cmake make gcc-c++ rpm-build
yum install -y boost-devel $PYTHON-pytest log4cpp-devel doxygen CCfits-devel qt5-devel
yum install -y graphviz $PYTHON-sphinx $PYTHON-sphinxcontrib-apidoc $PYTHON-astropy

# Build
mkdir -p /build
cd /build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DINSTALL_TESTS=OFF -DRPM_NO_CHECK=OFF $CMAKEFLAGS /src
make $MAKEFLAGS rpm

