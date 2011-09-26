#!/bin/bash
cd ~
rm -Rf debbuild
mkdir debbuild
mkdir debbuild/SOURCES
cd debbuild/SOURCES
wget -O ffDiaporama.tar.gz http://svn.tuxfamily.org/viewvc.cgi/ffdiaporama_trunk.tar.gz?view=tar
tar -xzf ffDiaporama.tar.gz
rm ffDiaporama.tar.gz
cd ffdiaporama_trunk
rm ffDiaporama/locale/*.ts
mv ffDiaporama/Packaging/debian ffDiaporama/debian
rm -Rf ffDiaporama/Packaging
mv ffDiaporama ffdiaporama-1.0.0
tar -czf ../ffdiaporama_1.0.0.orig.tar.gz *
mv ffdiaporama-1.0.0 ~/debbuild/SOURCES/ffdiaporama-1.0.0
cd ../ffdiaporama-1.0.0
debuild -b --lintian-opts -i

