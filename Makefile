#############################################################################
# Makefile for building: ffDiaporama
# Generated by qmake (2.01a) (Qt 4.8.0) on: lun. 9. janv. 17:22:50 2012
# Project:  ffDiaporama.pro
# Template: subdirs
# Command: c:\Qt\qt-everywhere-opensource-src-4.8.0\bin\qmake -spec c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\win32-g++ CONFIG+=release -o Makefile ffDiaporama.pro
#############################################################################

first: make_default
MAKEFILE      = Makefile
QMAKE         = c:\Qt\qt-everywhere-opensource-src-4.8.0\bin\qmake
DEL_FILE      = del
CHK_DIR_EXISTS= if not exist
MKDIR         = mkdir
COPY          = copy /y
COPY_FILE     = $(COPY)
COPY_DIR      = xcopy /s /q /y /i
INSTALL_FILE  = $(COPY_FILE)
INSTALL_PROGRAM = $(COPY_FILE)
INSTALL_DIR   = $(COPY_DIR)
DEL_FILE      = del
SYMLINK       = 
DEL_DIR       = rmdir
MOVE          = move
CHK_DIR_EXISTS= if not exist
MKDIR         = mkdir
SUBTARGETS    =  \
		sub-src-ffDiaporama \
		sub-src-ffDiaporamaStart \
		sub-src-ffDiaporamaMMFiler

src\ffDiaporama\$(MAKEFILE): 
	@$(CHK_DIR_EXISTS) src\ffDiaporama\ $(MKDIR) src\ffDiaporama\ 
	cd src\ffDiaporama\ && $(QMAKE) y:\ffDiaporama\trunk\ffDiaporama\src\ffDiaporama\ffDiaporama.pro -spec c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\win32-g++ CONFIG+=release -o $(MAKEFILE)
sub-src-ffDiaporama-qmake_all:  FORCE
	@$(CHK_DIR_EXISTS) src\ffDiaporama\ $(MKDIR) src\ffDiaporama\ 
	cd src\ffDiaporama\ && $(QMAKE) y:\ffDiaporama\trunk\ffDiaporama\src\ffDiaporama\ffDiaporama.pro -spec c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\win32-g++ CONFIG+=release -o $(MAKEFILE)
sub-src-ffDiaporama: src\ffDiaporama\$(MAKEFILE) FORCE
	cd src\ffDiaporama\ && $(MAKE) -f $(MAKEFILE)
sub-src-ffDiaporama-make_default-ordered: src\ffDiaporama\$(MAKEFILE) FORCE
	cd src\ffDiaporama\ && $(MAKE) -f $(MAKEFILE) 
sub-src-ffDiaporama-make_default: src\ffDiaporama\$(MAKEFILE) FORCE
	cd src\ffDiaporama\ && $(MAKE) -f $(MAKEFILE) 
sub-src-ffDiaporama-make_first-ordered: src\ffDiaporama\$(MAKEFILE) FORCE
	cd src\ffDiaporama\ && $(MAKE) -f $(MAKEFILE) first
sub-src-ffDiaporama-make_first: src\ffDiaporama\$(MAKEFILE) FORCE
	cd src\ffDiaporama\ && $(MAKE) -f $(MAKEFILE) first
sub-src-ffDiaporama-all-ordered: src\ffDiaporama\$(MAKEFILE) FORCE
	cd src\ffDiaporama\ && $(MAKE) -f $(MAKEFILE) all
sub-src-ffDiaporama-all: src\ffDiaporama\$(MAKEFILE) FORCE
	cd src\ffDiaporama\ && $(MAKE) -f $(MAKEFILE) all
sub-src-ffDiaporama-clean-ordered: src\ffDiaporama\$(MAKEFILE) FORCE
	cd src\ffDiaporama\ && $(MAKE) -f $(MAKEFILE) clean
sub-src-ffDiaporama-clean: src\ffDiaporama\$(MAKEFILE) FORCE
	cd src\ffDiaporama\ && $(MAKE) -f $(MAKEFILE) clean
