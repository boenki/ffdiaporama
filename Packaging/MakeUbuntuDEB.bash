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
cp ffDiaporama/Packaging/debian ~/debbuild/SOURCES/ffdiaporama-0.99.2
rm -Rf ffDiaporama/Packaging
rm ffDiaporama/locale/*.ts
mv ffDiaporama ffdiaporama-0.99.2
tar -czf ../ffdiaporama_0.99.2.orig.tar.gz *
mv ffdiaporama-0.99.2 ~/debbuild/SOURCES/ffdiaporama-0.99.2
cd ffdiaporama-0.99.2
debuild -b --lintian-opts -i

