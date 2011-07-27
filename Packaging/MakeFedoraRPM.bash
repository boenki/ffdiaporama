#!/bin/bash
cd ~
rm -Rf rpmbuild
mkdir rpmbuild
mkdir rpmbuild/SOURCES
mkdir rpmbuild/SPECS
cd rpmbuild/SOURCES
wget -O ffDiaporama.tar.gz http://svn.tuxfamily.org/viewvc.cgi/ffdiaporama_trunk.tar.gz?view=tar
tar -xzf ffDiaporama.tar.gz
rm ffDiaporama.tar.gz
cd ffdiaporama_trunk
cp ffDiaporama/Packaging/ffDiaporama.SPEC ~/rpmbuild/SPECS
cp ffDiaporama/img/logo.xpm ~/rpmbuild/SOURCES
rm -Rf ffDiaporama/Packaging
rm ffDiaporama/locale/*.ts
mv ffDiaporama ffDiaporama-0.99.4
tar -czf ../ffDiaporama-0.99.4.tar.gz *
cd ..
rm -Rf ffdiaporama_trunk
cd ~
rpmbuild -ba rpmbuild/SPECS/ffDiaporama.SPEC