sub-src-ffDiaporama-distclean-ordered: src\ffDiaporama\$(MAKEFILE) FORCE
	cd src\ffDiaporama\ && $(MAKE) -f $(MAKEFILE) distclean
sub-src-ffDiaporama-distclean: src\ffDiaporama\$(MAKEFILE) FORCE
	cd src\ffDiaporama\ && $(MAKE) -f $(MAKEFILE) distclean
sub-src-ffDiaporama-install_subtargets-ordered: src\ffDiaporama\$(MAKEFILE) FORCE
	cd src\ffDiaporama\ && $(MAKE) -f $(MAKEFILE) install
sub-src-ffDiaporama-install_subtargets: src\ffDiaporama\$(MAKEFILE) FORCE
	cd src\ffDiaporama\ && $(MAKE) -f $(MAKEFILE) install
sub-src-ffDiaporama-uninstall_subtargets-ordered: src\ffDiaporama\$(MAKEFILE) FORCE
	cd src\ffDiaporama\ && $(MAKE) -f $(MAKEFILE) uninstall
sub-src-ffDiaporama-uninstall_subtargets: src\ffDiaporama\$(MAKEFILE) FORCE
	cd src\ffDiaporama\ && $(MAKE) -f $(MAKEFILE) uninstall
src\ffDiaporamaStart\$(MAKEFILE): 
	@$(CHK_DIR_EXISTS) src\ffDiaporamaStart\ $(MKDIR) src\ffDiaporamaStart\ 
	cd src\ffDiaporamaStart\ && $(QMAKE) y:\ffDiaporama\trunk\ffDiaporama\src\ffDiaporamaStart\ffDiaporamaStart.pro -spec c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\win32-g++ CONFIG+=release -o $(MAKEFILE)
sub-src-ffDiaporamaStart-qmake_all:  FORCE
	@$(CHK_DIR_EXISTS) src\ffDiaporamaStart\ $(MKDIR) src\ffDiaporamaStart\ 
	cd src\ffDiaporamaStart\ && $(QMAKE) y:\ffDiaporama\trunk\ffDiaporama\src\ffDiaporamaStart\ffDiaporamaStart.pro -spec c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\win32-g++ CONFIG+=release -o $(MAKEFILE)
sub-src-ffDiaporamaStart: src\ffDiaporamaStart\$(MAKEFILE) FORCE
	cd src\ffDiaporamaStart\ && $(MAKE) -f $(MAKEFILE)
sub-src-ffDiaporamaStart-make_default-ordered: src\ffDiaporamaStart\$(MAKEFILE) sub-src-ffDiaporama-make_default-ordered  FORCE
	cd src\ffDiaporamaStart\ && $(MAKE) -f $(MAKEFILE) 
sub-src-ffDiaporamaStart-make_default: src\ffDiaporamaStart\$(MAKEFILE) FORCE
	cd src\ffDiaporamaStart\ && $(MAKE) -f $(MAKEFILE) 
sub-src-ffDiaporamaStart-make_first-ordered: src\ffDiaporamaStart\$(MAKEFILE) sub-src-ffDiaporama-make_first-ordered  FORCE
	cd src\ffDiaporamaStart\ && $(MAKE) -f $(MAKEFILE) first
sub-src-ffDiaporamaStart-make_first: src\ffDiaporamaStart\$(MAKEFILE) FORCE
	cd src\ffDiaporamaStart\ && $(MAKE) -f $(MAKEFILE) first
sub-src-ffDiaporamaStart-all-ordered: src\ffDiaporamaStart\$(MAKEFILE) sub-src-ffDiaporama-all-ordered  FORCE
	cd src\ffDiaporamaStart\ && $(MAKE) -f $(MAKEFILE) all
sub-src-ffDiaporamaStart-all: src\ffDiaporamaStart\$(MAKEFILE) FORCE
	cd src\ffDiaporamaStart\ && $(MAKE) -f $(MAKEFILE) all
sub-src-ffDiaporamaStart-clean-ordered: src\ffDiaporamaStart\$(MAKEFILE) sub-src-ffDiaporama-clean-ordered  FORCE
	cd src\ffDiaporamaStart\ && $(MAKE) -f $(MAKEFILE) clean
sub-src-ffDiaporamaStart-clean: src\ffDiaporamaStart\$(MAKEFILE) FORCE
	cd src\ffDiaporamaStart\ && $(MAKE) -f $(MAKEFILE) clean
sub-src-ffDiaporamaStart-distclean-ordered: src\ffDiaporamaStart\$(MAKEFILE) sub-src-ffDiaporama-distclean-ordered  FORCE
	cd src\ffDiaporamaStart\ && $(MAKE) -f $(MAKEFILE) distclean
sub-src-ffDiaporamaStart-distclean: src\ffDiaporamaStart\$(MAKEFILE) FORCE
	cd src\ffDiaporamaStart\ && $(MAKE) -f $(MAKEFILE) distclean
sub-src-ffDiaporamaStart-install_subtargets-ordered: src\ffDiaporamaStart\$(MAKEFILE) sub-src-ffDiaporama-install_subtargets-ordered  FORCE
	cd src\ffDiaporamaStart\ && $(MAKE) -f $(MAKEFILE) install
sub-src-ffDiaporamaStart-install_subtargets: src\ffDiaporamaStart\$(MAKEFILE) FORCE
	cd src\ffDiaporamaStart\ && $(MAKE) -f $(MAKEFILE) install
sub-src-ffDiaporamaStart-uninstall_subtargets-ordered: src\ffDiaporamaStart\$(MAKEFILE) sub-src-ffDiaporama-uninstall_subtargets-ordered  FORCE
	cd src\ffDiaporamaStart\ && $(MAKE) -f $(MAKEFILE) uninstall
sub-src-ffDiaporamaStart-uninstall_subtargets: src\ffDiaporamaStart\$(MAKEFILE) FORCE
	cd src\ffDiaporamaStart\ && $(MAKE) -f $(MAKEFILE) uninstall
src\ffDiaporamaMMFiler\$(MAKEFILE): 
	@$(CHK_DIR_EXISTS) src\ffDiaporamaMMFiler\ $(MKDIR) src\ffDiaporamaMMFiler\ 
	cd src\ffDiaporamaMMFiler\ && $(QMAKE) y:\ffDiaporama\trunk\ffDiaporama\src\ffDiaporamaMMFiler\ffDiaporamaMMFiler.pro -spec c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\win32-g++ CONFIG+=release -o $(MAKEFILE)
sub-src-ffDiaporamaMMFiler-qmake_all:  FORCE
	@$(CHK_DIR_EXISTS) src\ffDiaporamaMMFiler\ $(MKDIR) src\ffDiaporamaMMFiler\ 
	cd src\ffDiaporamaMMFiler\ && $(QMAKE) y:\ffDiaporama\trunk\ffDiaporama\src\ffDiaporamaMMFiler\ffDiaporamaMMFiler.pro -spec c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\win32-g++ CONFIG+=release -o $(MAKEFILE)
sub-src-ffDiaporamaMMFiler: src\ffDiaporamaMMFiler\$(MAKEFILE) FORCE
	cd src\ffDiaporamaMMFiler\ && $(MAKE) -f $(MAKEFILE)
sub-src-ffDiaporamaMMFiler-make_default-ordered: src\ffDiaporamaMMFiler\$(MAKEFILE) sub-src-ffDiaporamaStart-make_default-ordered  FORCE
	cd src\ffDiaporamaMMFiler\ && $(MAKE) -f $(MAKEFILE) 
sub-src-ffDiaporamaMMFiler-make_default: src\ffDiaporamaMMFiler\$(MAKEFILE) FORCE
	cd src\ffDiaporamaMMFiler\ && $(MAKE) -f $(MAKEFILE) 
sub-src-ffDiaporamaMMFiler-make_first-ordered: src\ffDiaporamaMMFiler\$(MAKEFILE) sub-src-ffDiaporamaStart-make_first-ordered  FORCE
	cd src\ffDiaporamaMMFiler\ && $(MAKE) -f $(MAKEFILE) first
sub-src-ffDiaporamaMMFiler-make_first: src\ffDiaporamaMMFiler\$(MAKEFILE) FORCE
	cd src\ffDiaporamaMMFiler\ && $(MAKE) -f $(MAKEFILE) first
sub-src-ffDiaporamaMMFiler-all-ordered: src\ffDiaporamaMMFiler\$(MAKEFILE) sub-src-ffDiaporamaStart-all-ordered  FORCE
	cd src\ffDiaporamaMMFiler\ && $(MAKE) -f $(MAKEFILE) all
sub-src-ffDiaporamaMMFiler-all: src\ffDiaporamaMMFiler\$(MAKEFILE) FORCE
	cd src\ffDiaporamaMMFiler\ && $(MAKE) -f $(MAKEFILE) all
sub-src-ffDiaporamaMMFiler-clean-ordered: src\ffDiaporamaMMFiler\$(MAKEFILE) sub-src-ffDiaporamaStart-clean-ordered  FORCE
	cd src\ffDiaporamaMMFiler\ && $(MAKE) -f $(MAKEFILE) clean
sub-src-ffDiaporamaMMFiler-clean: src\ffDiaporamaMMFiler\$(MAKEFILE) FORCE
	cd src\ffDiaporamaMMFiler\ && $(MAKE) -f $(MAKEFILE) clean
sub-src-ffDiaporamaMMFiler-distclean-ordered: src\ffDiaporamaMMFiler\$(MAKEFILE) sub-src-ffDiaporamaStart-distclean-ordered  FORCE
	cd src\ffDiaporamaMMFiler\ && $(MAKE) -f $(MAKEFILE) distclean
sub-src-ffDiaporamaMMFiler-distclean: src\ffDiaporamaMMFiler\$(MAKEFILE) FORCE
	cd src\ffDiaporamaMMFiler\ && $(MAKE) -f $(MAKEFILE) distclean
sub-src-ffDiaporamaMMFiler-install_subtargets-ordered: src\ffDiaporamaMMFiler\$(MAKEFILE) sub-src-ffDiaporamaStart-install_subtargets-ordered  FORCE
	cd src\ffDiaporamaMMFiler\ && $(MAKE) -f $(MAKEFILE) install
sub-src-ffDiaporamaMMFiler-install_subtargets: src\ffDiaporamaMMFiler\$(MAKEFILE) FORCE
	cd src\ffDiaporamaMMFiler\ && $(MAKE) -f $(MAKEFILE) install
sub-src-ffDiaporamaMMFiler-uninstall_subtargets-ordered: src\ffDiaporamaMMFiler\$(MAKEFILE) sub-src-ffDiaporamaStart-uninstall_subtargets-ordered  FORCE
	cd src\ffDiaporamaMMFiler\ && $(MAKE) -f $(MAKEFILE) uninstall
sub-src-ffDiaporamaMMFiler-uninstall_subtargets: src\ffDiaporamaMMFiler\$(MAKEFILE) FORCE
	cd src\ffDiaporamaMMFiler\ && $(MAKE) -f $(MAKEFILE) uninstall

Makefile: ffDiaporama.pro  c:/Qt/qt-everywhere-opensource-src-4.8.0/mkspecs/win32-g++/qmake.conf c:/Qt/qt-everywhere-opensource-src-4.8.0/mkspecs/qconfig.pri \
		c:/Qt/qt-everywhere-opensource-src-4.8.0/mkspecs/features/qt_functions.prf \
		c:/Qt/qt-everywhere-opensource-src-4.8.0/mkspecs/features/qt_config.prf \
		c:/Qt/qt-everywhere-opensource-src-4.8.0/mkspecs/features/exclusive_builds.prf \
		c:/Qt/qt-everywhere-opensource-src-4.8.0/mkspecs/features/default_pre.prf \
		c:/Qt/qt-everywhere-opensource-src-4.8.0/mkspecs/features/win32/default_pre.prf \
		ffDiaporama.pri \
		c:/Qt/qt-everywhere-opensource-src-4.8.0/mkspecs/features/release.prf \
		c:/Qt/qt-everywhere-opensource-src-4.8.0/mkspecs/features/debug_and_release.prf \
		c:/Qt/qt-everywhere-opensource-src-4.8.0/mkspecs/features/default_post.prf \
		c:/Qt/qt-everywhere-opensource-src-4.8.0/mkspecs/features/win32/default_post.prf \
		c:/Qt/qt-everywhere-opensource-src-4.8.0/mkspecs/features/win32/rtti.prf \
		c:/Qt/qt-everywhere-opensource-src-4.8.0/mkspecs/features/win32/exceptions.prf \
		c:/Qt/qt-everywhere-opensource-src-4.8.0/mkspecs/features/win32/stl.prf \
		c:/Qt/qt-everywhere-opensource-src-4.8.0/mkspecs/features/shared.prf \
		c:/Qt/qt-everywhere-opensource-src-4.8.0/mkspecs/features/warn_on.prf \
		c:/Qt/qt-everywhere-opensource-src-4.8.0/mkspecs/features/qt.prf \
		c:/Qt/qt-everywhere-opensource-src-4.8.0/mkspecs/features/win32/thread.prf \
		c:/Qt/qt-everywhere-opensource-src-4.8.0/mkspecs/features/moc.prf \
		c:/Qt/qt-everywhere-opensource-src-4.8.0/mkspecs/features/win32/windows.prf \
		c:/Qt/qt-everywhere-opensource-src-4.8.0/mkspecs/features/resources.prf \
		c:/Qt/qt-everywhere-opensource-src-4.8.0/mkspecs/features/uic.prf \
		c:/Qt/qt-everywhere-opensource-src-4.8.0/mkspecs/features/yacc.prf \
		c:/Qt/qt-everywhere-opensource-src-4.8.0/mkspecs/features/lex.prf \
		c:/Qt/qt-everywhere-opensource-src-4.8.0/mkspecs/features/include_source_dir.prf
	$(QMAKE) -spec c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\win32-g++ CONFIG+=release -o Makefile ffDiaporama.pro
c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\qconfig.pri:
c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\features\qt_functions.prf:
c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\features\qt_config.prf:
c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\features\exclusive_builds.prf:
c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\features\default_pre.prf:
c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\features\win32\default_pre.prf:
ffDiaporama.pri:
c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\features\release.prf:
c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\features\debug_and_release.prf:
c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\features\default_post.prf:
c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\features\win32\default_post.prf:
c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\features\win32\rtti.prf:
c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\features\win32\exceptions.prf:
c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\features\win32\stl.prf:
c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\features\shared.prf:
c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\features\warn_on.prf:
c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\features\qt.prf:
c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\features\win32\thread.prf:
c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\features\moc.prf:
c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\features\win32\windows.prf:
c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\features\resources.prf:
c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\features\uic.prf:
c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\features\yacc.prf:
c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\features\lex.prf:
c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\features\include_source_dir.prf:
qmake: qmake_all FORCE
	@$(QMAKE) -spec c:\Qt\qt-everywhere-opensource-src-4.8.0\mkspecs\win32-g++ CONFIG+=release -o Makefile ffDiaporama.pro

qmake_all: sub-src-ffDiaporama-qmake_all sub-src-ffDiaporamaStart-qmake_all sub-src-ffDiaporamaMMFiler-qmake_all FORCE

make_default: sub-src-ffDiaporama-make_default-ordered sub-src-ffDiaporamaStart-make_default-ordered sub-src-ffDiaporamaMMFiler-make_default-ordered FORCE
make_first: sub-src-ffDiaporama-make_first-ordered sub-src-ffDiaporamaStart-make_first-ordered sub-src-ffDiaporamaMMFiler-make_first-ordered FORCE
all: sub-src-ffDiaporama-all-ordered sub-src-ffDiaporamaStart-all-ordered sub-src-ffDiaporamaMMFiler-all-ordered FORCE
clean: sub-src-ffDiaporama-clean-ordered sub-src-ffDiaporamaStart-clean-ordered sub-src-ffDiaporamaMMFiler-clean-ordered FORCE
distclean: sub-src-ffDiaporama-distclean-ordered sub-src-ffDiaporamaStart-distclean-ordered sub-src-ffDiaporamaMMFiler-distclean-ordered FORCE
	-$(DEL_FILE) Makefile
install_subtargets: sub-src-ffDiaporama-install_subtargets-ordered sub-src-ffDiaporamaStart-install_subtargets-ordered sub-src-ffDiaporamaMMFiler-install_subtargets-ordered FORCE
uninstall_subtargets: sub-src-ffDiaporama-uninstall_subtargets-ordered sub-src-ffDiaporamaStart-uninstall_subtargets-ordered sub-src-ffDiaporamaMMFiler-uninstall_subtargets-ordered FORCE

sub-src-ffDiaporama-sub_Debug_ordered: src\ffDiaporama\$(MAKEFILE)
	cd src\ffDiaporama\ && $(MAKE) -f $(MAKEFILE) debug
sub-src-ffDiaporamaStart-sub_Debug_ordered: src\ffDiaporamaStart\$(MAKEFILE) sub-src-ffDiaporama-sub_Debug_ordered 
	cd src\ffDiaporamaStart\ && $(MAKE) -f $(MAKEFILE) debug
sub-src-ffDiaporamaMMFiler-sub_Debug_ordered: src\ffDiaporamaMMFiler\$(MAKEFILE) sub-src-ffDiaporamaStart-sub_Debug_ordered 
	cd src\ffDiaporamaMMFiler\ && $(MAKE) -f $(MAKEFILE) debug
debug: sub-src-ffDiaporama-sub_Debug_ordered sub-src-ffDiaporamaStart-sub_Debug_ordered sub-src-ffDiaporamaMMFiler-sub_Debug_ordered

sub-src-ffDiaporama-sub_Release_ordered: src\ffDiaporama\$(MAKEFILE)
	cd src\ffDiaporama\ && $(MAKE) -f $(MAKEFILE) release
sub-src-ffDiaporamaStart-sub_Release_ordered: src\ffDiaporamaStart\$(MAKEFILE) sub-src-ffDiaporama-sub_Release_ordered 
	cd src\ffDiaporamaStart\ && $(MAKE) -f $(MAKEFILE) release
sub-src-ffDiaporamaMMFiler-sub_Release_ordered: src\ffDiaporamaMMFiler\$(MAKEFILE) sub-src-ffDiaporamaStart-sub_Release_ordered 
	cd src\ffDiaporamaMMFiler\ && $(MAKE) -f $(MAKEFILE) release
release: sub-src-ffDiaporama-sub_Release_ordered sub-src-ffDiaporamaStart-sub_Release_ordered sub-src-ffDiaporamaMMFiler-sub_Release_ordered

sub-src-ffDiaporama-check_ordered: src\ffDiaporama\$(MAKEFILE)
	cd src\ffDiaporama\ && $(MAKE) -f $(MAKEFILE) check
sub-src-ffDiaporamaStart-check_ordered: src\ffDiaporamaStart\$(MAKEFILE) sub-src-ffDiaporama-check_ordered 
	cd src\ffDiaporamaStart\ && $(MAKE) -f $(MAKEFILE) check
sub-src-ffDiaporamaMMFiler-check_ordered: src\ffDiaporamaMMFiler\$(MAKEFILE) sub-src-ffDiaporamaStart-check_ordered 
	cd src\ffDiaporamaMMFiler\ && $(MAKE) -f $(MAKEFILE) check
check: sub-src-ffDiaporama-check_ordered sub-src-ffDiaporamaStart-check_ordered sub-src-ffDiaporamaMMFiler-check_ordered

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all
install: install_subtargets  FORCE

uninstall:  uninstall_subtargets FORCE

FORCE:

